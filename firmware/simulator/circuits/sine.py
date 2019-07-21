"""
# Xytronic LF-1600
# Open Source firmware
# Sine calculation
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

import math

__all__ = [
	"SineGenerator",
]

class SineGenerator(object):
	__slots__ = (
		"f",
		"peakV",
		"cycleT",
		"v",
	)

	def __init__(self, f, rmsV=5.0):
		self.f = max(f, 0.0)
		self.peakV = rmsV * math.sqrt(2)
		self.cycleT = 0.0
		self.v = 0.0

	@property
	def rmsV(self):
		return self.peakV / math.sqrt(2)

	@property
	def cycleDur(self):
		return 1.0 / self.f

	def calc(self, dt):
		if self.f:
			self.cycleT += dt
			self.v = self.peakV * math.sin(math.pi * 2 *
						       (self.cycleT / self.cycleDur))
		return self.v

if __name__ == "__main__":
	import time
	sine = SineGenerator(f=0.5)
	dt = 0.1
	i = 0.0
	while 1:
		v = sine.calc(dt)
		print("t=%.1f, out=%.1f" % (i, v))
		time.sleep(dt)
		i += dt
