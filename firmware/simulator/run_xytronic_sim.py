#!/usr/bin/env python3
"""
# Xytronic LF-1600
# Open Source firmware
# Simulator
#
# Copyright (c) 2018 Michael Buesch <m@bues.ch>
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
"""

import pyxytronic
import time


CURR_DIV = 10.0


class Simulator(object):
	def run(self):
		self.__uartbuf = bytearray()
		self.reset_debuginterface()

		self.xy = pyxytronic
		self.xy.simulator_init()
		try:
			while 1:
				self.xy.simulator_mainloop_once()
				self.handle_debuginterface()
				time.sleep(0.0005)
		finally:
			self.xy.simulator_exit()

	def uart_tx_get_line(self):
		data = self.xy.simulator_uart_get_tx()
		if data:
			self.__uartbuf += data
			i = self.__uartbuf.find(b'\r\n')
			if i >= 0:
				line = self.__uartbuf[:i]
				del self.__uartbuf[:i+2]
				if line:
					return line.decode("utf-8", "ignore").strip()
		return ""

	def reset_debuginterface(self):
		self.__dbg_currentRealR = 0.0
		self.__dbg_currentUsedR = 0.0
		self.__dbg_currentRState = 0
		self.__dbg_currentY = 0.0
		self.__dbg_tempR = 0.0
		self.__dbg_tempY1 = 0.0
		self.__dbg_tempY2 = 0.0
		self.__dbg_measCurr = 0
		self.__dbg_filtCurr = 0
		self.__dbg_measTemp = 0
		self.__dbg_boostMode = 0
		self.__dbg_calCurrPercent = 0

	@classmethod
	def __parseInt(cls, valStr, valIdent):
		try:
			val = int(valStr, 10)
		except ValueError:
			cls.error("Invalid %s" % valIdent)
			return 0
		return val

	@classmethod
	def __parseFixpt(cls, valStr, valIdent):
		val = cls.__parseInt(valStr, valIdent)
		val = float(val) / (1 << 6)
		return val

	def handle_debuginterface(self):
		line = self.uart_tx_get_line()
		if not line:
			return

		if line == "st":
			self.reset_debuginterface()
			return

		i = line.find(':')
		if i < 0:
			self.error("Time stamp not found")
			return
		try:
			timeStamp = int(line[:i], 16)
		except ValueError:
			self.error("Invalid time stamp format")
			return
		line = line[i+1:].strip()

		elems = line.split()
		if len(elems) != 2:
			self.error("Unknown format: %s" % line)
			return
		if elems[0] == "cr1":
			self.__dbg_currentRealR = self.__parseFixpt(elems[1], "cr1") / CURR_DIV
		elif elems[0] == "cr2":
			self.__dbg_currentUsedR = self.__parseFixpt(elems[1], "cr2") / CURR_DIV
		elif elems[0] == "rs":
			self.__dbg_currentRState = self.__parseInt(elems[1], "rs")
		elif elems[0] == "cy":
			self.__dbg_currentY = self.__parseFixpt(elems[1], "cy") / CURR_DIV
		elif elems[0] == "tr":
			self.__dbg_tempR = self.__parseFixpt(elems[1], "tr")
		elif elems[0] == "ty1":
			self.__dbg_tempY1 = self.__parseFixpt(elems[1], "ty1")
		elif elems[0] == "ty2":
			self.__dbg_tempY2 = self.__parseFixpt(elems[1], "ty2") / CURR_DIV
		elif elems[0] == "tb":
			self.__dbg_boostMode = self.__parseInt(elems[1], "tb")
		elif elems[0] == "mc":
			self.__dbg_measCurr = self.__parseInt(elems[1], "mc")
		elif elems[0] == "fc":
			self.__dbg_filtCurr = self.__parseInt(elems[1], "fc")
		elif elems[0] == "mt":
			self.__dbg_measTemp = self.__parseInt(elems[1], "mt")
		elif elems[0] == "cc":
			self.__dbg_calCurrPercent = self.__parseInt(elems[1], "cc")
		else:
			self.error("Unknown elem: %s" % elems[0])
			return

		print("temp-r: %.02f °C   current-y: %.02f A   current-r: %.02f A" % (
			self.__dbg_tempR,
			self.__dbg_currentY,
			self.__dbg_currentUsedR,
		))

	@classmethod
	def error(cls, msg):
		print(msg)

sim = Simulator()
sim.run()
