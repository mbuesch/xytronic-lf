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
Sheet 1 5
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
L ATMEGA88PA-P U1
U 1 1 5493288E
P 4450 4550
F 0 "U1" H 3700 5800 40  0000 L BNN
F 1 "ATMEGA88PA-PU" H 4850 3150 40  0000 L BNN
F 2 "DIL28" H 4450 4550 30  0000 C CIN
F 3 "" H 4450 4550 60  0000 C CNN
	1    4450 4550
	1    0    0    -1  
$EndComp
$Sheet
S 1700 950  950  200 
U 549333F6
F0 "supply" 50
F1 "supply.sch" 50
$EndSheet
Wire Wire Line
	5450 3650 8500 3650
Wire Wire Line
	5450 3750 8500 3750
Wire Wire Line
	5450 3850 8500 3850
NoConn ~ 5450 3950
$Comp
L R R8
U 1 1 54935809
P 6600 4900
F 0 "R8" V 6680 4900 40  0000 C CNN
F 1 "10 k" V 6607 4901 40  0000 C CNN
F 2 "~" V 6530 4900 30  0000 C CNN
F 3 "~" H 6600 4900 30  0000 C CNN
	1    6600 4900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5450 4900 6450 4900
Text GLabel 7050 4900 2    60   Input ~ 0
+5V
$Comp
L C C1
U 1 1 54935947
P 6950 4550
F 0 "C1" H 6950 4650 40  0000 L CNN
F 1 "100 nF" H 6956 4465 40  0000 L CNN
F 2 "~" H 6988 4400 30  0000 C CNN
F 3 "~" H 6950 4550 60  0000 C CNN
	1    6950 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 4900 7050 4900
Text GLabel 7050 4200 2    60   Input ~ 0
GND
Text GLabel 3350 5750 0    60   Input ~ 0
GND
Wire Wire Line
	3450 5650 3450 6200
Wire Wire Line
	3450 5650 3550 5650
Wire Wire Line
	3350 5750 3550 5750
$Comp
L R R12
U 1 1 54935CDD
P 2350 3750
F 0 "R12" V 2430 3750 40  0000 C CNN
F 1 "33" V 2357 3751 40  0000 C CNN
F 2 "~" V 2280 3750 30  0000 C CNN
F 3 "~" H 2350 3750 30  0000 C CNN
	1    2350 3750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2500 3750 3550 3750
$Comp
L R R11
U 1 1 54935F81
P 3150 4650
F 0 "R11" V 3230 4650 40  0000 C CNN
F 1 "100" V 3157 4651 40  0000 C CNN
F 2 "~" V 3080 4650 30  0000 C CNN
F 3 "~" H 3150 4650 30  0000 C CNN
	1    3150 4650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 4650 3450 4650
Wire Wire Line
	3450 4850 3450 4050
Wire Wire Line
	3450 4050 3550 4050
Wire Wire Line
	2600 4650 3000 4650
Text GLabel 2100 4650 0    60   Input ~ 0
GND
Text GLabel 2450 6050 3    60   Input ~ 0
GND
Wire Wire Line
	2450 5950 2450 6050
Connection ~ 3450 4650
Wire Wire Line
	2000 4850 3450 4850
Text Label 3450 4450 1    60   ~ 0
2.5 V
Wire Wire Line
	6950 4700 6950 4900
Connection ~ 6950 4900
Wire Wire Line
	5450 5050 8500 5050
Wire Wire Line
	5450 5150 8500 5150
Wire Wire Line
	5450 5250 8500 5250
Wire Wire Line
	5450 5450 8500 5450
Wire Wire Line
	5450 5550 8500 5550
Wire Wire Line
	5450 5650 8500 5650
Wire Wire Line
	5450 5750 8500 5750
Wire Wire Line
	5450 5350 8500 5350
NoConn ~ 5450 4050
Wire Wire Line
	5450 4150 6250 4150
Wire Wire Line
	6250 4150 6250 5900
Connection ~ 2750 4650
Wire Wire Line
	2750 4650 2750 3750
Text GLabel 1900 3450 0    60   Input ~ 0
+5V
Wire Wire Line
	1900 3450 3550 3450
Connection ~ 2750 3750
Wire Wire Line
	2000 3450 2000 3750
Connection ~ 2000 3450
Wire Wire Line
	2000 3750 2200 3750
Wire Wire Line
	7550 3550 5450 3550
Wire Wire Line
	7550 2100 7550 3550
Wire Wire Line
	5450 4600 5950 4600
Wire Wire Line
	5950 4600 5950 6000
Wire Wire Line
	5450 4700 5850 4700
Wire Wire Line
	5850 4700 5850 6100
Wire Wire Line
	5450 4800 5750 4800
Wire Wire Line
	5750 4800 5750 6200
Text Label 7750 6000 0    39   ~ 0
anode, big digit, left
Text Label 7750 6100 0    39   ~ 0
anode, big digit, center
Text Label 7750 6200 0    39   ~ 0
anode, big digit, right
Text Label 7750 5900 0    39   ~ 0
anode, small digit
Text Label 7900 5050 0    39   ~ 0
big: b; small: f
Text Label 7900 5150 0    39   ~ 0
big: g; small: a
Text Label 7900 5250 0    39   ~ 0
big: c; small: d
Text Label 7900 5350 0    39   ~ 0
big: dp; small: e
Text Label 7900 5450 0    39   ~ 0
big: d; small: c
Text Label 7900 5550 0    39   ~ 0
big: e; small: dp
Text Label 7900 5650 0    39   ~ 0
big: f; small: b
Text Label 7900 5750 0    39   ~ 0
big: a; small: g
$Sheet
S 8500 3400 950  3000
U 549454DE
F0 "user_interface" 50
F1 "user_interface.sch" 50
F2 "anode_big_left" I L 8500 6000 60 
F3 "anode_big_center" I L 8500 6100 60 
F4 "anode_big_right" I L 8500 6200 60 
F5 "anode_small" I L 8500 5900 60 
F6 "dp/e" I L 8500 5350 60 
F7 "a/f" I L 8500 5050 60 
F8 "g/a" I L 8500 5150 60 
F9 "c/d" I L 8500 5250 60 
F10 "d/c" I L 8500 5450 60 
F11 "e/dp" I L 8500 5550 60 
F12 "f/b" I L 8500 5650 60 
F13 "a/g" I L 8500 5750 60 
F14 "sw_set" I L 8500 3650 60 
F15 "sw_minus" I L 8500 3750 60 
F16 "sw_plus" I L 8500 3850 60 
$EndSheet
Wire Wire Line
	6250 5900 8500 5900
Wire Wire Line
	5950 6000 8500 6000
Wire Wire Line
	5850 6100 8500 6100
Wire Wire Line
	5750 6200 8500 6200
Wire Wire Line
	6950 4200 6950 4400
Wire Wire Line
	6950 4200 7050 4200
$Sheet
S 8500 950  950  1350
U 5496AC86
F0 "heater" 50
F1 "heater.sch" 50
F2 "heater_on" I L 8500 2100 60 
F3 "current_measure" I L 8500 2000 60 
$EndSheet
Wire Wire Line
	7550 2100 8500 2100
Wire Wire Line
	5450 4500 6050 4500
Wire Wire Line
	6050 4500 6050 2000
Wire Wire Line
	6050 2000 8500 2000
$Sheet
S 4050 950  950  1350
U 5495298F
F0 "temperature" 50
F1 "temperature.sch" 50
F2 "temp_measure" I R 5000 2000 60 
F3 "iron_switch" I R 5000 2100 60 
$EndSheet
Wire Wire Line
	5950 2000 5950 4400
Wire Wire Line
	5950 4400 5450 4400
Wire Wire Line
	5950 2000 5000 2000
Wire Wire Line
	5450 3450 5850 3450
Wire Wire Line
	5850 3450 5850 2100
Wire Wire Line
	5850 2100 5000 2100
Text Notes 5350 6050 2    39   ~ 0
labled as 'HS-K67121'
$Comp
L C C1b
U 1 1 54A1FCC7
P 3450 6350
F 0 "C1b" H 3450 6450 40  0000 L CNN
F 1 "10 nF" H 3456 6265 40  0000 L CNN
F 2 "~" H 3488 6200 30  0000 C CNN
F 3 "~" H 3450 6350 60  0000 C CNN
	1    3450 6350
	1    0    0    -1  
$EndComp
Connection ~ 3450 5750
Text GLabel 3450 6650 3    60   Input ~ 0
+5V
Wire Wire Line
	3450 6500 3450 6650
Text Notes 3700 6600 0    39   ~ 0
Fix:\nC1b was added, because C1 is far away from VCC/GND pins\nin older through-hole models.
$Comp
L D D4
U 1 1 55EC2D4B
P 6150 1400
F 0 "D4" H 6150 1500 40  0000 C CNN
F 1 "DIODE" H 6150 1300 40  0000 C CNN
F 2 "~" H 6150 1400 60  0000 C CNN
F 3 "~" H 6150 1400 60  0000 C CNN
	1    6150 1400
	0    1    1    0   
$EndComp
$Comp
L R R9
U 1 1 55EC2E11
P 6450 1350
F 0 "R9" V 6530 1350 40  0000 C CNN
F 1 "R" V 6457 1351 40  0000 C CNN
F 2 "~" V 6380 1350 30  0000 C CNN
F 3 "~" H 6450 1350 30  0000 C CNN
	1    6450 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1500 6450 1750
Wire Wire Line
	6450 1750 6150 1750
Connection ~ 6150 1750
Text GLabel 6150 1050 1    60   Input ~ 0
GND
Wire Wire Line
	6150 1050 6150 1250
Text GLabel 6450 1050 1    60   Input ~ 0
+5V
Wire Wire Line
	6450 1050 6450 1200
Wire Wire Line
	6150 4300 5450 4300
Wire Notes Line
	5950 1650 6600 1100
Text Notes 6650 1450 0    39   ~ 0
D4 and R9\nare not assembled\non the board.
Wire Notes Line
	5950 1100 6600 1650
Wire Wire Line
	2100 4650 2300 4650
$Comp
L CP C7
U 1 1 57948B5F
P 2450 4650
F 0 "C7" H 2350 4750 40  0000 R CNN
F 1 "47 µF / 16 V" H 2350 4550 40  0000 R CNN
F 2 "" H 2488 4500 50  0000 C CNN
F 3 "" H 2450 4650 50  0000 C CNN
	1    2450 4650
	0    1    1    0   
$EndComp
Wire Wire Line
	6150 1550 6150 4300
$Comp
L TL431 U3
U 1 1 57954078
P 2450 5600
F 0 "U3" H 2450 5400 39  0000 C CNN
F 1 "TL431" H 2300 5800 39  0000 C CNN
F 2 "" H 2450 5600 39  0000 C CNN
F 3 "" H 2450 5600 39  0000 C CNN
	1    2450 5600
	0    1    1    0   
$EndComp
Wire Wire Line
	2450 4850 2450 4950
Wire Wire Line
	2000 4850 2000 5450
Connection ~ 2450 4850
Text Notes 5350 6150 2    39   ~ 0
can be replaced by ATMEGA328P
$EndSCHEMATC
