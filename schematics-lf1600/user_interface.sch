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
Sheet 3 5
Title "Xytronic LF-1600 - User interface"
Date "6 sep 2015"
Rev "1.0"
Comp "Reverse engineered by Michael Büsch <m@bues.ch>"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 1950 1200 0    60   Input ~ 0
anode_big_left
Text HLabel 1950 1400 0    60   Input ~ 0
anode_big_center
Text HLabel 1950 1600 0    60   Input ~ 0
anode_big_right
Text HLabel 1950 1800 0    60   Input ~ 0
anode_small
Text HLabel 1950 2700 0    60   Input ~ 0
dp/e
Text HLabel 1950 2100 0    60   Input ~ 0
b/f
Text HLabel 1950 2300 0    60   Input ~ 0
g/a
Text HLabel 1950 2500 0    60   Input ~ 0
c/d
Text HLabel 1950 2900 0    60   Input ~ 0
d/c
Text HLabel 1950 3100 0    60   Input ~ 0
e/dp
Text HLabel 1950 3300 0    60   Input ~ 0
f/b
Text HLabel 1950 3500 0    60   Input ~ 0
a/g
Text HLabel 1950 6500 0    60   Input ~ 0
sw_set
Text HLabel 1950 6850 0    60   Input ~ 0
sw_minus
Text HLabel 1950 7200 0    60   Input ~ 0
sw_plus
$Comp
L SW_PUSH DS1
U 1 1 54945BF2
P 3450 7200
F 0 "DS1" H 3600 7310 50  0000 C CNN
F 1 "PLUS" H 3450 7120 50  0000 C CNN
F 2 "~" H 3450 7200 60  0000 C CNN
F 3 "~" H 3450 7200 60  0000 C CNN
	1    3450 7200
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH DS2
U 1 1 54945BF8
P 3450 6850
F 0 "DS2" H 3600 6960 50  0000 C CNN
F 1 "MINUS" H 3450 6770 50  0000 C CNN
F 2 "~" H 3450 6850 60  0000 C CNN
F 3 "~" H 3450 6850 60  0000 C CNN
	1    3450 6850
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH DS3
U 1 1 54945BFE
P 3450 6500
F 0 "DS3" H 3600 6610 50  0000 C CNN
F 1 "SET" H 3450 6420 50  0000 C CNN
F 2 "~" H 3450 6500 60  0000 C CNN
F 3 "~" H 3450 6500 60  0000 C CNN
	1    3450 6500
	1    0    0    -1  
$EndComp
Text GLabel 3900 6500 2    60   Input ~ 0
GND
Text GLabel 3900 6850 2    60   Input ~ 0
GND
Text GLabel 3900 7200 2    60   Input ~ 0
GND
Wire Wire Line
	3900 6500 3750 6500
Wire Wire Line
	3900 6850 3750 6850
Wire Wire Line
	3900 7200 3750 7200
Wire Wire Line
	1950 7200 3150 7200
Wire Wire Line
	1950 6850 3150 6850
Wire Wire Line
	1950 6500 3150 6500
$Comp
L R R0
U 1 1 5495563A
P 3450 2700
F 0 "R0" V 3530 2700 40  0000 C CNN
F 1 "120" V 3457 2701 40  0000 C CNN
F 2 "~" V 3380 2700 30  0000 C CNN
F 3 "~" H 3450 2700 30  0000 C CNN
	1    3450 2700
	0    -1   -1   0   
$EndComp
$Comp
L R R1
U 1 1 54955640
P 3450 2100
F 0 "R1" V 3530 2100 40  0000 C CNN
F 1 "120" V 3457 2101 40  0000 C CNN
F 2 "~" V 3380 2100 30  0000 C CNN
F 3 "~" H 3450 2100 30  0000 C CNN
	1    3450 2100
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 54955646
P 3450 2300
F 0 "R2" V 3530 2300 40  0000 C CNN
F 1 "120" V 3457 2301 40  0000 C CNN
F 2 "~" V 3380 2300 30  0000 C CNN
F 3 "~" H 3450 2300 30  0000 C CNN
	1    3450 2300
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 5495564C
P 3450 2500
F 0 "R3" V 3530 2500 40  0000 C CNN
F 1 "120" V 3457 2501 40  0000 C CNN
F 2 "~" V 3380 2500 30  0000 C CNN
F 3 "~" H 3450 2500 30  0000 C CNN
	1    3450 2500
	0    -1   -1   0   
$EndComp
$Comp
L R R4
U 1 1 54955652
P 3450 2900
F 0 "R4" V 3530 2900 40  0000 C CNN
F 1 "120" V 3457 2901 40  0000 C CNN
F 2 "~" V 3380 2900 30  0000 C CNN
F 3 "~" H 3450 2900 30  0000 C CNN
	1    3450 2900
	0    -1   -1   0   
$EndComp
$Comp
L R R5
U 1 1 54955658
P 3450 3100
F 0 "R5" V 3530 3100 40  0000 C CNN
F 1 "120" V 3457 3101 40  0000 C CNN
F 2 "~" V 3380 3100 30  0000 C CNN
F 3 "~" H 3450 3100 30  0000 C CNN
	1    3450 3100
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 5495565E
P 3450 3300
F 0 "R6" V 3530 3300 40  0000 C CNN
F 1 "120" V 3457 3301 40  0000 C CNN
F 2 "~" V 3380 3300 30  0000 C CNN
F 3 "~" H 3450 3300 30  0000 C CNN
	1    3450 3300
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 54955664
P 3450 3500
F 0 "R7" V 3530 3500 40  0000 C CNN
F 1 "120" V 3457 3501 40  0000 C CNN
F 2 "~" V 3380 3500 30  0000 C CNN
F 3 "~" H 3450 3500 30  0000 C CNN
	1    3450 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1950 2100 3300 2100
Wire Wire Line
	1950 2300 3300 2300
Wire Wire Line
	1950 2500 3300 2500
Wire Wire Line
	1950 2700 3300 2700
Wire Wire Line
	1950 2900 3300 2900
Wire Wire Line
	1950 3100 3300 3100
Wire Wire Line
	1950 3300 3300 3300
Wire Wire Line
	1950 3500 3300 3500
$Comp
L 7SEGMENTS LED0a
U 1 1 54963B4D
P 3250 5400
F 0 "LED0a" H 3250 5950 60  0000 C CNN
F 1 "BIG_LEFT" H 3250 4950 60  0000 C CNN
F 2 "~" H 3250 5400 60  0000 C CNN
F 3 "~" H 3250 5400 60  0000 C CNN
	1    3250 5400
	1    0    0    -1  
$EndComp
$Comp
L 7SEGMENTS LED0b
U 1 1 54963B53
P 5150 5400
F 0 "LED0b" H 5150 5950 60  0000 C CNN
F 1 "BIG_CENTER" H 5150 4950 60  0000 C CNN
F 2 "~" H 5150 5400 60  0000 C CNN
F 3 "~" H 5150 5400 60  0000 C CNN
	1    5150 5400
	1    0    0    -1  
$EndComp
$Comp
L 7SEGMENTS LED0c
U 1 1 54963B59
P 7050 5400
F 0 "LED0c" H 7050 5950 60  0000 C CNN
F 1 "BIG_RIGHT" H 7050 4950 60  0000 C CNN
F 2 "~" H 7050 5400 60  0000 C CNN
F 3 "~" H 7050 5400 60  0000 C CNN
	1    7050 5400
	1    0    0    -1  
$EndComp
$Comp
L 7SEGMENTS LED1
U 1 1 54963B78
P 8950 5300
F 0 "LED1" H 8950 5850 60  0000 C CNN
F 1 "SMALL" H 8950 4850 60  0000 C CNN
F 2 "~" H 8950 5300 60  0000 C CNN
F 3 "~" H 8950 5300 60  0000 C CNN
	1    8950 5300
	-1   0    0    1   
$EndComp
Wire Wire Line
	2650 4700 9550 4700
Wire Wire Line
	2650 4700 2650 5000
Wire Wire Line
	4500 2100 4500 5100
Wire Wire Line
	4500 5100 4550 5100
Wire Wire Line
	2600 4600 9600 4600
Wire Wire Line
	2600 4600 2600 5100
Wire Wire Line
	2600 5100 2650 5100
Wire Wire Line
	4450 5200 4550 5200
Wire Wire Line
	4450 2500 4450 5200
Wire Wire Line
	4400 5300 4550 5300
Wire Wire Line
	4400 2900 4400 5300
Wire Wire Line
	4350 5400 4550 5400
Wire Wire Line
	4350 3100 4350 5400
Wire Wire Line
	4300 5500 4550 5500
Wire Wire Line
	4300 3300 4300 5500
Wire Wire Line
	4250 5600 4550 5600
Wire Wire Line
	4250 2300 4250 5600
Wire Wire Line
	2550 4500 9700 4500
Wire Wire Line
	2550 4500 2550 5200
Wire Wire Line
	2550 5200 2650 5200
Wire Wire Line
	2500 4400 9750 4400
Wire Wire Line
	2500 4400 2500 5300
Wire Wire Line
	2500 5300 2650 5300
Wire Wire Line
	2450 4300 8250 4300
Wire Wire Line
	2450 4300 2450 5400
Wire Wire Line
	2450 5400 2650 5400
Wire Wire Line
	2400 4200 9800 4200
Wire Wire Line
	2400 4200 2400 5500
Wire Wire Line
	2400 5500 2650 5500
Wire Wire Line
	2350 4100 9850 4100
Wire Wire Line
	2350 4100 2350 5600
Wire Wire Line
	2350 5600 2650 5600
Wire Wire Line
	6450 4700 6450 5000
Connection ~ 4550 4700
Wire Wire Line
	6400 4600 6400 5100
Wire Wire Line
	6400 5100 6450 5100
Connection ~ 4500 4600
Wire Wire Line
	6350 4500 6350 5200
Wire Wire Line
	6350 5200 6450 5200
Connection ~ 4450 4500
Wire Wire Line
	6300 4400 6300 5300
Wire Wire Line
	6300 5300 6450 5300
Connection ~ 4400 4400
Wire Wire Line
	6250 4300 6250 5400
Wire Wire Line
	6250 5400 6450 5400
Connection ~ 4350 4300
Wire Wire Line
	6200 4200 6200 5500
Wire Wire Line
	6200 5500 6450 5500
Connection ~ 4300 4200
Wire Wire Line
	6150 4100 6150 5600
Wire Wire Line
	6150 5600 6450 5600
Connection ~ 4250 4100
Wire Wire Line
	3950 5050 3850 5050
Wire Wire Line
	3950 3800 3950 5050
Wire Wire Line
	3950 4950 3850 4950
Connection ~ 3950 4950
Wire Wire Line
	5850 5050 5750 5050
Wire Wire Line
	5850 1400 5850 5050
Wire Wire Line
	5850 4950 5750 4950
Connection ~ 5850 4950
Wire Wire Line
	7750 5050 7650 5050
Wire Wire Line
	7750 1600 7750 5050
Wire Wire Line
	7750 4950 7650 4950
Connection ~ 7750 4950
Wire Wire Line
	8150 5650 8350 5650
Wire Wire Line
	8150 1800 8150 5750
Wire Wire Line
	8150 5750 8350 5750
Connection ~ 8150 5650
Wire Wire Line
	3850 5650 4050 5650
Wire Wire Line
	4050 5650 4050 4000
Wire Wire Line
	5750 5650 5950 5650
Wire Wire Line
	5950 5650 5950 4000
Wire Wire Line
	4050 4000 9650 4000
Wire Wire Line
	7650 5650 7850 5650
Wire Wire Line
	7850 5650 7850 4000
Connection ~ 5950 4000
Wire Wire Line
	8350 5050 8250 5050
Wire Wire Line
	3600 2300 4250 2300
Wire Wire Line
	3600 2500 4450 2500
Wire Wire Line
	3600 2700 4200 2700
Wire Wire Line
	4200 2700 4200 4000
Connection ~ 4200 4000
Wire Wire Line
	3600 2900 4400 2900
Wire Wire Line
	3600 3100 4350 3100
Wire Wire Line
	3600 3300 4300 3300
Wire Wire Line
	4550 3500 4550 5000
Wire Wire Line
	3600 3500 4550 3500
Wire Wire Line
	3600 2100 4500 2100
Wire Wire Line
	3950 3800 4900 3800
Wire Wire Line
	4900 3800 4900 1200
Wire Wire Line
	4900 1200 1950 1200
Wire Wire Line
	1950 1400 5850 1400
Wire Wire Line
	1950 1600 7750 1600
Wire Wire Line
	8150 1800 1950 1800
Wire Wire Line
	9550 4700 9550 5100
Connection ~ 6450 4700
Wire Wire Line
	9550 5200 9600 5200
Wire Wire Line
	9600 5200 9600 4600
Connection ~ 6400 4600
Wire Wire Line
	9550 5300 9650 5300
Wire Wire Line
	9650 5300 9650 4000
Connection ~ 7850 4000
Wire Wire Line
	9550 5400 9700 5400
Wire Wire Line
	9700 5400 9700 4500
Connection ~ 6350 4500
Wire Wire Line
	9550 5500 9750 5500
Wire Wire Line
	9750 5500 9750 4400
Connection ~ 6300 4400
Wire Wire Line
	9550 5600 9800 5600
Wire Wire Line
	9800 5600 9800 4200
Connection ~ 6200 4200
Wire Wire Line
	9550 5700 9850 5700
Wire Wire Line
	9850 5700 9850 4100
Connection ~ 6150 4100
Wire Wire Line
	8250 5050 8250 4300
Connection ~ 6250 4300
Text Notes 8250 6000 0    60   ~ 0
(this 7seg LED is upside down)
Text Notes 3050 3700 0    39   ~ 0
R0-7: is 180R in newer models
Text Notes 6100 2050 0    39   ~ 0
anodes switched via MOSFETs in newer models
$EndSCHEMATC
