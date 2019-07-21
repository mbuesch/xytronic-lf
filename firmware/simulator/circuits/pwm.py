"""
# Xytronic LF-1600
# Open Source firmware
# PWM calculation
#
# Copyright (c) 2019 Michael Buesch <m@bues.ch>
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

__all__ = [
	"PWMGenerator",
]

class PWMGenerator(object):
	__slots__ = (
		"f",
		"duty",
		"cycleT",
		"out",
		"__errors",
	)

	def __init__(self, f, duty=0.0):
		self.f = max(f, 0.0)
		self.duty = max(min(duty, 1.0), 0.0)
		self.cycleT = 0.0
		self.out = False
		self.__errors = 0

	@property
	def cycleDur(self):
		return 1.0 / self.f

	def calc(self, dt):
		if self.f:
			self.cycleT += dt
			dur = self.cycleDur
			wrap = False
			if self.out:
				if self.cycleT >= dur:
					if self.duty < 1.0:
						self.out = False
					wrap = True
			else:
				if self.cycleT >= dur * (1.0 - self.duty):
					if self.duty > 0.0:
						self.out = True
					else:
						wrap = True
			if wrap:
				self.cycleT -= dur
				if self.cycleT >= dur:
					if self.__errors < 10:
						print("The PWM frequency %f Hz is "
						      "too fast. Loosing steps." % (
						      self.f))
						self.__errors += 1
					self.cycleT = 0.0
		return self.out

if __name__ == "__main__":
	import time
	pwm = PWMGenerator(f=0.5, duty=0.3)
	dt = 0.1
	i = 0.0
	while 1:
		out = pwm.calc(dt)
		print("t=%.1f, out=%d, cycleT=%.3f" % (i, int(out), pwm.cycleT))
		time.sleep(dt)
		i += dt
