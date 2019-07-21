"""
# Xytronic LF-1600
# Open Source firmware
# R/C network calculation
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
	"RCNet",
]

class RCNet(object):
	"""Discrete R/C network calculation.
	"""

	__slots__ = (
		"r", # Resistor value, in Ohms.
		"c", # Capacitor value, in Farad.
		"v", # Capacitor voltage.
		"i", # Network current, in Ampere.
	)

	def __init__(self, r, c, v=None, q=None):
		self.r = r
		self.c = c
		self.v = v or 0.0
		if q is not None:
			assert(v is None)
			self.q = q
		self.i = 0.0

	@property
	def q(self):
		"""Get charge, in Coulomb.
		"""
		return self.c * self.v

	@q.setter
	def q(self, newQ):
		"""Set charge, in Coulomb.
		"""
		self.v = newQ / self.c

	def calc(self, vIn, dt):
		"""Calculate the next discrete step.
		Returns the new C voltage.
		"""
		c, r = self.c, self.r
		dv = vIn - self.v
		fact = math.exp((-1.0 / (r * c)) * dt)
		self.i = (dv / r) * fact
		self.v += dv * (1.0 - fact)
		return self.v

	def __str__(self):
		return "v=%.3f V, i=%.3f mA, q=%.3f uC" % (
			self.v,
			self.i * 1e3,
			self.q * 1e6)

if __name__ == "__main__":
	import matplotlib.pyplot as plt

	rc = RCNet(r=(10e3),
		   c=(220e-6),
		   q=0.0)

	dt = 0.01
	times = [t * dt for t in range(500)]
	values = []
	for t in times:
		rc.calc(vIn=(5.0 if t <= 3.0 else -2.0),
			dt=dt)
		print("t=%.3f s, %s" % (t, str(rc)))
		values.append((rc.q * 1e6, rc.i * 1e3, rc.v))

	fig, ax = plt.subplots(3, sharex=True)
	ax[0].plot(times, [val[0] for val in values], label="Q (uC)")
	ax[0].legend()
	ax[1].plot(times, [val[1] for val in values], label="I (mA)")
	ax[1].legend()
	ax[2].plot(times, [val[2] for val in values], label="U (V)")
	ax[2].legend()
	ax[2].set_xlabel("t (s)")
	plt.show()
