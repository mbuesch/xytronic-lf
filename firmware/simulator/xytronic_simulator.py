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
	FIXPT_SIZE	= 24
	FIXPT_SHIFT	= 6

	SETTINGS = {
		"temp_k[0].kp"		: "fixpt_t",
		"temp_k[0].ki"		: "fixpt_t",
		"temp_k[0].kd"		: "fixpt_t",
		"temp_k[0].d_decay_div"	: "fixpt_t",
		"temp_k[1].kp"		: "fixpt_t",
		"temp_k[1].ki"		: "fixpt_t",
		"temp_k[1].kd"		: "fixpt_t",
		"temp_k[1].d_decay_div"	: "fixpt_t",
		"temp_k[2].kp"		: "fixpt_t",
		"temp_k[2].ki"		: "fixpt_t",
		"temp_k[2].kd"		: "fixpt_t",
		"temp_k[2].d_decay_div"	: "fixpt_t",
		"temp_idle_setpoint"	: "fixpt_t",
		"temp_setpoint"		: "fixpt_t",
		"temp_setpoint_active"	: "uint8_t",
		"temp_adj"		: "fixpt_t",
		"serial"		: "uint8_t",
	}

	def __init__(self):
		self.xy = pyxytronic
		self.__uartbuf = bytearray()
		self.__reset_debuginterface()
		self.__runHook = []
		self.stats_ena()

	def addRunHook(self, hook):
		self.__runHook.append(hook)

	def run(self):
		self.xy.simulator_mainloop_once()
		self.__handle_debuginterface()
		for hook in self.__runHook:
			hook();

	def stats_ena(self, mainloop_stats_ena=True):
		self.xy.simulator_stats_ena(mainloop_stats_ena)

	def setting_get(self, name):
		value = self.xy.simulator_setting_read(name)
		if self.SETTINGS[name] == "fixpt_t":
			value = self.__fixptToFloat(value)
		return value

	def setting_set(self, name, value):
		if self.SETTINGS[name] == "fixpt_t":
			value = self.__floatToFixpt(value)
		self.xy.simulator_setting_write(name, value)

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

	@classmethod
	def __floatToFixpt(cls, f):
		f = float(f)
		if f < 0.0:
			return int((f * float(1 << cls.FIXPT_SHIFT)) - 0.5)
		return int((f * float(1 << cls.FIXPT_SHIFT)) + 0.5)

	@classmethod
	def __fixptToFloat(cls, fixpt):
		mask = (1 << cls.FIXPT_SIZE) - 1
		upperMask = (mask >> cls.FIXPT_SHIFT) << cls.FIXPT_SHIFT
		lowerMask = mask ^ upperMask

		# sign
		if fixpt & (1 << (cls.FIXPT_SIZE - 1)):
			fixpt = -((~fixpt + 1) & mask)

		fact = -1 if fixpt < 0 else 1
		fixpt *= fact

		f = float((fixpt & upperMask) >> cls.FIXPT_SHIFT)
		f += float(fixpt & lowerMask) / float(lowerMask + 1)

		f *= fact
		return f

	def __reset_debuginterface(self):
		self.dbg_currentRealR = 0.0
		self.dbg_currentUsedR = 0.0
		self.dbg_currentRState = 0
		self.dbg_currentY = 0.0
		self.dbg_tempR = 0.0
		self.dbg_tempY1 = 0.0
		self.dbg_tempY2 = 0.0
		self.dbg_measCurr = 0
		self.dbg_filtCurr = 0
		self.dbg_measTemp = 0
		self.dbg_boostMode = 0
		self.dbg_pidTempE = 0.0
		self.dbg_pidTempP = 0.0
		self.dbg_pidTempI = 0.0
		self.dbg_pidTempD = 0.0
		self.dbg_pidTempPrevE = 0.0
		self.dbg_pidCurrE = 0.0
		self.dbg_pidCurrP = 0.0
		self.dbg_pidCurrI = 0.0
		self.dbg_pidCurrD = 0.0
		self.dbg_pidCurrPrevE = 0.0

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
		if len(elems) < 2 or len(elems) > 3:
			self.error("Unknown format: %s" % line)
			return
		if elems[0] == "cr1":
			self.dbg_currentRealR = self.__parseFixpt(elems[1], "cr1") / self.CURR_DIV
			return
		elif elems[0] == "cr2":
			self.dbg_currentUsedR = self.__parseFixpt(elems[1], "cr2") / self.CURR_DIV
			return
		elif elems[0] == "rs":
			self.dbg_currentRState = self.__parseInt(elems[1], "rs")
			return
		elif elems[0] == "cy":
			self.dbg_currentY = self.__parseFixpt(elems[1], "cy") / self.CURR_DIV
			return
		elif elems[0] == "tr":
			self.dbg_tempR = self.__parseFixpt(elems[1], "tr")
			return
		elif elems[0] == "ty1":
			self.dbg_tempY1 = self.__parseFixpt(elems[1], "ty1")
			return
		elif elems[0] == "ty2":
			self.dbg_tempY2 = self.__parseFixpt(elems[1], "ty2") / self.CURR_DIV
			return
		elif elems[0] == "tb":
			self.dbg_boostMode = self.__parseInt(elems[1], "tb")
			return
		elif elems[0] == "mc":
			self.dbg_measCurr = self.__parseInt(elems[1], "mc")
			return
		elif elems[0] == "fc":
			self.dbg_filtCurr = self.__parseInt(elems[1], "fc")
			return
		elif elems[0] == "mt":
			self.dbg_measTemp = self.__parseInt(elems[1], "mt")
			return
		elif elems[0] == "pid-t" and len(elems) == 3:
			if elems[1] == "e":
				self.dbg_pidTempE = self.__parseFixpt(elems[2], "pid-t e")
				return
			elif elems[1] == "p":
				self.dbg_pidTempP = self.__parseFixpt(elems[2], "pid-t p")
				return
			elif elems[1] == "i":
				self.dbg_pidTempI = self.__parseFixpt(elems[2], "pid-t i")
				return
			elif elems[1] == "d":
				self.dbg_pidTempD = self.__parseFixpt(elems[2], "pid-t d")
				return
			elif elems[1] == "pe":
				self.dbg_pidTempPrevE = self.__parseFixpt(elems[2], "pid-t pe")
				return
		elif elems[0] == "pid-c" and len(elems) == 3:
			if elems[1] == "e":
				self.dbg_pidCurrE = self.__parseFixpt(elems[2], "pid-c e")
				return
			elif elems[1] == "p":
				self.dbg_pidCurrP = self.__parseFixpt(elems[2], "pid-c p")
				return
			elif elems[1] == "i":
				self.dbg_pidCurrI = self.__parseFixpt(elems[2], "pid-c i")
				return
			elif elems[1] == "d":
				self.dbg_pidCurrD = self.__parseFixpt(elems[2], "pid-c d")
				return
			elif elems[1] == "pe":
				self.dbg_pidCurrPrevE = self.__parseFixpt(elems[2], "pid-c pe")
				return
		self.error("Unknown elem: %s" % elems[0])

	@classmethod
	def error(cls, msg):
		print(msg)

	@classmethod
	def info(cls, msg):
		print(msg)

class IronTempSimulator(object):
	def __init__(self, sim):
		self.sim = sim
		self.sim.addRunHook(self.__run)

	def __run(self):
		self.sim.adc_temp_set(100)#TODO

class IronCurrentSimulator(object):
	def __init__(self, sim):
		self.sim = sim
		self.sim.addRunHook(self.__run)

	def __run(self):
		curY = self.sim.pwm_current_get()
		self.sim.adc_current_set(4)#TODO

def main():
	pyxytronic.simulator_init()
	try:
		sim = Simulator()
		tempSim = IronTempSimulator(sim)
		curSim = IronCurrentSimulator(sim)
		while 1:
			sim.run()
			time.sleep(0.0005)
	finally:
		pyxytronic.simulator_exit()
	return 0

if __name__ == "__main__":
	sys.exit(main())
