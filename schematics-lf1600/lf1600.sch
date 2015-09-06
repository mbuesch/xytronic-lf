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
Sheet 1 5
Title "Xytronic LF-1600"
Date "6 sep 2015"
Rev "1.0"
Comp "Reverse engineered by Michael Büsch <m@bues.ch>"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA88PA-P U1
U 1 1 5493288E
P 4750 4750
F 0 "U1" H 4000 6000 40  0000 L BNN
F 1 "ATMEGA88PA-PU" H 5150 3350 40  0000 L BNN
F 2 "DIL28" H 4750 4750 30  0000 C CIN
F 3 "" H 4750 4750 60  0000 C CNN
	1    4750 4750
	1    0    0    -1  
$EndComp
$Sheet
S 2000 1150 950  200 
U 549333F6
F0 "supply" 50
F1 "supply.sch" 50
$EndSheet
Wire Wire Line
	5750 3850 8800 3850
Wire Wire Line
	5750 3950 8800 3950
Wire Wire Line
	5750 4050 8800 4050
NoConn ~ 5750 4150
$Comp
L R R8
U 1 1 54935809
P 6900 5100
F 0 "R8" V 6980 5100 40  0000 C CNN
F 1 "10 k" V 6907 5101 40  0000 C CNN
F 2 "~" V 6830 5100 30  0000 C CNN
F 3 "~" H 6900 5100 30  0000 C CNN
	1    6900 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5750 5100 6650 5100
Text GLabel 7350 5100 2    60   Input ~ 0
+5V
$Comp
L C C1
U 1 1 54935947
P 7250 4750
F 0 "C1" H 7250 4850 40  0000 L CNN
F 1 "100 nF" H 7256 4665 40  0000 L CNN
F 2 "~" H 7288 4600 30  0000 C CNN
F 3 "~" H 7250 4750 60  0000 C CNN
	1    7250 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 5100 7350 5100
Text GLabel 7350 4400 2    60   Input ~ 0
GND
Text GLabel 3650 5950 0    60   Input ~ 0
GND
Wire Wire Line
	3750 5850 3750 6350
Wire Wire Line
	3750 5850 3850 5850
Wire Wire Line
	3650 5950 3850 5950
$Comp
L R R12
U 1 1 54935CDD
P 2650 3950
F 0 "R12" V 2730 3950 40  0000 C CNN
F 1 "33" V 2657 3951 40  0000 C CNN
F 2 "~" V 2580 3950 30  0000 C CNN
F 3 "~" H 2650 3950 30  0000 C CNN
	1    2650 3950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 3950 3850 3950
$Comp
L R R11
U 1 1 54935F81
P 3450 4850
F 0 "R11" V 3530 4850 40  0000 C CNN
F 1 "100" V 3457 4851 40  0000 C CNN
F 2 "~" V 3380 4850 30  0000 C CNN
F 3 "~" H 3450 4850 30  0000 C CNN
	1    3450 4850
	0    -1   -1   0   
$EndComp
$Comp
L C C7
U 1 1 54935FB7
P 2750 4850
F 0 "C7" H 2750 4950 40  0000 L CNN
F 1 "47 µF / 16 V" H 2756 4765 40  0000 L CNN
F 2 "~" H 2788 4700 30  0000 C CNN
F 3 "~" H 2750 4850 60  0000 C CNN
	1    2750 4850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3700 4850 3750 4850
Wire Wire Line
	3750 5050 3750 4250
Wire Wire Line
	3750 4250 3850 4250
Wire Wire Line
	2950 4850 3200 4850
Text GLabel 2400 4850 0    60   Input ~ 0
GND
Wire Wire Line
	2400 4850 2550 4850
Text Label 2950 4850 0    60   ~ 0
+
Text Label 2550 4850 0    60   ~ 0
-
$Comp
L TL431LP U3
U 1 1 54936DE8
P 2800 5550
F 0 "U3" H 2800 5650 70  0000 C CNN
F 1 "TL431" H 2800 5450 70  0000 C CNN
F 2 "" H 2800 5550 60  0000 C CNN
F 3 "" H 2800 5550 60  0000 C CNN
	1    2800 5550
	0    -1   -1   0   
$EndComp
Text GLabel 3400 5550 2    60   Input ~ 0
GND
Wire Wire Line
	3300 5550 3400 5550
Wire Wire Line
	2800 5150 2800 5050
Wire Wire Line
	2200 5050 3750 5050
Connection ~ 3750 4850
Wire Wire Line
	2300 5550 2200 5550
Wire Wire Line
	2200 5550 2200 5050
Connection ~ 2800 5050
Text Label 3750 4650 1    60   ~ 0
2.5 V
Wire Wire Line
	7250 4950 7250 5100
Connection ~ 7250 5100
Wire Wire Line
	5750 5250 8800 5250
Wire Wire Line
	5750 5350 8800 5350
Wire Wire Line
	5750 5450 8800 5450
Wire Wire Line
	5750 5650 8800 5650
Wire Wire Line
	5750 5750 8800 5750
Wire Wire Line
	5750 5850 8800 5850
Wire Wire Line
	5750 5950 8800 5950
Wire Wire Line
	5750 5550 8800 5550
NoConn ~ 5750 4250
Wire Wire Line
	5750 4350 6550 4350
Wire Wire Line
	6550 4350 6550 6100
Connection ~ 3050 4850
Wire Wire Line
	3050 4850 3050 3950
Text GLabel 2200 3650 0    60   Input ~ 0
+5V
Wire Wire Line
	2200 3650 3850 3650
Connection ~ 3050 3950
Wire Wire Line
	2300 3650 2300 3950
Connection ~ 2300 3650
Wire Wire Line
	2300 3950 2400 3950
Wire Wire Line
	7850 3750 5750 3750
Wire Wire Line
	7850 2300 7850 3750
Wire Wire Line
	5750 4800 6250 4800
Wire Wire Line
	6250 4800 6250 6200
Wire Wire Line
	5750 4900 6150 4900
Wire Wire Line
	6150 4900 6150 6300
Wire Wire Line
	5750 5000 6050 5000
Wire Wire Line
	6050 5000 6050 6400
Text Label 8050 6200 0    39   ~ 0
anode, big digit, left
Text Label 8050 6300 0    39   ~ 0
anode, big digit, center
Text Label 8050 6400 0    39   ~ 0
anode, big digit, right
Text Label 8050 6100 0    39   ~ 0
anode, small digit
Text Label 8200 5250 0    39   ~ 0
big: b; small: f
Text Label 8200 5350 0    39   ~ 0
big: g; small: a
Text Label 8200 5450 0    39   ~ 0
big: c; small: d
Text Label 8200 5550 0    39   ~ 0
big: dp; small: e
Text Label 8200 5650 0    39   ~ 0
big: d; small: c
Text Label 8200 5750 0    39   ~ 0
big: e; small: dp
Text Label 8200 5850 0    39   ~ 0
big: f; small: b
Text Label 8200 5950 0    39   ~ 0
big: a; small: g
$Sheet
S 8800 3600 950  3000
U 549454DE
F0 "user_interface" 50
F1 "user_interface.sch" 50
F2 "anode_big_left" I L 8800 6200 60 
F3 "anode_big_center" I L 8800 6300 60 
F4 "anode_big_right" I L 8800 6400 60 
F5 "anode_small" I L 8800 6100 60 
F6 "dp/e" I L 8800 5550 60 
F7 "a/f" I L 8800 5250 60 
F8 "g/a" I L 8800 5350 60 
F9 "c/d" I L 8800 5450 60 
F10 "d/c" I L 8800 5650 60 
F11 "e/dp" I L 8800 5750 60 
F12 "f/b" I L 8800 5850 60 
F13 "a/g" I L 8800 5950 60 
F14 "sw_set" I L 8800 3850 60 
F15 "sw_minus" I L 8800 3950 60 
F16 "sw_plus" I L 8800 4050 60 
$EndSheet
Wire Wire Line
	6550 6100 8800 6100
Wire Wire Line
	6250 6200 8800 6200
Wire Wire Line
	6150 6300 8800 6300
Wire Wire Line
	6050 6400 8800 6400
Wire Wire Line
	7250 4550 7250 4400
Wire Wire Line
	7250 4400 7350 4400
$Sheet
S 8800 1150 950  1350
U 5496AC86
F0 "heater" 50
F1 "heater.sch" 50
F2 "heater_on" I L 8800 2300 60 
F3 "current_measure" I L 8800 2200 60 
$EndSheet
Wire Wire Line
	7850 2300 8800 2300
Wire Wire Line
	5750 4700 6350 4700
Wire Wire Line
	6350 4700 6350 2200
Wire Wire Line
	6350 2200 8800 2200
$Sheet
S 4350 1150 950  1350
U 5495298F
F0 "temperature" 50
F1 "temperature.sch" 50
F2 "temp_measure" I R 5300 2200 60 
F3 "iron_switch" I R 5300 2300 60 
$EndSheet
Wire Wire Line
	6250 2200 6250 4600
Wire Wire Line
	6250 4600 5750 4600
Wire Wire Line
	6250 2200 5300 2200
Wire Wire Line
	5750 3650 6150 3650
Wire Wire Line
	6150 3650 6150 2300
Wire Wire Line
	6150 2300 5300 2300
Text Notes 4550 6250 0    60   ~ 0
marked as 'HS-K67121'
$Comp
L C C1b
U 1 1 54A1FCC7
P 3750 6550
F 0 "C1b" H 3750 6650 40  0000 L CNN
F 1 "10 nF" H 3756 6465 40  0000 L CNN
F 2 "~" H 3788 6400 30  0000 C CNN
F 3 "~" H 3750 6550 60  0000 C CNN
	1    3750 6550
	1    0    0    -1  
$EndComp
Connection ~ 3750 5950
Text GLabel 3750 6850 3    60   Input ~ 0
+5V
Wire Wire Line
	3750 6750 3750 6850
Text Notes 3800 6750 0    60   ~ 0
added, because C1 is far\naway from VCC/GND pins
Wire Wire Line
	6450 1500 6450 4500
$Comp
L DIODE D4
U 1 1 55EC2D4B
P 6450 1300
F 0 "D4" H 6450 1400 40  0000 C CNN
F 1 "DIODE" H 6450 1200 40  0000 C CNN
F 2 "~" H 6450 1300 60  0000 C CNN
F 3 "~" H 6450 1300 60  0000 C CNN
	1    6450 1300
	0    -1   -1   0   
$EndComp
$Comp
L R R9
U 1 1 55EC2E11
P 6750 1250
F 0 "R9" V 6830 1250 40  0000 C CNN
F 1 "R" V 6757 1251 40  0000 C CNN
F 2 "~" V 6680 1250 30  0000 C CNN
F 3 "~" H 6750 1250 30  0000 C CNN
	1    6750 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 1500 6750 1650
Wire Wire Line
	6750 1650 6450 1650
Connection ~ 6450 1650
Text GLabel 6450 950  1    60   Input ~ 0
GND
Wire Wire Line
	6450 950  6450 1100
Text GLabel 6750 950  1    60   Input ~ 0
+5V
Wire Wire Line
	6750 950  6750 1000
Wire Wire Line
	6450 4500 5750 4500
Wire Notes Line
	6250 1550 6900 1000
Text Notes 6950 1200 0    60   ~ 0
D4 and R9\nare not assembled\non the board.
Wire Notes Line
	6250 1000 6900 1550
$EndSCHEMATC
