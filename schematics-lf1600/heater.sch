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
Sheet 4 5
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
L MOC3041M U5
U 1 1 5496ADF9
P 4700 2450
F 0 "U5" H 4490 2640 40  0000 C CNN
F 1 "MOC3041" H 4790 2260 40  0000 C CNN
F 2 "DIP6" H 4500 2270 30  0000 C CIN
F 3 "~" H 4700 2450 60  0000 C CNN
	1    4700 2450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 J2
U 1 1 5496B0E2
P 2450 3950
F 0 "J2" H 2450 4100 40  0000 C CNN
F 1 "V_OUT" V 2550 3950 40  0000 C CNN
F 2 "" H 2450 3950 60  0000 C CNN
F 3 "" H 2450 3950 60  0000 C CNN
	1    2450 3950
	-1   0    0    1   
$EndComp
$Comp
L R R24
U 1 1 5496B0E8
P 5800 4050
F 0 "R24" V 5880 4050 40  0000 C CNN
F 1 "0.1" V 5807 4051 40  0000 C CNN
F 2 "~" V 5730 4050 30  0000 C CNN
F 3 "~" H 5800 4050 30  0000 C CNN
	1    5800 4050
	0    -1   -1   0   
$EndComp
Text GLabel 8100 4050 2    60   Input ~ 0
GND
$Comp
L D D7
U 1 1 5496B0F1
P 5300 4400
F 0 "D7" H 5300 4500 40  0000 C CNN
F 1 "DIODE" H 5300 4300 40  0000 C CNN
F 2 "~" H 5300 4400 60  0000 C CNN
F 3 "~" H 5300 4400 60  0000 C CNN
	1    5300 4400
	0    -1   -1   0   
$EndComp
$Comp
L C C13
U 1 1 5496B0F9
P 5300 5000
F 0 "C13" H 5300 5100 40  0000 L CNN
F 1 "1 µF / 50 V / NP" H 5306 4915 40  0000 L CNN
F 2 "~" H 5338 4850 30  0000 C CNN
F 3 "~" H 5300 5000 60  0000 C CNN
	1    5300 5000
	1    0    0    -1  
$EndComp
Text GLabel 4200 2350 0    60   Input ~ 0
+5V
$Comp
L R R21
U 1 1 5496FF69
P 5200 1950
F 0 "R21" V 5280 1950 40  0000 C CNN
F 1 "51" V 5207 1951 40  0000 C CNN
F 2 "~" V 5130 1950 30  0000 C CNN
F 3 "~" H 5200 1950 30  0000 C CNN
	1    5200 1950
	1    0    0    -1  
$EndComp
Text GLabel 5200 1500 1    60   Input ~ 0
24V
Text GLabel 6250 1500 1    60   Input ~ 0
24V
$Comp
L C C6
U 1 1 54970334
P 7000 2950
F 0 "C6" H 7000 3050 40  0000 L CNN
F 1 "0.1 µF / 100 V" H 7006 2865 40  0000 L CNN
F 2 "~" H 7038 2800 30  0000 C CNN
F 3 "~" H 7000 2950 60  0000 C CNN
	1    7000 2950
	-1   0    0    1   
$EndComp
$Comp
L R R22
U 1 1 5497035D
P 7000 2000
F 0 "R22" V 7080 2000 40  0000 C CNN
F 1 "100" V 7007 2001 40  0000 C CNN
F 2 "~" V 6930 2000 30  0000 C CNN
F 3 "~" H 7000 2000 30  0000 C CNN
	1    7000 2000
	-1   0    0    1   
$EndComp
Text GLabel 7000 1500 1    60   Input ~ 0
24V
$Comp
L R R20
U 1 1 549491CE
P 3700 2550
F 0 "R20" V 3780 2550 40  0000 C CNN
F 1 "390" V 3707 2551 40  0000 C CNN
F 2 "~" V 3630 2550 30  0000 C CNN
F 3 "~" H 3700 2550 30  0000 C CNN
	1    3700 2550
	0    1    1    0   
$EndComp
Text HLabel 2600 2550 0    60   Input ~ 0
heater_on
$Comp
L R R25
U 1 1 5494E6B0
P 4950 5050
F 0 "R25" V 5030 5050 40  0000 C CNN
F 1 "10 M" V 4957 5051 40  0000 C CNN
F 2 "~" V 4880 5050 30  0000 C CNN
F 3 "~" H 4950 5050 30  0000 C CNN
	1    4950 5050
	1    0    0    -1  
$EndComp
$Comp
L R R26
U 1 1 5494E6BF
P 6150 5050
F 0 "R26" V 6230 5050 40  0000 C CNN
F 1 "10 k" V 6157 5051 40  0000 C CNN
F 2 "~" V 6080 5050 30  0000 C CNN
F 3 "~" H 6150 5050 30  0000 C CNN
	1    6150 5050
	1    0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 5494E6F2
P 6500 5700
F 0 "C14" H 6500 5800 40  0000 L CNN
F 1 "100 nF" H 6506 5615 40  0000 L CNN
F 2 "~" H 6538 5550 30  0000 C CNN
F 3 "~" H 6500 5700 60  0000 C CNN
	1    6500 5700
	1    0    0    -1  
$EndComp
Text GLabel 6500 6000 3    60   Input ~ 0
GND
Wire Wire Line
	5950 4050 8100 4050
Wire Wire Line
	2750 4050 5650 4050
Wire Wire Line
	5300 4050 5300 4250
Connection ~ 5300 4050
Wire Wire Line
	4200 2350 4400 2350
Wire Wire Line
	5000 2550 6100 2550
Wire Wire Line
	5200 2100 5200 2350
Wire Wire Line
	5200 2350 5000 2350
Wire Wire Line
	5200 1500 5200 1800
Wire Wire Line
	6250 1500 6250 2300
Wire Wire Line
	2750 3850 7000 3850
Wire Wire Line
	6250 3850 6250 2600
Connection ~ 6250 3850
Wire Wire Line
	7000 2150 7000 2800
Wire Wire Line
	7000 1500 7000 1850
Wire Wire Line
	3850 2550 4400 2550
Wire Wire Line
	2600 2550 3550 2550
Wire Wire Line
	6500 5850 6500 6000
Wire Wire Line
	6150 5200 6150 5500
Wire Wire Line
	6150 4700 6150 4900
$Comp
L R R27
U 1 1 5494E9CA
P 6150 5650
F 0 "R27" V 6230 5650 40  0000 C CNN
F 1 "100 k" V 6157 5651 40  0000 C CNN
F 2 "~" V 6080 5650 30  0000 C CNN
F 3 "~" H 6150 5650 30  0000 C CNN
	1    6150 5650
	1    0    0    -1  
$EndComp
$Comp
L R R28
U 1 1 5494E9D9
P 7050 5350
F 0 "R28" V 7130 5350 40  0000 C CNN
F 1 "10 k" V 7057 5351 40  0000 C CNN
F 2 "~" V 6980 5350 30  0000 C CNN
F 3 "~" H 7050 5350 30  0000 C CNN
	1    7050 5350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6150 5350 6900 5350
Connection ~ 6150 5350
Text GLabel 6150 6000 3    60   Input ~ 0
GND
Wire Wire Line
	6150 5800 6150 6000
Text GLabel 4950 5400 3    60   Input ~ 0
GND
Wire Wire Line
	4950 5200 4950 5400
Text GLabel 5300 5400 3    60   Input ~ 0
GND
Wire Wire Line
	5300 5150 5300 5400
Wire Wire Line
	5300 4550 5300 4850
Wire Wire Line
	4950 4700 6150 4700
Connection ~ 5300 4700
Wire Wire Line
	6500 5350 6500 5550
Connection ~ 6500 5350
Text Notes 5700 4200 0    60   ~ 0
shunt
Text HLabel 8450 5350 2    60   Input ~ 0
current_measure
Wire Wire Line
	7200 5350 8450 5350
Wire Wire Line
	4950 4700 4950 4900
Text Notes 1050 4050 0    60   ~ 0
Heater element\nR = 3.5 Ohms @ 20 °C
Wire Wire Line
	7000 3850 7000 3100
Text Notes 6800 5100 0    39   ~ 0
Fix:\nThe input to the ADC needs to be of low impedance\nto help charging the ADC capacitor quickly.\nSo R28 was removed (bridged).
Wire Wire Line
	6750 5350 6750 5150
Wire Wire Line
	6750 5150 7350 5150
Wire Wire Line
	7350 5150 7350 5350
Connection ~ 7350 5350
Connection ~ 6750 5350
Wire Wire Line
	2650 3900 2750 3900
Wire Wire Line
	2750 3900 2750 3850
Wire Wire Line
	2650 4000 2750 4000
Wire Wire Line
	2750 4000 2750 4050
Text Notes 6900 5450 0    157  ~ 0
XX
$Comp
L Q_TRIAC_AAG D?
U 1 1 58C703A8
P 6250 2450
F 0 "D?" H 6375 2475 50  0000 L CNN
F 1 "BTA12-600C" H 6375 2400 50  0000 L CNN
F 2 "" V 6325 2475 50  0000 C CNN
F 3 "" V 6250 2450 50  0000 C CNN
	1    6250 2450
	1    0    0    -1  
$EndComp
$EndSCHEMATC
