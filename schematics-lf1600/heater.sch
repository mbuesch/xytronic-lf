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
Sheet 4 5
Title "Xytronic LF-1600 - Heater power supply"
Date "6 sep 2015"
Rev "1.0"
Comp "Reverse engineered by Michael Büsch <m@bues.ch>"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MOC3041M U5
U 1 1 5496ADF9
P 4950 2750
F 0 "U5" H 4740 2940 40  0000 C CNN
F 1 "MOC3041" H 5040 2560 40  0000 C CNN
F 2 "DIP6" H 4750 2570 30  0000 C CIN
F 3 "~" H 4950 2750 60  0000 C CNN
	1    4950 2750
	1    0    0    -1  
$EndComp
$Comp
L TRIAC U?
U 1 1 5496AE5E
P 6500 2650
F 0 "U?" H 6250 3000 70  0000 C CNN
F 1 "BTA12-600C" H 6200 2400 60  0000 C CNN
F 2 "~" H 6500 2650 60  0000 C CNN
F 3 "~" H 6500 2650 60  0000 C CNN
	1    6500 2650
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 J2
U 1 1 5496B0E2
P 2700 4250
F 0 "J2" V 2650 4250 40  0000 C CNN
F 1 "V_OUT" V 2750 4250 40  0000 C CNN
F 2 "" H 2700 4250 60  0000 C CNN
F 3 "" H 2700 4250 60  0000 C CNN
	1    2700 4250
	-1   0    0    1   
$EndComp
$Comp
L R R24
U 1 1 5496B0E8
P 6050 4350
F 0 "R24" V 6130 4350 40  0000 C CNN
F 1 "0.1" V 6057 4351 40  0000 C CNN
F 2 "~" V 5980 4350 30  0000 C CNN
F 3 "~" H 6050 4350 30  0000 C CNN
	1    6050 4350
	0    -1   -1   0   
$EndComp
Text GLabel 8350 4350 2    60   Input ~ 0
GND
$Comp
L DIODE D7
U 1 1 5496B0F1
P 5550 4700
F 0 "D7" H 5550 4800 40  0000 C CNN
F 1 "DIODE" H 5550 4600 40  0000 C CNN
F 2 "~" H 5550 4700 60  0000 C CNN
F 3 "~" H 5550 4700 60  0000 C CNN
	1    5550 4700
	0    1    1    0   
$EndComp
$Comp
L C C13
U 1 1 5496B0F9
P 5550 5300
F 0 "C13" H 5550 5400 40  0000 L CNN
F 1 "1 µF / 50 V / NP" H 5556 5215 40  0000 L CNN
F 2 "~" H 5588 5150 30  0000 C CNN
F 3 "~" H 5550 5300 60  0000 C CNN
	1    5550 5300
	1    0    0    -1  
$EndComp
Text GLabel 4450 2650 0    60   Input ~ 0
+5V
$Comp
L R R21
U 1 1 5496FF69
P 5450 2250
F 0 "R21" V 5530 2250 40  0000 C CNN
F 1 "51" V 5457 2251 40  0000 C CNN
F 2 "~" V 5380 2250 30  0000 C CNN
F 3 "~" H 5450 2250 30  0000 C CNN
	1    5450 2250
	1    0    0    -1  
$EndComp
Text GLabel 5450 1800 1    60   Input ~ 0
24V
Text GLabel 6500 1800 1    60   Input ~ 0
24V
$Comp
L C C6
U 1 1 54970334
P 7250 3250
F 0 "C6" H 7250 3350 40  0000 L CNN
F 1 "0.1 µF / 100 V" H 7256 3165 40  0000 L CNN
F 2 "~" H 7288 3100 30  0000 C CNN
F 3 "~" H 7250 3250 60  0000 C CNN
	1    7250 3250
	-1   0    0    1   
$EndComp
$Comp
L R R22
U 1 1 5497035D
P 7250 2300
F 0 "R22" V 7330 2300 40  0000 C CNN
F 1 "100" V 7257 2301 40  0000 C CNN
F 2 "~" V 7180 2300 30  0000 C CNN
F 3 "~" H 7250 2300 30  0000 C CNN
	1    7250 2300
	-1   0    0    1   
$EndComp
Text GLabel 7250 1800 1    60   Input ~ 0
24V
$Comp
L R R?
U 1 1 549491CE
P 3950 2850
F 0 "R?" V 4030 2850 40  0000 C CNN
F 1 "390" V 3957 2851 40  0000 C CNN
F 2 "~" V 3880 2850 30  0000 C CNN
F 3 "~" H 3950 2850 30  0000 C CNN
	1    3950 2850
	0    1    1    0   
$EndComp
Text HLabel 2850 2850 0    60   Input ~ 0
heater_on
$Comp
L R R25
U 1 1 5494E6B0
P 5200 5350
F 0 "R25" V 5280 5350 40  0000 C CNN
F 1 "10 k" V 5207 5351 40  0000 C CNN
F 2 "~" V 5130 5350 30  0000 C CNN
F 3 "~" H 5200 5350 30  0000 C CNN
	1    5200 5350
	1    0    0    -1  
$EndComp
$Comp
L R R26
U 1 1 5494E6BF
P 6400 5350
F 0 "R26" V 6480 5350 40  0000 C CNN
F 1 "10 k" V 6407 5351 40  0000 C CNN
F 2 "~" V 6330 5350 30  0000 C CNN
F 3 "~" H 6400 5350 30  0000 C CNN
	1    6400 5350
	1    0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 5494E6F2
P 6750 6000
F 0 "C14" H 6750 6100 40  0000 L CNN
F 1 "100 nF" H 6756 5915 40  0000 L CNN
F 2 "~" H 6788 5850 30  0000 C CNN
F 3 "~" H 6750 6000 60  0000 C CNN
	1    6750 6000
	1    0    0    -1  
$EndComp
Text GLabel 6750 6300 3    60   Input ~ 0
GND
Wire Wire Line
	6300 4350 8350 4350
Wire Wire Line
	3050 4350 5800 4350
Wire Wire Line
	5550 4500 5550 4350
Connection ~ 5550 4350
Wire Wire Line
	4450 2650 4600 2650
Wire Wire Line
	5300 2850 6000 2850
Wire Wire Line
	5450 2500 5450 2650
Wire Wire Line
	5450 2650 5300 2650
Wire Wire Line
	5450 1800 5450 2000
Wire Wire Line
	6500 1800 6500 2250
Wire Wire Line
	3050 4150 7250 4150
Wire Wire Line
	6500 4150 6500 2900
Connection ~ 6500 4150
Wire Wire Line
	7250 2550 7250 3050
Wire Wire Line
	7250 2050 7250 1800
Wire Wire Line
	4200 2850 4600 2850
Wire Wire Line
	2850 2850 3700 2850
Wire Wire Line
	6750 6300 6750 6200
Wire Wire Line
	6400 5600 6400 5700
Wire Wire Line
	6400 5000 6400 5100
$Comp
L R R27
U 1 1 5494E9CA
P 6400 5950
F 0 "R27" V 6480 5950 40  0000 C CNN
F 1 "100 k" V 6407 5951 40  0000 C CNN
F 2 "~" V 6330 5950 30  0000 C CNN
F 3 "~" H 6400 5950 30  0000 C CNN
	1    6400 5950
	1    0    0    -1  
$EndComp
$Comp
L R R28
U 1 1 5494E9D9
P 7300 5650
F 0 "R28" V 7380 5650 40  0000 C CNN
F 1 "10 k" V 7307 5651 40  0000 C CNN
F 2 "~" V 7230 5650 30  0000 C CNN
F 3 "~" H 7300 5650 30  0000 C CNN
	1    7300 5650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6400 5650 7050 5650
Connection ~ 6400 5650
Text GLabel 6400 6300 3    60   Input ~ 0
GND
Wire Wire Line
	6400 6200 6400 6300
Text GLabel 5200 5700 3    60   Input ~ 0
GND
Wire Wire Line
	5200 5600 5200 5700
Text GLabel 5550 5700 3    60   Input ~ 0
GND
Wire Wire Line
	5550 5500 5550 5700
Wire Wire Line
	5550 4900 5550 5100
Wire Wire Line
	5200 5000 6400 5000
Connection ~ 5550 5000
Wire Wire Line
	6750 5650 6750 5800
Connection ~ 6750 5650
Text Notes 5900 4500 0    60   ~ 0
shunt
Text HLabel 8700 5650 2    60   Input ~ 0
current_measure
Wire Wire Line
	7550 5650 8700 5650
Wire Wire Line
	5200 5100 5200 5000
Text Notes 950  4300 0    60   ~ 0
Heater elem. R 3.5 Ohms @ 20 °C
Wire Wire Line
	7250 4150 7250 3450
Text Notes 7100 5150 0    60   ~ 0
Fix:\nThe input to the ADC needs to be of low impedance\nto help charging the ADC capacitor quickly.\nSo R28 was removed (bridged).
Wire Wire Line
	7000 5650 7000 5450
Wire Wire Line
	7000 5450 7600 5450
Wire Wire Line
	7600 5450 7600 5650
Connection ~ 7600 5650
Connection ~ 7000 5650
$EndSCHEMATC
