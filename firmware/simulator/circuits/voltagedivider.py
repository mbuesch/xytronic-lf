"""
# Xytronic LF-1600
# Open Source firmware
# Voltage divider calculation
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


class VoltageDivider(object):
	"""Voltage divider calculation.
	"""

	__slots__ = (
		"r1",
		"r2",
		"i",
	)

	def __init__(self, r1, r2, i=0.0):
		self.r1 = r1
		self.r2 = r2
		self.i = i

	@property
	def u1(self):
		return self.i * self.r1

	@property
	def u2(self):
		return self.i * self.r2

	def calc(self, uIn):
		self.i = uIn / (self.r1 + self.r2)
		return self.u2
