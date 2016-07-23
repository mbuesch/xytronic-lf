EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:tl431
LIBS:lf1600-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
Title "Xytronic LF-1600"
Date ""
Rev "1.1"
Comp "Michael Büsch <m@bues.ch>"
Comment1 "Reverse engineered"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X03 J3
U 1 1 549557D4
P 2150 1450
F 0 "J3" H 2150 1650 50  0000 C CNN
F 1 "SENS" V 2250 1450 40  0000 C CNN
F 2 "" H 2150 1450 60  0000 C CNN
F 3 "" H 2150 1450 60  0000 C CNN
	1    2150 1450
	-1   0    0    1   
$EndComp
$Comp
L LM6161 U2
U 1 1 54956736
P 6300 5200
F 0 "U2" H 6500 5500 60  0000 C CNN
F 1 "OP07DP" H 6500 5400 60  0000 C CNN
F 2 "" H 6300 5200 60  0000 C CNN
F 3 "" H 6300 5200 60  0000 C CNN
	1    6300 5200
	-1   0    0    1   
$EndComp
NoConn ~ 6200 4900
NoConn ~ 6300 4900
Text GLabel 6400 5600 3    60   Input ~ 0
+5V
Text GLabel 6400 4800 1    60   Input ~ 0
-5V
Text GLabel 6050 2600 3    60   Input ~ 0
+5V
NoConn ~ 6050 1950
Wire Wire Line
	6050 2250 6050 2600
$Comp
L POT VR2
U 1 1 54956921
P 6050 2100
F 0 "VR2" H 6050 2000 50  0000 C CNN
F 1 "10 k" H 6050 2100 50  0000 C CNN
F 2 "~" H 6050 2100 60  0000 C CNN
F 3 "~" H 6050 2100 60  0000 C CNN
	1    6050 2100
	0    -1   -1   0   
$EndComp
$Comp
L POT VR1
U 1 1 54956927
P 5650 4050
F 0 "VR1" H 5650 3950 50  0000 C CNN
F 1 "10 k" H 5650 4050 50  0000 C CNN
F 2 "~" H 5650 4050 60  0000 C CNN
F 3 "~" H 5650 4050 60  0000 C CNN
	1    5650 4050
	0    1    1    0   
$EndComp
NoConn ~ 5650 3900
$Comp
L R R12
U 1 1 54958872
P 3850 5200
F 0 "R12" V 3930 5200 40  0000 C CNN
F 1 "10 k" V 3857 5201 40  0000 C CNN
F 2 "~" V 3780 5200 30  0000 C CNN
F 3 "~" H 3850 5200 30  0000 C CNN
	1    3850 5200
	0    1    1    0   
$EndComp
Text HLabel 2350 5200 0    60   Input ~ 0
temp_measure
Wire Wire Line
	2350 5200 3700 5200
Text HLabel 2350 2450 0    60   Input ~ 0
iron_switch
$Comp
L R R18
U 1 1 549616C5
P 2700 2000
F 0 "R18" V 2780 2000 40  0000 C CNN
F 1 "1 k" V 2707 2001 40  0000 C CNN
F 2 "~" V 2630 2000 30  0000 C CNN
F 3 "~" H 2700 2000 30  0000 C CNN
	1    2700 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 1550 2700 1550
Wire Wire Line
	2700 1550 2700 1850
Wire Wire Line
	2700 2150 2700 2450
Wire Wire Line
	2700 2450 2350 2450
Text GLabel 3050 1450 2    60   Input ~ 0
GND
Wire Wire Line
	2350 1450 3050 1450
Text Label 2450 1550 0    39   ~ 0
SW
Text Label 2450 1450 0    39   ~ 0
-
Text Label 2450 1350 0    39   ~ 0
+
$Comp
L INDUCTOR L5
U 1 1 549634F8
P 4150 1350
F 0 "L5" V 4100 1350 40  0000 C CNN
F 1 "1500 µH / 10 %" V 4250 1350 40  0000 C CNN
F 2 "~" H 4150 1350 60  0000 C CNN
F 3 "~" H 4150 1350 60  0000 C CNN
	1    4150 1350
	0    1    1    0   
$EndComp
$Comp
L C C4
U 1 1 5496366E
P 3700 1700
F 0 "C4" H 3700 1800 40  0000 L CNN
F 1 "22 µF / 63 V" H 3706 1615 40  0000 L CNN
F 2 "~" H 3738 1550 30  0000 C CNN
F 3 "~" H 3700 1700 60  0000 C CNN
	1    3700 1700
	1    0    0    -1  
$EndComp
Text GLabel 3700 2050 3    60   Input ~ 0
GND
Wire Wire Line
	2350 1350 3850 1350
Wire Wire Line
	3700 1350 3700 1550
Connection ~ 3700 1350
Wire Wire Line
	3700 1850 3700 2050
$Comp
L C C3
U 1 1 54963922
P 4600 1700
F 0 "C3" H 4600 1800 40  0000 L CNN
F 1 "22 µF / 63 V" H 4606 1615 40  0000 L CNN
F 2 "~" H 4638 1550 30  0000 C CNN
F 3 "~" H 4600 1700 60  0000 C CNN
	1    4600 1700
	1    0    0    -1  
$EndComp
Text GLabel 4600 2050 3    60   Input ~ 0
GND
Wire Wire Line
	4600 1850 4600 2050
Wire Wire Line
	4450 1350 6350 1350
Wire Wire Line
	4600 1350 4600 1550
Connection ~ 4600 1350
$Comp
L R R17
U 1 1 54963BC9
P 5350 1700
F 0 "R17" V 5430 1700 40  0000 C CNN
F 1 "R" V 5357 1701 40  0000 C CNN
F 2 "~" V 5280 1700 30  0000 C CNN
F 3 "~" H 5350 1700 30  0000 C CNN
	1    5350 1700
	1    0    0    -1  
$EndComp
Text GLabel 5350 2050 3    60   Input ~ 0
+5V
Wire Wire Line
	5350 1350 5350 1550
Connection ~ 5350 1350
Wire Wire Line
	5350 1850 5350 2050
$Comp
L R R19
U 1 1 54963EA1
P 5800 1700
F 0 "R19" V 5880 1700 40  0000 C CNN
F 1 "43 k" V 5807 1701 40  0000 C CNN
F 2 "~" V 5730 1700 30  0000 C CNN
F 3 "~" H 5800 1700 30  0000 C CNN
	1    5800 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 1350 5800 1550
Connection ~ 5800 1350
$Comp
L R R16
U 1 1 5496404D
P 6500 1350
F 0 "R16" V 6580 1350 40  0000 C CNN
F 1 "10 k" V 6507 1351 40  0000 C CNN
F 2 "~" V 6430 1350 30  0000 C CNN
F 3 "~" H 6500 1350 30  0000 C CNN
	1    6500 1350
	0    -1   -1   0   
$EndComp
$Comp
L R L3
U 1 1 5496432E
P 8350 5550
F 0 "L3" V 8430 5550 40  0000 C CNN
F 1 "0 ohm" V 8357 5551 40  0000 C CNN
F 2 "~" V 8280 5550 30  0000 C CNN
F 3 "~" H 8350 5550 30  0000 C CNN
	1    8350 5550
	0    -1   -1   0   
$EndComp
$Comp
L C C17
U 1 1 54964481
P 7350 1350
F 0 "C17" H 7350 1450 40  0000 L CNN
F 1 "47 µF / 16 V" H 7356 1265 40  0000 L CNN
F 2 "~" H 7388 1200 30  0000 C CNN
F 3 "~" H 7350 1350 60  0000 C CNN
	1    7350 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6650 1350 7200 1350
Text GLabel 7750 1350 2    60   Input ~ 0
GND
Wire Wire Line
	7500 1350 7750 1350
Connection ~ 6950 1350
$Comp
L C C15
U 1 1 5496476E
P 7950 5200
F 0 "C15" H 7950 5300 40  0000 L CNN
F 1 "330 pF" H 7956 5115 40  0000 L CNN
F 2 "~" H 7988 5050 30  0000 C CNN
F 3 "~" H 7950 5200 60  0000 C CNN
	1    7950 5200
	1    0    0    -1  
$EndComp
$Comp
L R L1
U 1 1 5496477B
P 7550 5550
F 0 "L1" V 7630 5550 40  0000 C CNN
F 1 "0 ohm" V 7557 5551 40  0000 C CNN
F 2 "~" V 7480 5550 30  0000 C CNN
F 3 "~" H 7550 5550 30  0000 C CNN
	1    7550 5550
	0    -1   -1   0   
$EndComp
$Comp
L C C16
U 1 1 54964A38
P 7150 5200
F 0 "C16" H 7150 5300 40  0000 L CNN
F 1 "330 pF" H 7156 5115 40  0000 L CNN
F 2 "~" H 7188 5050 30  0000 C CNN
F 3 "~" H 7150 5200 60  0000 C CNN
	1    7150 5200
	1    0    0    -1  
$EndComp
$Comp
L R L2
U 1 1 54964A81
P 7550 4850
F 0 "L2" V 7630 4850 40  0000 C CNN
F 1 "0 ohm" V 7557 4851 40  0000 C CNN
F 2 "~" V 7480 4850 30  0000 C CNN
F 3 "~" H 7550 4850 30  0000 C CNN
	1    7550 4850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6600 5300 6950 5300
$Comp
L R L4
U 1 1 54965035
P 8350 4850
F 0 "L4" V 8430 4850 40  0000 C CNN
F 1 "0 ohm" V 8357 4851 40  0000 C CNN
F 2 "~" V 8280 4850 30  0000 C CNN
F 3 "~" H 8350 4850 30  0000 C CNN
	1    8350 4850
	0    -1   -1   0   
$EndComp
$Comp
L D D5
U 1 1 54965466
P 8750 5200
F 0 "D5" H 8750 5300 40  0000 C CNN
F 1 "DIODE" H 8750 5100 40  0000 C CNN
F 2 "~" H 8750 5200 60  0000 C CNN
F 3 "~" H 8750 5200 60  0000 C CNN
	1    8750 5200
	0    1    1    0   
$EndComp
$Comp
L D D6
U 1 1 54965473
P 9100 5200
F 0 "D6" H 9100 5300 40  0000 C CNN
F 1 "DIODE" H 9100 5100 40  0000 C CNN
F 2 "~" H 9100 5200 60  0000 C CNN
F 3 "~" H 9100 5200 60  0000 C CNN
	1    9100 5200
	0    -1   -1   0   
$EndComp
$Comp
L R R15
U 1 1 54965A0B
P 9100 3700
F 0 "R15" V 9180 3700 40  0000 C CNN
F 1 "470" V 9107 3701 40  0000 C CNN
F 2 "~" V 9030 3700 30  0000 C CNN
F 3 "~" H 9100 3700 30  0000 C CNN
	1    9100 3700
	1    0    0    -1  
$EndComp
Text GLabel 9100 3400 1    60   Input ~ 0
GND
$Comp
L R R14
U 1 1 54965E3E
P 5650 4700
F 0 "R14" V 5730 4700 40  0000 C CNN
F 1 "10 k" V 5657 4701 40  0000 C CNN
F 2 "~" V 5580 4700 30  0000 C CNN
F 3 "~" H 5650 4700 30  0000 C CNN
	1    5650 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 5200 6000 5200
$Comp
L R R13
U 1 1 549665E7
P 4750 5200
F 0 "R13" V 4830 5200 40  0000 C CNN
F 1 "10 k" V 4757 5201 40  0000 C CNN
F 2 "~" V 4680 5200 30  0000 C CNN
F 3 "~" H 4750 5200 30  0000 C CNN
	1    4750 5200
	0    -1   -1   0   
$EndComp
$Comp
L C C?
U 1 1 54966746
P 4300 4800
F 0 "C?" H 4300 4900 40  0000 L CNN
F 1 "47 µF / 16 V" H 4306 4715 40  0000 L CNN
F 2 "~" H 4338 4650 30  0000 C CNN
F 3 "~" H 4300 4800 60  0000 C CNN
	1    4300 4800
	1    0    0    -1  
$EndComp
Text GLabel 4300 4400 1    60   Input ~ 0
GND
Wire Wire Line
	4300 4400 4300 4650
Wire Wire Line
	4000 5200 4600 5200
Wire Wire Line
	4300 4950 4300 5200
Connection ~ 4300 5200
Wire Wire Line
	6600 5100 6950 5100
Wire Wire Line
	6950 5100 6950 4850
Wire Wire Line
	6950 4850 7400 4850
Wire Wire Line
	7150 4850 7150 5050
Connection ~ 7150 4850
Wire Wire Line
	6950 5300 6950 5550
Wire Wire Line
	6950 5550 7400 5550
Wire Wire Line
	7150 5350 7150 5550
Connection ~ 7150 5550
Wire Wire Line
	7950 4850 7950 5050
Wire Wire Line
	7950 5350 7950 5550
Connection ~ 7950 4850
Connection ~ 7950 5550
Wire Wire Line
	10050 5550 8500 5550
Wire Wire Line
	7700 4850 8200 4850
Wire Wire Line
	7700 5550 8200 5550
Wire Wire Line
	10050 1700 10050 5550
Text Notes 800  1550 0    60   ~ 0
Sensor\nR = 53 Ohms @ 20 °C
Text Notes 3300 5700 0    39   ~ 0
Fix:\nThe input to the ADC needs to be of low impedance\nto help charging the ADC capacitor quickly.\nSo R12 was removed (bridged).
Wire Wire Line
	3550 5200 3550 5400
Wire Wire Line
	4150 5200 4150 5400
Connection ~ 4150 5200
Connection ~ 3550 5200
Wire Wire Line
	4150 5400 3550 5400
Text Notes 3700 5300 0    157  ~ 0
XX
Wire Wire Line
	5650 4550 5650 4200
Wire Wire Line
	9100 3400 9100 3550
Wire Wire Line
	8500 4850 9100 4850
Wire Wire Line
	8750 5350 8750 5550
Connection ~ 8750 5550
Wire Wire Line
	9100 5350 9100 5550
Connection ~ 9100 5550
Wire Wire Line
	9100 3850 9100 5050
Wire Wire Line
	10050 1700 6950 1700
Wire Wire Line
	6950 1700 6950 1350
Wire Wire Line
	5800 1850 5800 2100
Wire Wire Line
	5800 2100 5900 2100
Connection ~ 9100 4850
Wire Wire Line
	8750 5050 8750 4850
Connection ~ 8750 4850
Connection ~ 9100 4050
Wire Wire Line
	6400 4800 6400 4900
Wire Wire Line
	6400 5500 6400 5600
Wire Wire Line
	5650 4850 5650 5200
Connection ~ 5650 5200
Wire Wire Line
	5800 4050 9100 4050
Text Notes 5750 4450 0    39   ~ 0
R14 is 20k in newer models
Text Notes 8750 5750 0    39   ~ 0
D5 and D6 connection is not correctly performed in\nolder through-hole based (non-SMD) models.
$EndSCHEMATC
