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
	dbg_pidTempE = 0.0
	dbg_pidTempP = 0.0
	dbg_pidTempI = 0.0
	dbg_pidTempD = 0.0
	dbg_pidTempPrevE = 0.0
	dbg_pidCurrE = 0.0
	dbg_pidCurrP = 0.0
	dbg_pidCurrI = 0.0
	dbg_pidCurrD = 0.0
	dbg_pidCurrPrevE = 0.0

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
		meas.dbg_pidTempE	= self.__sim.dbg_pidTempE
		meas.dbg_pidTempP	= self.__sim.dbg_pidTempP
		meas.dbg_pidTempI	= self.__sim.dbg_pidTempI
		meas.dbg_pidTempD	= self.__sim.dbg_pidTempD
		meas.dbg_pidTempPrevE	= self.__sim.dbg_pidTempPrevE
		meas.dbg_pidCurrE	= self.__sim.dbg_pidCurrE
		meas.dbg_pidCurrP	= self.__sim.dbg_pidCurrP
		meas.dbg_pidCurrI	= self.__sim.dbg_pidCurrI
		meas.dbg_pidCurrD	= self.__sim.dbg_pidCurrD
		meas.dbg_pidCurrPrevE	= self.__sim.dbg_pidCurrPrevE
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

class Chart(QChart):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

class MainChart(Chart):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self.lineSeries = {}
		self.timeAxis = None
		self.tempAxis = None
		self.currentAxis = None
		self.adcAxis = None
		self.stateAxis = None

	def rebuildChart(self,
			 enaLineCurrentRealR,
			 enaLineCurrentUsedR,
			 enaLineCurrentRState,
			 enaLineCurrentY,
			 enaLineTempR,
			 enaLineTempY1,
			 enaLineTempY2,
			 enaLineMeasCurr,
			 enaLineFiltCurr,
			 enaLineMeasTemp,
			 enaLineBoostMode):
		self.removeAllSeries()

		if self.timeAxis is None:
			self.timeAxis = QValueAxis()
			self.addAxis(self.timeAxis, Qt.AlignBottom)
		self.timeAxis.setRange(0, 100)
		self.timeAxis.setLabelFormat("%d s")
		self.timeAxis.setTickCount(10)

		if self.currentAxis is None:
			self.currentAxis = QValueAxis()
			self.addAxis(self.currentAxis, Qt.AlignLeft)
		self.currentAxis.setLabelFormat("%.1f A")
		self.currentAxis.setRange(0, 5)

		if self.tempAxis is None:
			self.tempAxis = QValueAxis()
			self.addAxis(self.tempAxis, Qt.AlignLeft)
		self.tempAxis.setLabelFormat("%d *C")
		self.tempAxis.setRange(0, 600)

		if self.adcAxis is None:
			self.adcAxis = QValueAxis()
			self.addAxis(self.adcAxis, Qt.AlignRight)
		self.adcAxis.setLabelFormat("0x%X")
		self.adcAxis.setRange(0, 0x3FF)

		if self.stateAxis is None:
			self.stateAxis = QValueAxis()
			self.addAxis(self.stateAxis, Qt.AlignRight)
		self.stateAxis.setTickCount(3)
		self.stateAxis.setLabelFormat("%d")
		self.stateAxis.setRange(0, 2)

		self.lineSeries = {}
		for name, description, ena, axis in (
				("crr", "Current real R (A)", enaLineCurrentRealR, self.currentAxis),
				("cur", "Current used R (A)", enaLineCurrentUsedR, self.currentAxis),
				("crs", "Current R state", enaLineCurrentRState, self.stateAxis),
				("cy", "Current Y (A)", enaLineCurrentY, self.currentAxis),
				("tr", "Temp R (*C)", enaLineTempR, self.tempAxis),
				("ty1", "Temp Y1 (*C)", enaLineTempY1, self.tempAxis),
				("ty2", "Temp Y2 (A)", enaLineTempY2, self.currentAxis),
				("mc", "Current ADC (hex)", enaLineMeasCurr, self.adcAxis),
				("fc", "Filtered current ADC (hex)", enaLineFiltCurr, self.adcAxis),
				("mt", "Temp ADC (hex)", enaLineMeasTemp, self.adcAxis),
				("bm", "Boost mode", enaLineBoostMode, self.stateAxis)):
			if ena:
				line = QLineSeries()
				line.setName(description)
				self.addSeries(line)
				line.attachAxis(self.timeAxis)
				line.attachAxis(axis)
				self.lineSeries[name] = line

	def updateChart(self, measurements, chartXCount):
		for chartLine, value in (
				(self.lineSeries.get("crr"), measurements.dbg_currentRealR),
				(self.lineSeries.get("cur"), measurements.dbg_currentUsedR),
				(self.lineSeries.get("crs"), measurements.dbg_currentRState),
				(self.lineSeries.get("cy"), measurements.dbg_currentY),
				(self.lineSeries.get("tr"), measurements.dbg_tempR),
				(self.lineSeries.get("ty1"), measurements.dbg_tempY1),
				(self.lineSeries.get("ty2"), measurements.dbg_tempY2),
				(self.lineSeries.get("mc"), measurements.dbg_measCurr),
				(self.lineSeries.get("fc"), measurements.dbg_filtCurr),
				(self.lineSeries.get("mt"), measurements.dbg_measTemp),
				(self.lineSeries.get("bm"), measurements.dbg_boostMode)):
			if chartLine is None:
				continue
			chartLine.append(chartXCount - 1, value)
			if chartXCount > 100:
				chartLine.remove(0)
		if chartXCount > 100:
			self.timeAxis.setRange(chartXCount - 100,
					       chartXCount)

class PidChart(Chart):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self.lineSeries = {}
		self.timeAxis = None
		self.valueAxis = None

	def rebuildChart(self,
			 enaLinePidCE,
			 enaLinePidCP,
			 enaLinePidCI,
			 enaLinePidCD,
			 enaLinePidCPE,
			 enaLinePidTE,
			 enaLinePidTP,
			 enaLinePidTI,
			 enaLinePidTD,
			 enaLinePidTPE):
		self.removeAllSeries()

		if self.timeAxis is None:
			self.timeAxis = QValueAxis()
			self.addAxis(self.timeAxis, Qt.AlignBottom)
		self.timeAxis.setRange(0, 100)
		self.timeAxis.setLabelFormat("%d s")
		self.timeAxis.setTickCount(10)

		if self.valueAxis is None:
			self.valueAxis = QValueAxis()
			self.addAxis(self.valueAxis, Qt.AlignLeft)
		self.valueAxis.setLabelFormat("%.2f")
		self.valueAxis.setRange(-100, 100)

		self.lineSeries = {}
		for name, description, ena, axis in (
				("ce", "PID current e", enaLinePidCE, self.valueAxis),
				("cp", "PID current p", enaLinePidCP, self.valueAxis),
				("ci", "PID current i", enaLinePidCI, self.valueAxis),
				("cd", "PID current d", enaLinePidCD, self.valueAxis),
				("cpe", "PID current prev. e", enaLinePidCPE, self.valueAxis),
				("te", "PID temp e", enaLinePidTE, self.valueAxis),
				("tp", "PID temp p", enaLinePidTP, self.valueAxis),
				("ti", "PID temp i", enaLinePidTI, self.valueAxis),
				("td", "PID temp d", enaLinePidTD, self.valueAxis),
				("tpe", "PID temp prev. e", enaLinePidTPE, self.valueAxis)):
			if ena:
				line = QLineSeries()
				line.setName(description)
				self.addSeries(line)
				line.attachAxis(self.timeAxis)
				line.attachAxis(axis)
				self.lineSeries[name] = line

	def updateChart(self, measurements, chartXCount):
		for chartLine, value in (
				(self.lineSeries.get("ce"), measurements.dbg_pidCurrE),
				(self.lineSeries.get("cp"), measurements.dbg_pidCurrP),
				(self.lineSeries.get("ci"), measurements.dbg_pidCurrI),
				(self.lineSeries.get("cd"), measurements.dbg_pidCurrD),
				(self.lineSeries.get("cpe"), measurements.dbg_pidCurrPrevE),
				(self.lineSeries.get("te"), measurements.dbg_pidTempE),
				(self.lineSeries.get("tp"), measurements.dbg_pidTempP),
				(self.lineSeries.get("ti"), measurements.dbg_pidTempI),
				(self.lineSeries.get("td"), measurements.dbg_pidTempD),
				(self.lineSeries.get("tpe"), measurements.dbg_pidTempPrevE)):
			if chartLine is None:
				continue
			chartLine.append(chartXCount - 1, value)
			if chartXCount > 100:
				chartLine.remove(0)
		if chartXCount > 100:
			self.timeAxis.setRange(chartXCount - 100,
					       chartXCount)

class ChartView(QChartView):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		self.setRenderHint(QPainter.Antialiasing)

class MainWidget(QWidget):
	def __init__(self, parent=None):
		QWidget.__init__(self, parent)
		self.setLayout(QGridLayout())

		self.simProc = None

		chartVBox = QVBoxLayout()

		self.mainChart = MainChart()
		self.mainChartView = ChartView(self.mainChart, self)
		chartVBox.addWidget(self.mainChartView)

		self.pidChart = PidChart()
		self.pidChartView = ChartView(self.pidChart, self)
		chartVBox.addWidget(self.pidChartView)

		self.layout().addLayout(chartVBox, 0, 0, 2, 1)

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

		self.enaLinePidCE = QCheckBox("Current PID e", self)
		self.measGroup.layout().addWidget(self.enaLinePidCE, 4, 0)

		self.enaLinePidCP = QCheckBox("Current PID p", self)
		self.measGroup.layout().addWidget(self.enaLinePidCP, 5, 0)

		self.enaLinePidCI = QCheckBox("Current PID i", self)
		self.measGroup.layout().addWidget(self.enaLinePidCI, 6, 0)

		self.enaLinePidCD = QCheckBox("Current PID d", self)
		self.measGroup.layout().addWidget(self.enaLinePidCD, 7, 0)

		self.enaLinePidCPE = QCheckBox("Current PID prev-e", self)
		self.measGroup.layout().addWidget(self.enaLinePidCPE, 8, 0)

		self.enaLineTempR = QCheckBox("Temp R", self)
		self.enaLineTempR.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLineTempR, 0, 1)

		self.enaLineTempY1 = QCheckBox("Temp Y1", self)
		self.enaLineTempY1.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLineTempY1, 1, 1)

		self.enaLineTempY2 = QCheckBox("Temp Y2", self)
		self.measGroup.layout().addWidget(self.enaLineTempY2, 2, 1)

		self.enaLinePidTE = QCheckBox("Temp PID e", self)
		self.enaLinePidTE.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLinePidTE, 3, 1)

		self.enaLinePidTP = QCheckBox("Temp PID p", self)
		self.enaLinePidTP.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLinePidTP, 4, 1)

		self.enaLinePidTI = QCheckBox("Temp PID i", self)
		self.enaLinePidTI.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLinePidTI, 5, 1)

		self.enaLinePidTD = QCheckBox("Temp PID d", self)
		self.enaLinePidTD.setCheckState(Qt.Checked)
		self.measGroup.layout().addWidget(self.enaLinePidTD, 6, 1)

		self.enaLinePidTPE = QCheckBox("Temp PID prev-e", self)
		self.measGroup.layout().addWidget(self.enaLinePidTPE, 7, 1)

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
		self.enaLinePidCE.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidCP.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidCI.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidCD.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidCPE.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempR.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempY1.stateChanged.connect(self.__handleChartChange)
		self.enaLineTempY2.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidTE.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidTP.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidTI.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidTD.stateChanged.connect(self.__handleChartChange)
		self.enaLinePidTPE.stateChanged.connect(self.__handleChartChange)
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
		self.mainChart.rebuildChart(
			enaLineCurrentRealR=(self.enaLineCurrentRealR.checkState() == Qt.Checked),
			enaLineCurrentUsedR=(self.enaLineCurrentUsedR.checkState() == Qt.Checked),
			enaLineCurrentRState=(self.enaLineCurrentRState.checkState() == Qt.Checked),
			enaLineCurrentY=(self.enaLineCurrentY.checkState() == Qt.Checked),
			enaLineTempR=(self.enaLineTempR.checkState() == Qt.Checked),
			enaLineTempY1=(self.enaLineTempY1.checkState() == Qt.Checked),
			enaLineTempY2=(self.enaLineTempY2.checkState() == Qt.Checked),
			enaLineMeasCurr=(self.enaLineMeasCurr.checkState() == Qt.Checked),
			enaLineFiltCurr=(self.enaLineFiltCurr.checkState() == Qt.Checked),
			enaLineMeasTemp=(self.enaLineMeasTemp.checkState() == Qt.Checked),
			enaLineBoostMode=(self.enaLineBoostMode.checkState() == Qt.Checked),
		)
		self.pidChart.rebuildChart(
			enaLinePidCE=(self.enaLinePidCE.checkState() == Qt.Checked),
			enaLinePidCP=(self.enaLinePidCP.checkState() == Qt.Checked),
			enaLinePidCI=(self.enaLinePidCI.checkState() == Qt.Checked),
			enaLinePidCD=(self.enaLinePidCD.checkState() == Qt.Checked),
			enaLinePidCPE=(self.enaLinePidCPE.checkState() == Qt.Checked),
			enaLinePidTE=(self.enaLinePidTE.checkState() == Qt.Checked),
			enaLinePidTP=(self.enaLinePidTP.checkState() == Qt.Checked),
			enaLinePidTI=(self.enaLinePidTI.checkState() == Qt.Checked),
			enaLinePidTD=(self.enaLinePidTD.checkState() == Qt.Checked),
			enaLinePidTPE=(self.enaLinePidTPE.checkState() == Qt.Checked),
		)
		self.chartXCount = 0

	def __handleValueChange(self):
		temp = float(self.tempSp.value())
		self.simProc.setValues(temp=temp)

	def __chartUpdate(self):
		for ident, args in self.simProc.getResponses():
			if ident == "meas":
				self.chartXCount += 1
				self.mainChart.updateChart(measurements=args,
							   chartXCount=self.chartXCount)
				self.pidChart.updateChart(measurements=args,
							  chartXCount=self.chartXCount)
		self.simProc.requestMeasurements()

class MainWindow(QMainWindow):
	def __init__(self, parent=None):
		QMainWindow.__init__(self, parent)
		self.setWindowTitle("xytronic-lf simulator")

		self.mainWidget = MainWidget(self)
		self.setCentralWidget(self.mainWidget)

		self.resize(1500, 700)

	def closeEvent(self, ev):
		self.mainWidget.simProc.shutdown()

def main():
	qapp = QApplication(sys.argv)
	mainwnd = MainWindow()
	mainwnd.show()
	return qapp.exec_()

if __name__ == "__main__":
	sys.exit(main())
