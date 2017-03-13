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
LIBS:switches
LIBS:lf1600-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
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
L CONN_01X05 J1
U 1 1 5493347D
P 4500 1950
F 0 "J1" H 4500 2250 50  0000 C CNN
F 1 "V_IN" V 4600 1950 50  0000 C CNN
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
Text Notes 6000 2600 0    60   ~ 0
Supply voltage\n19Veff is not connected\n24Veff is labled as "24V", but actually is 31 V (eff)
$Comp
L D D2
U 1 1 54933622
P 2650 4800
F 0 "D2" H 2650 4900 40  0000 C CNN
F 1 "1N4007" H 2650 4700 40  0000 C CNN
F 2 "~" H 2650 4800 60  0000 C CNN
F 3 "~" H 2650 4800 60  0000 C CNN
	1    2650 4800
	-1   0    0    1   
$EndComp
$Comp
L D D1
U 1 1 54933631
P 2650 5200
F 0 "D1" H 2650 5300 40  0000 C CNN
F 1 "1N4007" H 2650 5100 40  0000 C CNN
F 2 "~" H 2650 5200 60  0000 C CNN
F 3 "~" H 2650 5200 60  0000 C CNN
	1    2650 5200
	1    0    0    -1  
$EndComp
Text GLabel 1850 5000 0    60   Input ~ 0
8Veff
$Comp
L R R23
U 1 1 54933971
P 3500 5200
F 0 "R23" V 3580 5200 40  0000 C CNN
F 1 "330" V 3507 5201 40  0000 C CNN
F 2 "~" V 3430 5200 30  0000 C CNN
F 3 "~" H 3500 5200 30  0000 C CNN
	1    3500 5200
	0    -1   -1   0   
$EndComp
Text GLabel 3500 6100 3    60   Input ~ 0
GND
Text GLabel 8250 5200 2    60   Input ~ 0
-5V
$Comp
L C C10
U 1 1 54933EFD
P 3050 4400
F 0 "C10" H 3050 4500 40  0000 L CNN
F 1 "100 nF" H 3056 4315 40  0000 L CNN
F 2 "~" H 3088 4250 30  0000 C CNN
F 3 "~" H 3050 4400 60  0000 C CNN
	1    3050 4400
	1    0    0    -1  
$EndComp
Text GLabel 3050 3900 1    60   Input ~ 0
GND
Text GLabel 5950 4800 0    60   Input ~ 0
GND
$Comp
L D D3
U 1 1 54934390
P 6100 3650
F 0 "D3" H 6100 3750 40  0000 C CNN
F 1 "1N4007" H 6100 3550 40  0000 C CNN
F 2 "~" H 6100 3650 60  0000 C CNN
F 3 "~" H 6100 3650 60  0000 C CNN
	1    6100 3650
	1    0    0    -1  
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
Wire Wire Line
	4700 1750 5050 1750
Wire Wire Line
	4700 1850 5350 1850
Wire Wire Line
	4700 1950 5950 1950
Wire Wire Line
	4700 2050 6300 2050
Wire Wire Line
	4700 2150 5950 2150
Wire Wire Line
	5000 1850 5000 1950
Connection ~ 5000 1950
Connection ~ 5000 1850
Wire Wire Line
	1850 5000 2350 5000
Wire Wire Line
	2350 4800 2350 5200
Wire Wire Line
	2350 4800 2500 4800
Wire Wire Line
	2350 5200 2500 5200
Connection ~ 2350 5000
Wire Wire Line
	2800 5200 3350 5200
Wire Wire Line
	3050 5200 3050 5450
Connection ~ 3050 5200
Wire Wire Line
	3050 5750 3050 5950
Wire Wire Line
	3650 5200 8250 5200
Wire Wire Line
	3950 5200 3950 5450
Connection ~ 3950 5200
Wire Wire Line
	3950 5950 3050 5950
Wire Wire Line
	3500 5950 3500 6100
Wire Wire Line
	3950 5750 3950 5950
Connection ~ 3500 5950
Wire Wire Line
	2800 4800 5450 4800
Wire Wire Line
	3050 4550 3050 4800
Wire Wire Line
	5450 4800 5450 4100
Wire Wire Line
	5450 4100 5700 4100
Connection ~ 3050 4800
Wire Wire Line
	6100 4400 6100 4800
Wire Wire Line
	3050 3900 3050 4250
Wire Wire Line
	3050 4050 4450 4050
Wire Wire Line
	3550 4050 3550 4250
Wire Wire Line
	3550 4550 3550 4800
Connection ~ 3550 4800
Wire Wire Line
	5650 3650 5950 3650
Wire Wire Line
	5650 3650 5650 4100
Connection ~ 5650 4100
Wire Wire Line
	6250 3650 6550 3650
Wire Wire Line
	6550 3650 6550 4100
Wire Wire Line
	6500 4100 8250 4100
Wire Wire Line
	6800 4100 6800 4250
Connection ~ 6550 4100
Wire Wire Line
	7300 4800 5950 4800
Wire Wire Line
	6800 4550 6800 4800
Connection ~ 6800 4100
Wire Wire Line
	7300 4550 7300 4800
Connection ~ 6800 4800
Wire Wire Line
	7300 4100 7300 4250
Connection ~ 7300 4100
Connection ~ 6100 4800
Text Notes 7400 4600 0    157  ~ 0
XXX
Text Notes 7400 4750 0    39   ~ 0
Fix:\nC12 replaced by: 100nF/63V film cap
Wire Wire Line
	4450 4050 4450 4250
Wire Wire Line
	4450 4550 4450 4800
Connection ~ 4450 4800
Connection ~ 3550 4050
Connection ~ 3050 4050
Text Notes 4150 3950 0    39   ~ 0
Fix:\nC12b was added due to\nremoval of the original C12
$Comp
L CP C8
U 1 1 5793C4A9
P 3550 4400
F 0 "C8" H 3500 4300 40  0000 R CNN
F 1 "470 µF / 25 V" H 3500 4550 40  0000 R CNN
F 2 "" H 3588 4250 50  0000 C CNN
F 3 "" H 3550 4400 50  0000 C CNN
	1    3550 4400
	-1   0    0    1   
$EndComp
$Comp
L CP C12b
U 1 1 5793CA92
P 4450 4400
F 0 "C12b" H 4400 4300 40  0000 R CNN
F 1 "470 µF / 25 V" H 4400 4550 40  0000 R CNN
F 2 "" H 4488 4250 50  0000 C CNN
F 3 "" H 4450 4400 50  0000 C CNN
	1    4450 4400
	-1   0    0    1   
$EndComp
$Comp
L CP C9
U 1 1 5793CE0D
P 3050 5600
F 0 "C9" H 3000 5500 40  0000 R CNN
F 1 "470 µF / 25 V" H 3000 5750 40  0000 R CNN
F 2 "" H 3088 5450 50  0000 C CNN
F 3 "" H 3050 5600 50  0000 C CNN
	1    3050 5600
	-1   0    0    1   
$EndComp
$Comp
L CP C12
U 1 1 5793D6FC
P 7300 4400
F 0 "C12" H 7350 4500 40  0000 L CNN
F 1 "470 µF / 25 V" H 7350 4300 40  0000 L CNN
F 2 "" H 7338 4250 50  0000 C CNN
F 3 "" H 7300 4400 50  0000 C CNN
	1    7300 4400
	1    0    0    -1  
$EndComp
$Comp
L D_Zener_ALT ZD1
U 1 1 58C703D7
P 3950 5600
F 0 "ZD1" H 3950 5700 50  0000 C CNN
F 1 "5V" H 3950 5500 50  0000 C CNN
F 2 "" H 3950 5600 50  0000 C CNN
F 3 "" H 3950 5600 50  0000 C CNN
	1    3950 5600
	0    1    1    0   
$EndComp
$EndSCHEMATC
