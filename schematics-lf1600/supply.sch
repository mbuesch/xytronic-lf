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
LIBS:special
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
LIBS:lf1600-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title "Xytronic LF-1600 - Power supply"
Date "5 sep 2015"
Rev "1.0"
Comp "Reverse engineered by Michael Büsch <m@bues.ch>"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_5 J?
U 1 1 5493347D
P 4500 1950
F 0 "J?" V 4450 1950 50  0000 C CNN
F 1 "V_IN" V 4550 1950 50  0000 C CNN
F 2 "" H 4500 1950 60  0000 C CNN
F 3 "" H 4500 1950 60  0000 C CNN
	1    4500 1950
	-1   0    0    1   
$EndComp
$Comp
L 7805 U4
U 1 1 54933483
P 6100 4150
F 0 "U4" H 6250 3954 60  0000 C CNN
F 1 "L7805CV" H 6100 4350 60  0000 C CNN
F 2 "" H 6100 4150 60  0000 C CNN
F 3 "" H 6100 4150 60  0000 C CNN
	1    6100 4150
	1    0    0    -1  
$EndComp
Text GLabel 5950 2150 2    60   Input ~ 0
24Veff
Text GLabel 6300 2050 2    60   Input ~ 0
19Veff
Text GLabel 5950 1950 2    60   Input ~ 0
GND
Text GLabel 5350 1850 2    60   Input ~ 0
GND
Text GLabel 5050 1750 2    60   Input ~ 0
8Veff
Text Notes 5050 1650 0    60   ~ 0
Control voltage
Text Notes 5950 2350 0    60   ~ 0
Supply voltage\n19Veff is not connected\n24Veff is labled as "24V", but actually is 31 V (eff)
$Comp
L DIODE D2
U 1 1 54933622
P 3750 4800
F 0 "D2" H 3750 4900 40  0000 C CNN
F 1 "1N4007" H 3750 4700 40  0000 C CNN
F 2 "~" H 3750 4800 60  0000 C CNN
F 3 "~" H 3750 4800 60  0000 C CNN
	1    3750 4800
	1    0    0    -1  
$EndComp
$Comp
L DIODE D1
U 1 1 54933631
P 3750 5200
F 0 "D1" H 3750 5300 40  0000 C CNN
F 1 "1N4007" H 3750 5100 40  0000 C CNN
F 2 "~" H 3750 5200 60  0000 C CNN
F 3 "~" H 3750 5200 60  0000 C CNN
	1    3750 5200
	-1   0    0    1   
$EndComp
Text GLabel 2950 5000 0    60   Input ~ 0
8Veff
$Comp
L C C9
U 1 1 54933778
P 4150 5600
F 0 "C9" H 4150 5700 40  0000 L CNN
F 1 "470 µF / 25 V" H 4156 5515 40  0000 L CNN
F 2 "~" H 4188 5450 30  0000 C CNN
F 3 "~" H 4150 5600 60  0000 C CNN
	1    4150 5600
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 54933971
P 4600 5200
F 0 "R23" V 4680 5200 40  0000 C CNN
F 1 "330" V 4607 5201 40  0000 C CNN
F 2 "~" V 4530 5200 30  0000 C CNN
F 3 "~" H 4600 5200 30  0000 C CNN
	1    4600 5200
	0    -1   -1   0   
$EndComp
Text Label 4150 5800 1    60   ~ 0
+
Text GLabel 4600 6100 3    60   Input ~ 0
GND
Text GLabel 8250 5200 2    60   Input ~ 0
-5V
$Comp
L ZENER ZD1
U 1 1 54933C97
P 5050 5600
F 0 "ZD1" H 5050 5700 50  0000 C CNN
F 1 "5 V" H 5050 5500 40  0000 C CNN
F 2 "~" H 5050 5600 60  0000 C CNN
F 3 "~" H 5050 5600 60  0000 C CNN
	1    5050 5600
	0    1    1    0   
$EndComp
$Comp
L C C10
U 1 1 54933EFD
P 4150 4400
F 0 "C10" H 4150 4500 40  0000 L CNN
F 1 "100 nF" H 4156 4315 40  0000 L CNN
F 2 "~" H 4188 4250 30  0000 C CNN
F 3 "~" H 4150 4400 60  0000 C CNN
	1    4150 4400
	1    0    0    -1  
$EndComp
Text GLabel 4400 3900 1    60   Input ~ 0
GND
Text GLabel 5950 4800 0    60   Input ~ 0
GND
$Comp
L C C8
U 1 1 549341A1
P 4650 4400
F 0 "C8" H 4650 4500 40  0000 L CNN
F 1 "470 µF / 25 V" H 4656 4315 40  0000 L CNN
F 2 "~" H 4688 4250 30  0000 C CNN
F 3 "~" H 4650 4400 60  0000 C CNN
	1    4650 4400
	1    0    0    -1  
$EndComp
$Comp
L DIODE D3
U 1 1 54934390
P 6100 3650
F 0 "D3" H 6100 3750 40  0000 C CNN
F 1 "1N4007" H 6100 3550 40  0000 C CNN
F 2 "~" H 6100 3650 60  0000 C CNN
F 3 "~" H 6100 3650 60  0000 C CNN
	1    6100 3650
	-1   0    0    1   
$EndComp
$Comp
L C C11
U 1 1 549344FC
P 6800 4400
F 0 "C11" H 6800 4500 40  0000 L CNN
F 1 "100 nF" H 6806 4315 40  0000 L CNN
F 2 "~" H 6838 4250 30  0000 C CNN
F 3 "~" H 6800 4400 60  0000 C CNN
	1    6800 4400
	1    0    0    -1  
$EndComp
Text GLabel 8250 4100 2    60   Input ~ 0
+5V
$Comp
L C C12
U 1 1 54934D42
P 7300 4400
F 0 "C12" H 7300 4500 40  0000 L CNN
F 1 "470 µF / 25 V" H 7306 4315 40  0000 L CNN
F 2 "~" H 7338 4250 30  0000 C CNN
F 3 "~" H 7300 4400 60  0000 C CNN
	1    7300 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 1750 5050 1750
Wire Wire Line
	4900 1850 5350 1850
Wire Wire Line
	4900 1950 5950 1950
Wire Wire Line
	4900 2050 6300 2050
Wire Wire Line
	4900 2150 5950 2150
Wire Wire Line
	5000 1850 5000 1950
Connection ~ 5000 1950
Connection ~ 5000 1850
Wire Wire Line
	2950 5000 3450 5000
Wire Wire Line
	3450 4800 3450 5200
Wire Wire Line
	3450 4800 3550 4800
Wire Wire Line
	3450 5200 3550 5200
Connection ~ 3450 5000
Wire Wire Line
	3950 5200 4350 5200
Wire Wire Line
	4150 5200 4150 5400
Connection ~ 4150 5200
Wire Wire Line
	4150 5800 4150 5950
Wire Wire Line
	4850 5200 8250 5200
Wire Wire Line
	5050 5200 5050 5400
Connection ~ 5050 5200
Wire Wire Line
	4150 5950 5050 5950
Wire Wire Line
	4600 5950 4600 6100
Wire Wire Line
	5050 5950 5050 5800
Connection ~ 4600 5950
Wire Wire Line
	3950 4800 5450 4800
Wire Wire Line
	4150 4800 4150 4600
Wire Wire Line
	5450 4800 5450 4100
Wire Wire Line
	5450 4100 5700 4100
Connection ~ 4150 4800
Wire Wire Line
	6100 4400 6100 4800
Wire Wire Line
	4150 4200 4150 4050
Wire Wire Line
	4150 4050 4650 4050
Wire Wire Line
	4650 4050 4650 4200
Wire Wire Line
	4400 3900 4400 4050
Connection ~ 4400 4050
Wire Wire Line
	4650 4800 4650 4600
Connection ~ 4650 4800
Wire Wire Line
	5900 3650 5650 3650
Wire Wire Line
	5650 3650 5650 4100
Connection ~ 5650 4100
Wire Wire Line
	6300 3650 6550 3650
Wire Wire Line
	6550 3650 6550 4100
Wire Wire Line
	6500 4100 8250 4100
Wire Wire Line
	6800 4100 6800 4200
Connection ~ 6550 4100
Wire Wire Line
	5950 4800 7300 4800
Wire Wire Line
	6800 4800 6800 4600
Connection ~ 6800 4100
Wire Wire Line
	7300 4800 7300 4600
Connection ~ 6800 4800
Wire Wire Line
	7300 4200 7300 4100
Connection ~ 7300 4100
Connection ~ 6100 4800
Text Label 4650 4600 1    60   ~ 0
+
Text Label 7300 4200 1    60   ~ 0
+
Text Label 7300 4600 1    60   ~ 0
-
Text Label 4650 4200 1    60   ~ 0
-
Text Label 4150 5400 1    60   ~ 0
-
Text Notes 7350 4550 0    118  ~ 0
XXXX
Text Notes 7350 4700 0    60   ~ 0
replaced by: 100nF/63V film cap
$EndSCHEMATC
