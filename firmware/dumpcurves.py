#!/usr/bin/env python3

import sys
import serial


inDev = sys.argv[1]
outFile = sys.argv[2]

s = serial.Serial(port = inDev, baudrate = 9600, bytesize = 8,
		  parity = serial.PARITY_NONE, stopbits = 2)

outFd = open(outFile, "wb")
outFd.write(b"time (ticks);current real r (A);current used r (A);current y (A);"
	    b"temp r (*C);temp y1 (*C);temp y2 (A);"
	    b"measured current (ADC);filtered current (ADC);"
	    b"measured temp (ADC);"
	    b"boost mode;"
	    b"calib current percent\r\n")

def reset():
	global outLines
	global prevTimeStamp
	global val_currentRealR
	global val_currentUsedR
	global val_currentY
	global val_tempR
	global val_tempY1
	global val_tempY2
	global val_measCurr
	global val_filtCurr
	global val_measTemp
	global val_boostMode;
	global val_calCurrPercent

	print("RESET")
	outLines = []
	prevTimeStamp = None
	val_currentRealR = 0.0
	val_currentUsedR = 0.0
	val_currentY = 0.0
	val_tempR = 0.0
	val_tempY1 = 0.0
	val_tempY2 = 0.0
	val_measCurr = 0
	val_filtCurr = 0
	val_measTemp = 0
	val_boostMode = 0
	val_calCurrPercent = 0

def putLine(timeStamp):
	csvLine = "%d;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d\r\n" % (
		timeStamp,
		val_currentRealR,
		val_currentUsedR,
		val_currentY,
		val_tempR,
		val_tempY1,
		val_tempY2,
		val_measCurr,
		val_filtCurr,
		val_measTemp,
		val_boostMode,
		val_calCurrPercent,
	)
	outLines.append(csvLine)

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
			val_currentRealR = parseFixpt(elems[1], "cr1")
		elif elems[0] == "cr2":
			val_currentUsedR = parseFixpt(elems[1], "cr2")
		elif elems[0] == "cy":
			val_currentY = parseFixpt(elems[1], "cy")
		elif elems[0] == "tr":
			val_tempR = parseFixpt(elems[1], "tr")
		elif elems[0] == "ty1":
			val_tempY1 = parseFixpt(elems[1], "ty1")
		elif elems[0] == "ty2":
			val_tempY2 = parseFixpt(elems[1], "ty2")
		elif elems[0] == "tb":
			val_boostMode = parseInt(elems[1], "tb")
		elif elems[0] == "mc":
			val_measCurr = parseInt(elems[1], "mc")
		elif elems[0] == "fc":
			val_filtCurr = parseInt(elems[1], "fc")
		elif elems[0] == "mt":
			val_measTemp = parseInt(elems[1], "mt")
		elif elems[0] == "cc":
			val_calCurrPercent = parseInt(elems[1], "cc")
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

		print("temp-r: %.02f   current-y: %.02f   current-r: %.02f" % (
			val_tempR,
			val_currentY,
			val_currentUsedR,
		))
except KeyboardInterrupt as e:
	pass

outFd.write("".join(outLines).encode("utf-8"))
outFd.close()

sys.exit(0)
