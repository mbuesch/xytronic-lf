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
import sys



class Simulator(object):
	CURR_DIV	= 10.0
	ADC_TEMP	= 1
	ADC_CURRENT	= 2
	PWM_CURRENT	= 1

	def __init__(self):
		self.xy = pyxytronic
		self.__uartbuf = bytearray()
		self.__reset_debuginterface()

	def run(self):
		self.xy.simulator_mainloop_once()
		self.__handle_debuginterface()

	def pwm_current_get(self):
		value, maxValue = self.xy.simulator_pwm_get(self.PWM_CURRENT)
		amps = self.__scale(raw=value,
				    raw_lo=maxValue,
				    raw_hi=0,
				    phys_lo=0.0,
				    phys_hi=5.0)
		return amps

	def adc_temp_set(self, degree):
		value = self.__unscale(phys=degree,
				       phys_lo=150.0,
				       phys_hi=480.0,
				       raw_lo=210,
				       raw_hi=411)
		self.xy.simulator_adc_set(self.ADC_TEMP, value)

	def adc_current_set(self, amps):
		value = self.__unscale(phys=amps,
				       phys_lo=0.0,
				       phys_hi=1.1,
				       raw_lo=0,
				       raw_hi=65)
		self.xy.simulator_adc_set(self.ADC_CURRENT, value)

	def __do_scale(self, raw, raw_lo, raw_hi, phys_lo, phys_hi):
		#        (phys_hi - phys_lo) * (raw - raw_lo)
		# ret = -------------------------------------- + phys_lo
		#                 raw_hi - raw_lo
		a = (phys_hi - phys_lo) * (raw - raw_lo)
		b = raw_hi - raw_lo
		ret = (a / b) + phys_lo
		return ret

	def __scale(self, raw, raw_lo, raw_hi, phys_lo, phys_hi):
		return float(self.__do_scale(int(raw), int(raw_lo), int(raw_hi),
					     float(phys_lo), float(phys_hi)))

	def __unscale(self, phys, phys_lo, phys_hi, raw_lo, raw_hi):
		return int(round(self.__do_scale(float(phys), float(phys_lo), float(phys_hi),
						 int(raw_lo), int(raw_hi))))

	def __uart_tx_get_line(self):
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

	def __reset_debuginterface(self):
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

	def __handle_debuginterface(self):
		line = self.__uart_tx_get_line()
		if not line:
			return

		if line == "st":
			self.__reset_debuginterface()
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
			self.__dbg_currentRealR = self.__parseFixpt(elems[1], "cr1") / self.CURR_DIV
		elif elems[0] == "cr2":
			self.__dbg_currentUsedR = self.__parseFixpt(elems[1], "cr2") / self.CURR_DIV
		elif elems[0] == "rs":
			self.__dbg_currentRState = self.__parseInt(elems[1], "rs")
		elif elems[0] == "cy":
			self.__dbg_currentY = self.__parseFixpt(elems[1], "cy") / self.CURR_DIV
		elif elems[0] == "tr":
			self.__dbg_tempR = self.__parseFixpt(elems[1], "tr")
		elif elems[0] == "ty1":
			self.__dbg_tempY1 = self.__parseFixpt(elems[1], "ty1")
		elif elems[0] == "ty2":
			self.__dbg_tempY2 = self.__parseFixpt(elems[1], "ty2") / self.CURR_DIV
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

		self.info("temp-r: %.02f °C   current-y: %.02f A   current-r: %.02f A" % (
			self.__dbg_tempR,
			self.__dbg_currentY,
			self.__dbg_currentUsedR,
		))

	@classmethod
	def error(cls, msg):
		print(msg)

	@classmethod
	def info(cls, msg):
		print(msg)

def main():
	pyxytronic.simulator_init()
	try:
		sim = Simulator()
		while 1:
			sim.run()
			sim.adc_temp_set(100)
			sim.adc_current_set(4)
			time.sleep(0.0005)
	finally:
		pyxytronic.simulator_exit()
	return 0

if __name__ == "__main__":
	sys.exit(main())
