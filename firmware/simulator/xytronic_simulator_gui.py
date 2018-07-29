#!/usr/bin/env python3
"""
# Xytronic LF-1600
# Open Source firmware
# Simulator graphical user interface
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
from xytronic_simulator import Simulator, IronTempSimulator, IronCurrentSimulator
import sys
from copy import copy
import multiprocessing
import time
import queue
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtChart import *


class Measurements(object):
	dbg_currentRealR = 0.0
	dbg_currentUsedR = 0.0
	dbg_currentRState = 0
	dbg_currentY = 0.0
	dbg_tempR = 0.0
	dbg_tempY1 = 0.0
	dbg_tempY2 = 0.0
	dbg_measCurr = 0
	dbg_filtCurr = 0
	dbg_measTemp = 0
	dbg_boostMode = 0

class SimulatorProcess(multiprocessing.Process):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs, daemon=False)

	def __oneLoop(self):
		self.__sim.run()
#		time.sleep(0.0005)

	def __updateValues(self, temp):
		self.__sim.setting_set("temp_setpoint_active", 0)
		self.__sim.setting_set("temp_setpoint", temp)

	def setValues(self, temp=350.0):
		self.__commands.put(("temp", temp))

	def __buildMeasurements(self):
		meas = Measurements()
		meas.dbg_currentRealR	= self.__sim.dbg_currentRealR
		meas.dbg_currentUsedR	= self.__sim.dbg_currentUsedR
		meas.dbg_currentRState	= self.__sim.dbg_currentRState
		meas.dbg_currentY	= self.__sim.dbg_currentY
		meas.dbg_tempR		= self.__sim.dbg_tempR
		meas.dbg_tempY1		= self.__sim.dbg_tempY1
		meas.dbg_tempY2		= self.__sim.dbg_tempY2
		meas.dbg_measCurr	= self.__sim.dbg_measCurr
		meas.dbg_filtCurr	= self.__sim.dbg_filtCurr
		meas.dbg_measTemp	= self.__sim.dbg_measTemp
		meas.dbg_boostMode	= self.__sim.dbg_boostMode
		self.__responses.put(("meas", meas))

	def requestMeasurements(self):
		self.__commands.put(("reqMeas", None))

	def getResponses(self):
		try:
			while True:
				ident, args = self.__responses.get(False)
				yield ident, args
		except queue.Empty as e:
			pass

	def start(self):
		self.__commands = multiprocessing.Queue()
		self.__responses = multiprocessing.Queue()
		self.setValues()
		super().start()

	def run(self):
		print("Starting simulator process.")
		pyxytronic.simulator_init()
		try:
			self.__sim = Simulator()
			self.__simTemp = IronTempSimulator(self.__sim)
			self.__simCurr = IronCurrentSimulator(self.__sim)
			run = True
			while run:
				while True:
					try:
						(cmd, args) = self.__commands.get(False)
						if cmd == "stop":
							run = False
						if cmd == "reqMeas":
							self.__buildMeasurements()
						if cmd == "temp":
							self.__updateValues(temp=args)
					except queue.Empty as e:
						break
				self.__oneLoop()
		finally:
			pyxytronic.simulator_exit()
		print("Exiting simulator process.")

	def shutdown(self):
		if self.is_alive():
			self.__commands.put(("stop", None))
			self.join()

class MainChart(QChart):
	def __init__(self):
		super().__init__()

class MainChartView(QChartView):
	def __init__(self, chart, parent=None):
		super().__init__(chart, parent)

class MainWidget(QWidget):
	def __init__(self, parent=None):
		QWidget.__init__(self, parent)
		self.setLayout(QGridLayout())

		self.simProc = None

		self.chart = MainChart()
		self.chartView = MainChartView(self.chart, self)
		self.chartView.setRenderHint(QPainter.Antialiasing)
		self.layout().addWidget(self.chartView, 0, 0, 2, 1)

		self.timeAxis = None
		self.tempAxis = None
		self.currentAxis = None
		self.adcAxis = None
		self.stateAxis = None

		self.measGroup = QGroupBox(self)
		self.measGroup.setLayout(QGridLayout())

		self.enaLineCurrentRealR = QCheckBox("Current real R", self)
		self.measGroup.layout().addWidget(self.enaLineCurrentRealR, 0, 0)

		self.enaLineCurrentUsedR = QCheckBox("Current used R", self)
		self.measGroup.layout().addWidget(self.enaLineCurrentUsedR, 1, 0)

		self.enaLineCurrentRState = QCheckBox("Current R state", self)
		self.measGroup.layout().addWidget(self.enaLineCurrentRState, 2, 0)

		self.enaLineCurrentY = QCheckBox("Current-Y", self)
		self.measGroup.layout().addWidget(self.enaLineCurrentY, 3, 0)

		self.enaLineTempR = QCheckBox("Temp R", self)
		self.enaLineTempR.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLineTempR, 0, 1)

		self.enaLineTempY1 = QCheckBox("Temp Y1", self)
		self.enaLineTempY1.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLineTempY1, 1, 1)

		self.enaLineTempY2 = QCheckBox("Temp Y2", self)
		self.measGroup.layout().addWidget(self.enaLineTempY2, 2, 1)

		self.enaLineMeasCurr = QCheckBox("Current ADC", self)
		self.measGroup.layout().addWidget(self.enaLineMeasCurr, 0, 2)

		self.enaLineFiltCurr = QCheckBox("Filtered current ADC", self)
		self.measGroup.layout().addWidget(self.enaLineFiltCurr, 1, 2)

		self.enaLineMeasTemp = QCheckBox("Temp ADC", self)
		self.measGroup.layout().addWidget(self.enaLineMeasTemp, 2, 2)

		self.enaLineBoostMode = QCheckBox("Boost mode", self)
		self.measGroup.layout().addWidget(self.enaLineBoostMode, 3, 2)

		self.layout().addWidget(self.measGroup, 0, 1)

		self.controlsGroup = QGroupBox(self)
		self.controlsGroup.setLayout(QGridLayout())

		self.resetButton = QPushButton("Reset", self)
		self.controlsGroup.layout().addWidget(self.resetButton, 0, 0, 1, 2)

		label = QLabel("Temp sp:", self)
		self.controlsGroup.layout().addWidget(label, 1, 0)
		self.tempSp = QSlider(Qt.Horizontal, self)
		self.tempSp.setRange(-50, 550)
		self.tempSp.setValue(350)
		self.controlsGroup.layout().addWidget(self.tempSp, 1, 1)

		self.controlsGroup.layout().setRowStretch(99, 1)
		self.layout().addWidget(self.controlsGroup, 1, 1)

		self.chartTimer = QTimer(self)
		self.chartTimer.timeout.connect(self.__chartUpdate)
		self.chartTimer.setSingleShot(False)
		self.chartTimer.setTimerType(Qt.PreciseTimer)
		self.chartTimer.start(100)

		self.resetButton.released.connect(self.__handleReset)
		self.enaLineCurrentRealR.stateChanged.connect(self.__handleChartChange)
		self.enaLineCurrentUsedR.stateChanged.connect(self.__handleChartChange)
		self.enaLineCurrentRState.stateChanged.connect(self.__handleChartChange)
		self.enaLineCurrentY.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempR.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempY1.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempY2.stateChanged.connect(self.__handleChartChange)
		self.enaLineMeasCurr.stateChanged.connect(self.__handleChartChange)
		self.enaLineFiltCurr.stateChanged.connect(self.__handleChartChange)
		self.enaLineMeasTemp.stateChanged.connect(self.__handleChartChange)
		self.enaLineBoostMode.stateChanged.connect(self.__handleChartChange)
		self.tempSp.valueChanged.connect(self.__handleValueChange)

		self.__handleReset()

	def __handleReset(self):
		self.__handleChartChange()

		if self.simProc:
			self.simProc.shutdown()
		self.simProc = SimulatorProcess()
		self.simProc.start()

		self.__handleValueChange()

	def __handleChartChange(self):
		self.chart.removeAllSeries()

		if self.timeAxis is None:
			self.timeAxis = QValueAxis()
			self.chart.addAxis(self.timeAxis, Qt.AlignBottom)
		self.timeAxis.setRange(0, 100)
		self.timeAxis.setLabelFormat("%d s")
		self.timeAxis.setTickCount(10)

		if self.currentAxis is None:
			self.currentAxis = QValueAxis()
			self.chart.addAxis(self.currentAxis, Qt.AlignLeft)
		self.currentAxis.setLabelFormat("%.1f A")
		self.currentAxis.setRange(0, 5)

		if self.tempAxis is None:
			self.tempAxis = QValueAxis()
			self.chart.addAxis(self.tempAxis, Qt.AlignLeft)
		self.tempAxis.setLabelFormat("%d *C")
		self.tempAxis.setRange(0, 600)

		if self.adcAxis is None:
			self.adcAxis = QValueAxis()
			self.chart.addAxis(self.adcAxis, Qt.AlignRight)
		self.adcAxis.setLabelFormat("0x%X")
		self.adcAxis.setRange(0, 0x3FF)

		if self.stateAxis is None:
			self.stateAxis = QValueAxis()
			self.chart.addAxis(self.stateAxis, Qt.AlignRight)
		self.stateAxis.setTickCount(3)
		self.stateAxis.setLabelFormat("%d")
		self.stateAxis.setRange(0, 2)

		self.chartLineCurrentRealR = None
		self.chartLineCurrentUsedR = None
		self.chartLineCurrentRState = None
		self.chartLineCurrentY = None
		self.chartLineTempR = None
		self.chartLineTempY1 = None
		self.chartLineTempY2 = None
		self.chartLineMeasCurr = None
		self.chartLineFiltCurr = None
		self.chartLineMeasTemp = None
		self.chartLineBoostMode = None

		if self.enaLineCurrentRealR.checkState() == Qt.Checked:
			self.chartLineCurrentRealR = QLineSeries()
			self.chartLineCurrentRealR.setName("Current real R (A)")
			self.chart.addSeries(self.chartLineCurrentRealR)
			self.chartLineCurrentRealR.attachAxis(self.timeAxis)
			self.chartLineCurrentRealR.attachAxis(self.currentAxis)

		if self.enaLineCurrentUsedR.checkState() == Qt.Checked:
			self.chartLineCurrentUsedR = QLineSeries()
			self.chartLineCurrentUsedR.setName("Current used R (A)")
			self.chart.addSeries(self.chartLineCurrentUsedR)
			self.chartLineCurrentUsedR.attachAxis(self.timeAxis)
			self.chartLineCurrentUsedR.attachAxis(self.currentAxis)

		if self.enaLineCurrentRState.checkState() == Qt.Checked:
			self.chartLineCurrentRState = QLineSeries()
			self.chartLineCurrentRState.setName("Current R state")
			self.chart.addSeries(self.chartLineCurrentRState)
			self.chartLineCurrentRState.attachAxis(self.timeAxis)
			self.chartLineCurrentRState.attachAxis(self.stateAxis)

		if self.enaLineCurrentY.checkState() == Qt.Checked:
			self.chartLineCurrentY = QLineSeries()
			self.chartLineCurrentY.setName("Current Y (A)")
			self.chart.addSeries(self.chartLineCurrentY)
			self.chartLineCurrentY.attachAxis(self.timeAxis)
			self.chartLineCurrentY.attachAxis(self.currentAxis)

		if self.enaLineTempR.checkState() == Qt.Checked:
			self.chartLineTempR = QLineSeries()
			self.chartLineTempR.setName("Temp R (*C)")
			self.chart.addSeries(self.chartLineTempR)
			self.chartLineTempR.attachAxis(self.timeAxis)
			self.chartLineTempR.attachAxis(self.tempAxis)

		if self.enaLineTempY1.checkState() == Qt.Checked:
			self.chartLineTempY1 = QLineSeries()
			self.chartLineTempY1.setName("Temp Y1 (*C)")
			self.chart.addSeries(self.chartLineTempY1)
			self.chartLineTempY1.attachAxis(self.timeAxis)
			self.chartLineTempY1.attachAxis(self.tempAxis)

		if self.enaLineTempY2.checkState() == Qt.Checked:
			self.chartLineTempY2 = QLineSeries()
			self.chartLineTempY2.setName("Temp Y2 (A)")
			self.chart.addSeries(self.chartLineTempY2)
			self.chartLineTempY2.attachAxis(self.timeAxis)
			self.chartLineTempY2.attachAxis(self.currentAxis)

		if self.enaLineMeasCurr.checkState() == Qt.Checked:
			self.chartLineMeasCurr = QLineSeries()
			self.chartLineMeasCurr.setName("Current ADC (hex)")
			self.chart.addSeries(self.chartLineMeasCurr)
			self.chartLineMeasCurr.attachAxis(self.timeAxis)
			self.chartLineMeasCurr.attachAxis(self.adcAxis)

		if self.enaLineFiltCurr.checkState() == Qt.Checked:
			self.chartLineFiltCurr = QLineSeries()
			self.chartLineFiltCurr.setName("Filtered current ADC (hex)")
			self.chart.addSeries(self.chartLineFiltCurr)
			self.chartLineFiltCurr.attachAxis(self.timeAxis)
			self.chartLineFiltCurr.attachAxis(self.adcAxis)

		if self.enaLineMeasTemp.checkState() == Qt.Checked:
			self.chartLineMeasTemp = QLineSeries()
			self.chartLineMeasTemp.setName("Temp ADC (hex)")
			self.chart.addSeries(self.chartLineMeasTemp)
			self.chartLineMeasTemp.attachAxis(self.timeAxis)
			self.chartLineMeasTemp.attachAxis(self.adcAxis)

		if self.enaLineBoostMode.checkState() == Qt.Checked:
			self.chartLineBoostMode = QLineSeries()
			self.chartLineBoostMode.setName("Boost mode")
			self.chart.addSeries(self.chartLineBoostMode)
			self.chartLineBoostMode.attachAxis(self.timeAxis)
			self.chartLineBoostMode.attachAxis(self.stateAxis)

		self.chartXCount = 0

	def __handleValueChange(self):
		temp = float(self.tempSp.value())
		self.simProc.setValues(temp=temp)

	def __chartUpdate(self):
		for ident, args in self.simProc.getResponses():
			if ident == "meas":
				meas = args
				self.chartXCount += 1
				for chartLine, value in (
						(self.chartLineCurrentRealR, meas.dbg_currentRealR),
						(self.chartLineCurrentUsedR, meas.dbg_currentUsedR),
						(self.chartLineCurrentRState, meas.dbg_currentRState),
						(self.chartLineCurrentY, meas.dbg_currentY),
						(self.chartLineTempR, meas.dbg_tempR),
						(self.chartLineTempY1, meas.dbg_tempY1),
						(self.chartLineTempY2, meas.dbg_tempY2),
						(self.chartLineMeasCurr, meas.dbg_measCurr),
						(self.chartLineFiltCurr, meas.dbg_filtCurr),
						(self.chartLineMeasTemp, meas.dbg_measTemp),
						(self.chartLineBoostMode, meas.dbg_boostMode)):
					if chartLine is None:
						continue
					chartLine.append(self.chartXCount - 1, value)
					if self.chartXCount > 100:
						chartLine.remove(0)
				if self.chartXCount > 100:
					self.chart.axisX().setRange(self.chartXCount - 100,
								    self.chartXCount)
		self.simProc.requestMeasurements()

class MainWindow(QMainWindow):
	def __init__(self, parent=None):
		QMainWindow.__init__(self, parent)
		self.setWindowTitle("xytronic-lf simulator")

		self.mainWidget = MainWidget(self)
		self.setCentralWidget(self.mainWidget)

		self.resize(1100, 500)

	def closeEvent(self, ev):
		self.mainWidget.simProc.shutdown()

def main():
	qapp = QApplication(sys.argv)
	mainwnd = MainWindow()
	mainwnd.show()
	return qapp.exec_()

if __name__ == "__main__":
	sys.exit(main())
