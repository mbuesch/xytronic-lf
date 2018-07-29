#!/usr/bin/env python3
#
# Xytronic LF-1600
# Debug interface tool
#
# Copyright (c) 2015-2016 Michael Buesch <m@bues.ch>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

import sys
import serial


CURR_DIV = 10.0

if len(sys.argv) != 3 or "-h" in sys.argv or "--help" in sys.argv:
	print("Usage: dumpcurves.py /dev/ttyUSB0 output.csv")
	print("")
	print("/dev/ttyUSB0 is the serial device where the data is read from.")
	print("output.csv is the output file.")
	sys.exit(1)

inDev = sys.argv[1]
outFile = sys.argv[2]

s = serial.Serial(port = inDev, baudrate = 9600, bytesize = 8,
		  parity = serial.PARITY_NONE, stopbits = 2)

def writeOutFile(lines):
	outFd = open(outFile, "wb")
	outFd.write(b"time (ticks);"
		    b"current controller real r (A);"
		    b"current controller used r (A);"
		    b"current controller r state;"
		    b"current controller y (A);"
		    b"temp controller r (*C);"
		    b"temp controller y1 (*C);"
		    b"temp controller y2 (A);"
		    b"measured current (ADC);"
		    b"filtered current (ADC);"
		    b"measured temp (ADC);"
		    b"boost mode\r\n")
	outFd.write("".join(lines).encode("utf-8"))
	outFd.close()

def reset():
	global outLines
	global prevTimeStamp
	global val_currentRealR
	global val_currentUsedR
	global val_currentRState
	global val_currentY
	global val_tempR
	global val_tempY1
	global val_tempY2
	global val_measCurr
	global val_filtCurr
	global val_measTemp
	global val_boostMode;

	print("RESET")
	outLines = []
	writeOutFile(outLines)
	prevTimeStamp = None
	val_currentRealR = 0.0
	val_currentUsedR = 0.0
	val_currentRState = 0
	val_currentY = 0.0
	val_tempR = 0.0
	val_tempY1 = 0.0
	val_tempY2 = 0.0
	val_measCurr = 0
	val_filtCurr = 0
	val_measTemp = 0
	val_boostMode = 0

def putLine(timeStamp):
	csvLine = "%d;%f;%f;%d;%f;%f;%f;%f;%d;%d;%d;%d\r\n" % (
		timeStamp,
		val_currentRealR,
		val_currentUsedR,
		val_currentRState,
		val_currentY,
		val_tempR,
		val_tempY1,
		val_tempY2,
		val_measCurr,
		val_filtCurr,
		val_measTemp,
		val_boostMode,
	)
	outLines.append(csvLine)
	writeOutFile(outLines)

def parseInt(valStr, valIdent):
	try:
		val = int(valStr, 10)
	except ValueError:
		print("Invalid %s" % valIdent)
		return 0
	return val

def parseFixpt(valStr, valIdent):
	val = parseInt(valStr, valIdent)
	val = float(val) / (1 << 6)
	return val

reset()
try:
	while 1:
		line = s.readline()
		line = line.decode("utf-8", "ignore").strip()
		if not line:
			continue

		if line == "st":
			reset()
			continue

		idx = line.find(':')
		if idx < 0:
			print("Time stamp not found")
			continue
		try:
			timeStamp = int(line[ : idx], 16)
		except ValueError:
			print("Invalid time stamp format")
			continue
		line = line[idx + 1 : ].strip()

		elems = line.split()
		if len(elems) != 2:
			print("Unknown format: %s" % line)
			continue
		if elems[0] == "cr1":
			val_currentRealR = parseFixpt(elems[1], "cr1") / CURR_DIV
		elif elems[0] == "cr2":
			val_currentUsedR = parseFixpt(elems[1], "cr2") / CURR_DIV
		elif elems[0] == "rs":
			val_currentRState = parseInt(elems[1], "rs")
		elif elems[0] == "cy":
			val_currentY = parseFixpt(elems[1], "cy") / CURR_DIV
		elif elems[0] == "tr":
			val_tempR = parseFixpt(elems[1], "tr")
		elif elems[0] == "ty1":
			val_tempY1 = parseFixpt(elems[1], "ty1")
		elif elems[0] == "ty2":
			val_tempY2 = parseFixpt(elems[1], "ty2") / CURR_DIV
		elif elems[0] == "tb":
			val_boostMode = parseInt(elems[1], "tb")
		elif elems[0] == "mc":
			val_measCurr = parseInt(elems[1], "mc")
		elif elems[0] == "fc":
			val_filtCurr = parseInt(elems[1], "fc")
		elif elems[0] == "mt":
			val_measTemp = parseInt(elems[1], "mt")
		else:
			print("Unknown elem: %s" % elems[0])
			continue

		# Fill the missing lines with copies of the previous.
		if 0:
			if prevTimeStamp is not None:
				t = (prevTimeStamp + 1) & 0xFFFF
				while t != timeStamp:
					putLine(t)
					t = (t + 1) & 0xFFFF
			prevTimeStamp = timeStamp
		# Put the current line into the CSV.
		putLine(timeStamp)

		print("temp-r: %.02f °C   current-y: %.02f A   current-r: %.02f A" % (
			val_tempR,
			val_currentY,
			val_currentUsedR,
		))
except KeyboardInterrupt as e:
	pass

sys.exit(0)
