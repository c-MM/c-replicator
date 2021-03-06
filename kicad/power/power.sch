EESchema Schematic File Version 4
LIBS:power-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "replicator motor control board"
Date "2019-01-30"
Rev "3.0"
Comp "c-base e.V."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R16
U 1 1 578BAD47
P 2750 1300
F 0 "R16" V 2350 1450 60  0000 C CNN
F 1 "VDR 30V" H 2850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 2750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 2750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 2750 1300 60  0001 C CNN "Reichelt"
	1    2750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R6
U 1 1 578BB2BB
P 1700 2550
F 0 "R6" V 1150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 1550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 1700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 1700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 1700 2550 60  0001 C CNN "Reichelt"
	1    1700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:CONN_10 P1
U 1 1 578BC1B3
P 10450 1200
F 0 "P1" V 10400 1200 60  0000 C CNN
F 1 "Motor" V 10500 1200 60  0000 C CNN
F 2 "replicator:Akl_Ria_230_10" H 10450 1200 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C100/AKL230%23RIA.pdf" H 10450 1200 60  0001 C CNN
F 4 "AKL 230-10" V 10450 1200 60  0001 C CNN "Reichelt"
	1    10450 1200
	1    0    0    -1  
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R7
U 1 1 578BC4EF
P 2700 2550
F 0 "R7" V 2150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 2550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 2700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 2700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 2700 2550 60  0001 C CNN "Reichelt"
	1    2700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R8
U 1 1 578BC593
P 3700 2550
F 0 "R8" V 3150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 3550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 3700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 3700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 3700 2550 60  0001 C CNN "Reichelt"
	1    3700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R9
U 1 1 578BC649
P 4700 2550
F 0 "R9" V 4150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 4550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 4700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 4700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 4700 2550 60  0001 C CNN "Reichelt"
	1    4700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R10
U 1 1 578BC6BD
P 5700 2550
F 0 "R10" V 5150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 5550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 5700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 5700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 5700 2550 60  0001 C CNN "Reichelt"
	1    5700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R11
U 1 1 578BC80A
P 6700 2550
F 0 "R11" V 6150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 6550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 6700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 6700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 6700 2550 60  0001 C CNN "Reichelt"
	1    6700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R12
U 1 1 578BC86D
P 7700 2550
F 0 "R12" V 7150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 7550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 7700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 7700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 7700 2550 60  0001 C CNN "Reichelt"
	1    7700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RELAY-1XCHANCE R13
U 1 1 578BC94F
P 8700 2550
F 0 "R13" V 8150 3300 60  0000 C CNN
F 1 "FTR K1CK005W" V 8550 3000 60  0000 C CNN
F 2 "replicator:Relais_1X" H 8700 2550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C300/RELAIS-FTRK1.pdf" H 8700 2550 60  0001 C CNN
F 4 "FTR K1CK005W" V 8700 2550 60  0001 C CNN "Reichelt"
	1    8700 2550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R17
U 1 1 578BCF9C
P 3750 1300
F 0 "R17" V 3350 1450 60  0000 C CNN
F 1 "VDR 30V" H 3850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 3750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 3750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 3750 1300 60  0001 C CNN "Reichelt"
	1    3750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R14
U 1 1 578BD15C
P 750 1300
F 0 "R14" V 350 1450 60  0000 C CNN
F 1 "VDR 30V" H 850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 750 1300 60  0001 C CNN "Reichelt"
	1    750  1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R15
U 1 1 578BD162
P 1750 1300
F 0 "R15" V 1350 1450 60  0000 C CNN
F 1 "VDR 30V" H 1850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 1750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 1750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 1750 1300 60  0001 C CNN "Reichelt"
	1    1750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R20
U 1 1 578BD566
P 6750 1300
F 0 "R20" V 6350 1450 60  0000 C CNN
F 1 "VDR 30V" H 6850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 6750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 6750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 6750 1300 60  0001 C CNN "Reichelt"
	1    6750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R21
U 1 1 578BD56C
P 7750 1300
F 0 "R21" V 7350 1450 60  0000 C CNN
F 1 "VDR 30V" H 7850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 7750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 7750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 7750 1300 60  0001 C CNN "Reichelt"
	1    7750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R18
U 1 1 578BD572
P 4750 1300
F 0 "R18" V 4350 1450 60  0000 C CNN
F 1 "VDR 30V" H 4850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 4750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 4750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 4750 1300 60  0001 C CNN "Reichelt"
	1    4750 1300
	0    1    1    0   
$EndComp
$Comp
L replicator:RESISTOR_VOLTAGE_DEPENDEND_VARISTOR R19
U 1 1 578BD578
P 5750 1300
F 0 "R19" V 5350 1450 60  0000 C CNN
F 1 "VDR 30V" H 5850 1550 60  0000 C CNN
F 2 "replicator:R_1206_HandSoldering" H 5750 1300 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B400/VC%201206_Serie.pdf" H 5750 1300 60  0001 C CNN
F 4 "VC 1206-30" V 5750 1300 60  0001 C CNN "Reichelt"
	1    5750 1300
	0    1    1    0   
$EndComp
Text Label 800  3100 3    60   ~ 0
ULN_COMM
Text Label 1800 3100 3    60   ~ 0
ULN_COMM
Text Label 2800 3100 3    60   ~ 0
ULN_COMM
Text Label 3800 3100 3    60   ~ 0
ULN_COMM
Text Label 4800 3100 3    60   ~ 0
ULN_COMM
Text Label 5800 3100 3    60   ~ 0
ULN_COMM
Text Label 6800 3100 3    60   ~ 0
ULN_COMM
Text Label 7800 3100 3    60   ~ 0
ULN_COMM
$Comp
L replicator:ULN2803D U5
U 1 1 578BE2C6
P 2050 4700
F 0 "U5" H 1800 5250 70  0000 C CNN
F 1 "ULN2803D" H 2200 4000 70  0000 C CNN
F 2 "replicator:SOIC-18_7.5x11.6mm_Pitch1.27mm" H 2050 4700 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A200/ULN2803.pdf" H 2050 4700 60  0001 C CNN
F 4 "ULN 2803 D" H 2050 4700 60  0001 C CNN "Reichelt"
	1    2050 4700
	0    -1   -1   0   
$EndComp
Text Label 800  1900 0    60   ~ 0
M1
Text Label 1800 1900 0    60   ~ 0
M2
Text Label 2800 1900 0    60   ~ 0
M3
Text Label 3800 1900 0    60   ~ 0
M4
Text Label 4800 1900 0    60   ~ 0
M5
Text Label 5800 1900 0    60   ~ 0
M6
Text Label 6800 1900 0    60   ~ 0
M7
Text Label 7800 1900 0    60   ~ 0
M8
Text Label 10100 750  2    60   ~ 0
M1
Text Label 10100 850  2    60   ~ 0
M2
Text Label 10100 950  2    60   ~ 0
M3
Text Label 10100 1050 2    60   ~ 0
M4
Text Label 10100 1150 2    60   ~ 0
M5
Text Label 10100 1250 2    60   ~ 0
M6
Text Label 10100 1350 2    60   ~ 0
M7
Text Label 10100 1450 2    60   ~ 0
M8
Text Label 10100 1550 2    60   ~ 0
VAC
Text Label 10100 1650 2    60   ~ 0
VAC
Text Label 7750 700  0    60   ~ 0
VAC
$Comp
L replicator:MOC3023M OK1
U 1 1 578BFDC9
P 8000 5450
F 0 "OK1" H 7724 5674 50  0000 L BNN
F 1 "MOC3020" H 7724 5153 50  0000 L BNN
F 2 "replicator:MOC_3020" H 8000 5600 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A200/DS_MOC3020-M.pdf" H 8000 5450 60  0001 C CNN
F 4 "MOC 3020 SMD" H 8000 5450 60  0001 C CNN "Reichelt"
	1    8000 5450
	1    0    0    1   
$EndComp
$Comp
L replicator:TRIAC U4
U 1 1 578BFEC5
P 8800 4550
F 0 "U4" H 8550 4900 50  0000 C CNN
F 1 "BTA 16/400B" H 9150 4300 50  0000 C CNN
F 2 "replicator:TO220_VERT" H 8800 4550 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A400/BTA16.pdf" H 8800 4550 60  0001 C CNN
F 4 "BTA 16/400B" H 8800 4550 60  0001 C CNN "Reichelt"
	1    8800 4550
	1    0    0    1   
$EndComp
$Comp
L replicator:R R5
U 1 1 578C01D1
P 8800 5150
F 0 "R5" V 8880 5150 50  0000 C CNN
F 1 "390" V 8800 5150 50  0000 C CNN
F 2 "replicator:R_0603_HandSoldering" V 8730 5150 50  0001 C CNN
F 3 "" H 8800 5150 50  0000 C CNN
F 4 "SMD-0603 390" V 8800 5150 60  0001 C CNN "Reichelt"
	1    8800 5150
	1    0    0    -1  
$EndComp
$Comp
L replicator:FUSE F1
U 1 1 578C0265
P 9200 5000
F 0 "F1" H 9300 5050 50  0000 C CNN
F 1 "6.3A" H 9100 4950 50  0000 C CNN
F 2 "replicator:Fuse_5mm" H 9200 5000 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C400/RND_FUSE_HOLDERS_PCB_OPEN_ENG_TDS.pdf" H 9200 5000 60  0001 C CNN
F 4 "RND 170-00001" H 9200 5000 60  0001 C CNN "Reichelt"
	1    9200 5000
	1    0    0    -1  
$EndComp
Text Label 9450 5000 0    60   ~ 0
VAC
$Comp
L replicator:R R3
U 1 1 578C04CE
P 8550 4050
F 0 "R3" V 8630 4050 50  0000 C CNN
F 1 "390" V 8550 4050 50  0000 C CNN
F 2 "replicator:R_0603_HandSoldering" V 8480 4050 50  0001 C CNN
F 3 "" H 8550 4050 50  0000 C CNN
F 4 "SMD-0603 390" V 8550 4050 60  0001 C CNN "Reichelt"
	1    8550 4050
	0    1    1    0   
$EndComp
$Comp
L replicator:LED D1
U 1 1 578C07EE
P 7350 5550
F 0 "D1" H 7350 5650 50  0000 C CNN
F 1 "LED" H 7350 5450 50  0000 C CNN
F 2 "replicator:LED-5MM" H 7350 5550 60  0001 C CNN
F 3 "" H 7350 5550 60  0000 C CNN
F 4 "LED 5MM ST RT" H 7350 5550 60  0001 C CNN "Reichelt"
	1    7350 5550
	1    0    0    -1  
$EndComp
$Comp
L replicator:R R4
U 1 1 578C0855
P 7000 5550
F 0 "R4" V 7080 5550 50  0000 C CNN
F 1 "100" V 7000 5550 50  0000 C CNN
F 2 "replicator:R_0603_HandSoldering" V 6930 5550 50  0001 C CNN
F 3 "" H 7000 5550 50  0000 C CNN
F 4 "SMD-0603 100" V 7000 5550 60  0001 C CNN "Reichelt"
	1    7000 5550
	0    1    1    0   
$EndComp
$Comp
L replicator:R_PACK4 RP1
U 1 1 578C1077
P 1350 5850
F 0 "RP1" H 1350 6300 50  0000 C CNN
F 1 "3K3" H 1350 5800 50  0000 C CNN
F 2 "replicator:R_Array_Convex_4x1206" H 1350 5850 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B300/BCN%23BI.pdf" H 1350 5850 50  0001 C CNN
F 4 "BCN16 3,3K" H 1350 5850 60  0001 C CNN "Reichelt"
	1    1350 5850
	-1   0    0    -1  
$EndComp
$Comp
L replicator:GND #PWR01
U 1 1 578C10DC
P 2550 5350
F 0 "#PWR01" H 2550 5350 30  0001 C CNN
F 1 "GND" H 2550 5280 30  0001 C CNN
F 2 "" H 2550 5350 60  0000 C CNN
F 3 "" H 2550 5350 60  0000 C CNN
	1    2550 5350
	1    0    0    -1  
$EndComp
$Comp
L replicator:R_PACK4 RP2
U 1 1 578C113C
P 2650 5850
F 0 "RP2" H 2650 6300 50  0000 C CNN
F 1 "3K3" H 2650 5800 50  0000 C CNN
F 2 "replicator:R_Array_Convex_4x1206" H 2650 5850 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B300/BCN%23BI.pdf" H 2650 5850 50  0001 C CNN
F 4 "BCN16 3,3K" H 2650 5850 60  0001 C CNN "Reichelt"
	1    2650 5850
	1    0    0    -1  
$EndComp
$Comp
L replicator:PCF8574A U2
U 1 1 578C1362
P 2050 6500
F 0 "U2" H 1700 7100 50  0000 L CNN
F 1 "PCF8574AT" H 2150 7100 50  0000 L CNN
F 2 "replicator:SOIC-16_7.5x10.3mm_Pitch1.27mm" H 2050 6500 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A300/PCF8574%23PHI.pdf" H 2050 6500 50  0001 C CNN
F 4 "PCF 8574 AT" H 2050 6500 60  0001 C CNN "Reichelt"
	1    2050 6500
	0    -1   -1   0   
$EndComp
$Comp
L replicator:+5V #PWR02
U 1 1 578C2152
P 2850 5900
F 0 "#PWR02" H 2850 5850 20  0001 C CNN
F 1 "+5V" H 2850 6000 30  0000 C CNN
F 2 "" H 2850 5900 60  0000 C CNN
F 3 "" H 2850 5900 60  0000 C CNN
	1    2850 5900
	-1   0    0    1   
$EndComp
$Comp
L replicator:+5V #PWR03
U 1 1 578C240F
P 1150 5900
F 0 "#PWR03" H 1150 5850 20  0001 C CNN
F 1 "+5V" H 1150 6000 30  0000 C CNN
F 2 "" H 1150 5900 60  0000 C CNN
F 3 "" H 1150 5900 60  0000 C CNN
	1    1150 5900
	-1   0    0    1   
$EndComp
Text Label 1650 7000 3    60   ~ 0
SCL
Text Label 1750 7000 3    60   ~ 0
SDA
$Comp
L replicator:C C4
U 1 1 578C257A
P 2300 7200
F 0 "C4" H 2325 7300 50  0000 L CNN
F 1 "100n" H 2325 7100 50  0000 L CNN
F 2 "replicator:C_0603_HandSoldering" H 2338 7050 50  0001 C CNN
F 3 "" H 2300 7200 50  0000 C CNN
F 4 "X7R-G0603 100N" H 2300 7200 60  0001 C CNN "Reichelt"
	1    2300 7200
	0    1    1    0   
$EndComp
NoConn ~ 2450 7000
$Comp
L replicator:GND #PWR04
U 1 1 578C29A4
P 2750 7250
F 0 "#PWR04" H 2750 7250 30  0001 C CNN
F 1 "GND" H 2750 7180 30  0001 C CNN
F 2 "" H 2750 7250 60  0000 C CNN
F 3 "" H 2750 7250 60  0000 C CNN
	1    2750 7250
	1    0    0    -1  
$EndComp
$Comp
L replicator:+5V #PWR05
U 1 1 578C2AA5
P 1350 7200
F 0 "#PWR05" H 1350 7150 20  0001 C CNN
F 1 "+5V" H 1350 7300 30  0000 C CNN
F 2 "" H 1350 7200 60  0000 C CNN
F 3 "" H 1350 7200 60  0000 C CNN
	1    1350 7200
	-1   0    0    1   
$EndComp
$Comp
L replicator:ULN2803D U6
U 1 1 578C3A0F
P 6200 5050
F 0 "U6" H 5950 5600 70  0000 C CNN
F 1 "ULN2803D" H 6350 4350 70  0000 C CNN
F 2 "replicator:SOIC-18_7.5x11.6mm_Pitch1.27mm" H 6200 5050 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A200/ULN2803.pdf" H 6200 5050 60  0001 C CNN
F 4 "ULN 2803 D" H 6200 5050 60  0001 C CNN "Reichelt"
	1    6200 5050
	1    0    0    -1  
$EndComp
$Comp
L replicator:R_PACK4 RP3
U 1 1 578C3A15
P 4800 4350
F 0 "RP3" H 4800 4800 50  0000 C CNN
F 1 "3K3" H 4800 4300 50  0000 C CNN
F 2 "replicator:R_Array_Convex_4x1206" H 4800 4350 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B300/BCN%23BI.pdf" H 4800 4350 50  0001 C CNN
F 4 "BCN16 3,3K" H 4800 4350 60  0001 C CNN "Reichelt"
	1    4800 4350
	0    1    -1   0   
$EndComp
$Comp
L replicator:GND #PWR06
U 1 1 578C3A1B
P 5550 5550
F 0 "#PWR06" H 5550 5550 30  0001 C CNN
F 1 "GND" H 5550 5480 30  0001 C CNN
F 2 "" H 5550 5550 60  0000 C CNN
F 3 "" H 5550 5550 60  0000 C CNN
	1    5550 5550
	0    1    1    0   
$EndComp
$Comp
L replicator:R_PACK4 RP4
U 1 1 578C3A21
P 4800 5650
F 0 "RP4" H 4800 6100 50  0000 C CNN
F 1 "3K3" H 4800 5600 50  0000 C CNN
F 2 "replicator:R_Array_Convex_4x1206" H 4800 5650 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/B300/BCN%23BI.pdf" H 4800 5650 50  0001 C CNN
F 4 "BCN16 3,3K" H 4800 5650 60  0001 C CNN "Reichelt"
	1    4800 5650
	0    1    1    0   
$EndComp
$Comp
L replicator:PCF8574A U3
U 1 1 578C3A27
P 4150 5050
F 0 "U3" H 3800 5650 50  0000 L CNN
F 1 "PCF8574AT" H 4250 5650 50  0000 L CNN
F 2 "replicator:SOIC-16_7.5x10.3mm_Pitch1.27mm" H 4150 5050 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A300/PCF8574%23PHI.pdf" H 4150 5050 50  0001 C CNN
F 4 "PCF 8574 AT" H 4150 5050 60  0001 C CNN "Reichelt"
	1    4150 5050
	1    0    0    -1  
$EndComp
$Comp
L replicator:+5V #PWR07
U 1 1 578C3A48
P 4750 5850
F 0 "#PWR07" H 4750 5800 20  0001 C CNN
F 1 "+5V" H 4750 5950 30  0000 C CNN
F 2 "" H 4750 5850 60  0000 C CNN
F 3 "" H 4750 5850 60  0000 C CNN
	1    4750 5850
	0    -1   -1   0   
$EndComp
Text Label 3650 4650 2    60   ~ 0
SCL
Text Label 3650 4750 2    60   ~ 0
SDA
$Comp
L replicator:C C5
U 1 1 578C3A5B
P 3450 5300
F 0 "C5" H 3475 5400 50  0000 L CNN
F 1 "100n" H 3475 5200 50  0000 L CNN
F 2 "replicator:C_0603_HandSoldering" H 3488 5150 50  0001 C CNN
F 3 "" H 3450 5300 50  0000 C CNN
F 4 "X7R-G0603 100N" H 3450 5300 60  0001 C CNN "Reichelt"
	1    3450 5300
	-1   0    0    1   
$EndComp
NoConn ~ 3650 5450
$Comp
L replicator:GND #PWR08
U 1 1 578C3A63
P 3400 5750
F 0 "#PWR08" H 3400 5750 30  0001 C CNN
F 1 "GND" H 3400 5680 30  0001 C CNN
F 2 "" H 3400 5750 60  0000 C CNN
F 3 "" H 3400 5750 60  0000 C CNN
	1    3400 5750
	0    1    1    0   
$EndComp
$Comp
L replicator:+5V #PWR09
U 1 1 578C3A69
P 3450 4350
F 0 "#PWR09" H 3450 4300 20  0001 C CNN
F 1 "+5V" H 3450 4450 30  0000 C CNN
F 2 "" H 3450 4350 60  0000 C CNN
F 3 "" H 3450 4350 60  0000 C CNN
	1    3450 4350
	0    -1   -1   0   
$EndComp
$Comp
L replicator:+5V #PWR010
U 1 1 578C3D82
P 4750 4150
F 0 "#PWR010" H 4750 4100 20  0001 C CNN
F 1 "+5V" H 4750 4250 30  0000 C CNN
F 2 "" H 4750 4150 60  0000 C CNN
F 3 "" H 4750 4150 60  0000 C CNN
	1    4750 4150
	0    -1   -1   0   
$EndComp
NoConn ~ 5550 5250
NoConn ~ 12850 1150
$Comp
L replicator:+5V #PWR011
U 1 1 578C6F9D
P 700 6700
F 0 "#PWR011" H 700 6650 20  0001 C CNN
F 1 "+5V" H 700 6800 30  0000 C CNN
F 2 "" H 700 6700 60  0000 C CNN
F 3 "" H 700 6700 60  0000 C CNN
	1    700  6700
	1    0    0    -1  
$EndComp
$Comp
L replicator:PWR_FLAG #FLG012
U 1 1 578C7076
P 700 6800
F 0 "#FLG012" H 700 6895 30  0001 C CNN
F 1 "PWR_FLAG" H 700 6980 30  0000 C CNN
F 2 "" H 700 6800 60  0000 C CNN
F 3 "" H 700 6800 60  0000 C CNN
	1    700  6800
	-1   0    0    1   
$EndComp
$Comp
L replicator:PWR_FLAG #FLG013
U 1 1 578C70EE
P 950 6800
F 0 "#FLG013" H 950 6895 30  0001 C CNN
F 1 "PWR_FLAG" H 950 6980 30  0000 C CNN
F 2 "" H 950 6800 60  0000 C CNN
F 3 "" H 950 6800 60  0000 C CNN
	1    950  6800
	-1   0    0    1   
$EndComp
$Comp
L replicator:GND #PWR014
U 1 1 578C715F
P 950 6650
F 0 "#PWR014" H 950 6650 30  0001 C CNN
F 1 "GND" H 950 6580 30  0001 C CNN
F 2 "" H 950 6650 60  0000 C CNN
F 3 "" H 950 6650 60  0000 C CNN
	1    950  6650
	-1   0    0    1   
$EndComp
Text Label 2750 4050 0    60   ~ 0
ULN_COMM
$Comp
L replicator:+5V #PWR015
U 1 1 578C7FE9
P 2550 4050
F 0 "#PWR015" H 2550 4000 20  0001 C CNN
F 1 "+5V" H 2550 4150 30  0000 C CNN
F 2 "" H 2550 4050 60  0000 C CNN
F 3 "" H 2550 4050 60  0000 C CNN
	1    2550 4050
	1    0    0    -1  
$EndComp
NoConn ~ 800  2000
$Comp
L replicator:4528 U1
U 2 1 578C9FE1
P 5400 6650
F 0 "U1" H 5450 7050 60  0000 C CNN
F 1 "4538B" H 5600 6250 60  0000 C CNN
F 2 "replicator:SO-16-N" H 5400 6650 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A200/smd_4538.pdf" H 5400 6650 60  0001 C CNN
F 4 "SMD 4538" H 5400 6650 60  0001 C CNN "Reichelt"
	2    5400 6650
	-1   0    0    1   
$EndComp
NoConn ~ 4800 6350
$Comp
L replicator:GND #PWR016
U 1 1 578CA33E
P 6000 6550
F 0 "#PWR016" H 6000 6550 30  0001 C CNN
F 1 "GND" H 6000 6480 30  0001 C CNN
F 2 "" H 6000 6550 60  0000 C CNN
F 3 "" H 6000 6550 60  0000 C CNN
	1    6000 6550
	0    -1   -1   0   
$EndComp
$Comp
L replicator:CP C1
U 1 1 578CA533
P 6250 6900
F 0 "C1" H 6275 7000 50  0000 L CNN
F 1 "100u" H 6275 6800 50  0000 L CNN
F 2 "replicator:c_elec_6.3x5.8" H 6288 6750 50  0001 C CNN
F 3 "" H 6250 6900 50  0000 C CNN
F 4 "VF 100/16 P-D" H 6250 6900 60  0001 C CNN "Reichelt"
	1    6250 6900
	-1   0    0    1   
$EndComp
$Comp
L replicator:POT RV1
U 1 1 578CA5A6
P 6250 7300
F 0 "RV1" H 6250 7200 50  0000 C CNN
F 1 "1M" H 6250 7300 50  0000 C CNN
F 2 "replicator:Potentiometer_Bourns_3296W_3-8Zoll_Inline_ScrewUp" H 6250 7300 60  0001 C CNN
F 3 "" H 6250 7300 60  0001 C CNN
F 4 "64W-1,0M" H 6250 7300 60  0001 C CNN "Reichelt"
	1    6250 7300
	1    0    0    -1  
$EndComp
NoConn ~ 6500 7300
$Comp
L replicator:+5V #PWR017
U 1 1 578CA8EF
P 6000 7300
F 0 "#PWR017" H 6000 7250 20  0001 C CNN
F 1 "+5V" H 6000 7400 30  0000 C CNN
F 2 "" H 6000 7300 60  0000 C CNN
F 3 "" H 6000 7300 60  0000 C CNN
	1    6000 7300
	0    -1   -1   0   
$EndComp
$Comp
L replicator:+5V #PWR018
U 1 1 578CAB32
P 6850 5550
F 0 "#PWR018" H 6850 5500 20  0001 C CNN
F 1 "+5V" H 6850 5650 30  0000 C CNN
F 2 "" H 6850 5550 60  0000 C CNN
F 3 "" H 6850 5550 60  0000 C CNN
	1    6850 5550
	1    0    0    -1  
$EndComp
$Comp
L replicator:CONN_3 K1
U 1 1 578CCA43
P 10450 2150
F 0 "K1" V 10400 2150 50  0000 C CNN
F 1 "LED" V 10500 2150 40  0000 C CNN
F 2 "replicator:PSS254-3G" H 10450 2150 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/PSS254%23IMP.pdf" H 10450 2150 60  0001 C CNN
F 4 "PSS 254/3G" V 10450 2150 60  0001 C CNN "Reichelt"
	1    10450 2150
	1    0    0    -1  
$EndComp
$Comp
L replicator:CONN_5 P4
U 1 1 578CCAB8
P 10450 3200
F 0 "P4" V 10400 3200 50  0000 C CNN
F 1 "Expansion" V 10500 3200 50  0000 C CNN
F 2 "replicator:PSS254-5G" H 10450 3200 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/PSS254%23IMP.pdf" H 10450 3200 60  0001 C CNN
F 4 "PSS 254/5G" V 10450 3200 60  0001 C CNN "Reichelt"
	1    10450 3200
	1    0    0    -1  
$EndComp
$Comp
L replicator:CONN_5 P3
U 1 1 578CCB23
P 10450 3900
F 0 "P3" V 10400 3900 50  0000 C CNN
F 1 "Power" V 10500 3900 50  0000 C CNN
F 2 "replicator:PSS254-5G" H 10450 3900 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/PSS254%23IMP.pdf" H 10450 3900 60  0001 C CNN
F 4 "PSS 254/3G" V 10450 3900 60  0001 C CNN "Reichelt"
	1    10450 3900
	1    0    0    -1  
$EndComp
$Comp
L replicator:CONN_3 P2
U 1 1 578CCC1A
P 10450 2600
F 0 "P2" V 10400 2600 50  0000 C CNN
F 1 "I2C" V 10500 2600 40  0000 C CNN
F 2 "replicator:PSS254-3G" H 10450 2600 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/PSS254%23IMP.pdf" H 10450 2600 60  0001 C CNN
F 4 "PSS 254/3G" V 10450 2600 60  0001 C CNN "Reichelt"
	1    10450 2600
	1    0    0    -1  
$EndComp
$Comp
L replicator:R R1
U 1 1 578CCCE2
P 7150 4650
F 0 "R1" V 7050 4800 50  0000 C CNN
F 1 "470" V 7150 4650 50  0000 C CNN
F 2 "replicator:R_0603_HandSoldering" V 7080 4650 50  0001 C CNN
F 3 "" H 7150 4650 50  0000 C CNN
F 4 "SMD-0603 100" V 7150 4650 60  0001 C CNN "Reichelt"
	1    7150 4650
	0    1    1    0   
$EndComp
$Comp
L replicator:R R2
U 1 1 578CCF46
P 7150 4750
F 0 "R2" V 7250 4900 50  0000 C CNN
F 1 "470" V 7150 4750 50  0000 C CNN
F 2 "replicator:R_0603_HandSoldering" V 7080 4750 50  0001 C CNN
F 3 "" H 7150 4750 50  0000 C CNN
F 4 "SMD-0603 100" V 7150 4750 60  0001 C CNN "Reichelt"
	1    7150 4750
	0    1    1    0   
$EndComp
Text Label 7450 4650 0    60   ~ 0
K1_1
Text Label 7450 4750 0    60   ~ 0
K1_2
Text Label 10000 2050 2    60   ~ 0
K1_1
Text Label 10000 2150 2    60   ~ 0
K1_2
$Comp
L replicator:+5V #PWR019
U 1 1 578CDEE1
P 10100 2250
F 0 "#PWR019" H 10100 2200 20  0001 C CNN
F 1 "+5V" H 10100 2350 30  0000 C CNN
F 2 "" H 10100 2250 60  0000 C CNN
F 3 "" H 10100 2250 60  0000 C CNN
	1    10100 2250
	0    -1   -1   0   
$EndComp
$Comp
L replicator:GND #PWR020
U 1 1 578CE41F
P 10050 2750
F 0 "#PWR020" H 10050 2750 30  0001 C CNN
F 1 "GND" H 10050 2680 30  0001 C CNN
F 2 "" H 10050 2750 60  0000 C CNN
F 3 "" H 10050 2750 60  0000 C CNN
	1    10050 2750
	1    0    0    -1  
$EndComp
Text Label 10100 2500 2    60   ~ 0
SCL
Text Label 10100 2600 2    60   ~ 0
SDA
Text Label 10050 3000 2    60   ~ 0
P4_1
Text Label 10050 3100 2    60   ~ 0
P4_2
Text Label 10050 3200 2    60   ~ 0
P4_3
Text Label 10050 3300 2    60   ~ 0
P4_4
Text Label 10050 3400 2    60   ~ 0
P4_5
Text Label 6850 4850 0    60   ~ 0
P4_1
Text Label 6850 4950 0    60   ~ 0
P4_2
Text Label 6850 5050 0    60   ~ 0
P4_3
Text Label 6850 5150 0    60   ~ 0
P4_4
Text Label 6850 5250 0    60   ~ 0
P4_5
NoConn ~ 10050 3700
NoConn ~ 10050 4100
$Comp
L replicator:GND #PWR021
U 1 1 578CF39C
P 9800 4100
F 0 "#PWR021" H 9800 4100 30  0001 C CNN
F 1 "GND" H 9800 4030 30  0001 C CNN
F 2 "" H 9800 4100 60  0000 C CNN
F 3 "" H 9800 4100 60  0000 C CNN
	1    9800 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 3000 1650 4050
Wire Wire Line
	2350 4050 2350 3900
Wire Wire Line
	2350 3900 8650 3900
Wire Wire Line
	8650 3900 8650 3000
Wire Wire Line
	7650 3000 7650 3850
Wire Wire Line
	7650 3850 2250 3850
Wire Wire Line
	2250 3850 2250 4050
Wire Wire Line
	6650 3000 6650 3800
Wire Wire Line
	6650 3800 2150 3800
Wire Wire Line
	2150 3800 2150 4050
Wire Wire Line
	5650 3000 5650 3750
Wire Wire Line
	5650 3750 2050 3750
Wire Wire Line
	2050 3750 2050 4050
Wire Wire Line
	4650 3000 4650 3700
Wire Wire Line
	4650 3700 1950 3700
Wire Wire Line
	1950 3700 1950 4050
Wire Wire Line
	3650 3000 3650 3650
Wire Wire Line
	3650 3650 1850 3650
Wire Wire Line
	1850 3650 1850 4050
Wire Wire Line
	2650 3000 2650 3600
Wire Wire Line
	2650 3600 1750 3600
Wire Wire Line
	1750 3600 1750 4050
Wire Wire Line
	800  3000 800  3100
Wire Wire Line
	1800 3000 1800 3100
Wire Wire Line
	2800 3000 2800 3100
Wire Wire Line
	3800 3000 3800 3100
Wire Wire Line
	4800 3000 4800 3100
Wire Wire Line
	5800 3000 5800 3100
Wire Wire Line
	6800 3000 6800 3100
Wire Wire Line
	7800 3000 7800 3100
Wire Wire Line
	7800 2000 7650 2000
Wire Wire Line
	7650 2000 7650 2150
Wire Wire Line
	6800 2000 6650 2000
Wire Wire Line
	6650 2000 6650 2150
Wire Wire Line
	5800 2000 5650 2000
Wire Wire Line
	5650 2000 5650 2150
Wire Wire Line
	4800 2000 4650 2000
Wire Wire Line
	4650 2000 4650 2150
Wire Wire Line
	3800 2000 3650 2000
Wire Wire Line
	3650 2000 3650 2150
Wire Wire Line
	2800 2000 2650 2000
Wire Wire Line
	2650 2000 2650 2150
Wire Wire Line
	1800 2000 1650 2000
Wire Wire Line
	1650 2000 1650 2150
Wire Wire Line
	7750 1900 7750 2150
Wire Wire Line
	7750 2150 7800 2150
Wire Wire Line
	6750 1900 6750 2150
Wire Wire Line
	6750 2150 6800 2150
Wire Wire Line
	5750 1900 5750 2150
Wire Wire Line
	5750 2150 5800 2150
Wire Wire Line
	4750 1900 4750 2150
Wire Wire Line
	4750 2150 4800 2150
Wire Wire Line
	3750 1900 3750 2150
Wire Wire Line
	3750 2150 3800 2150
Wire Wire Line
	2750 1900 2750 2150
Wire Wire Line
	2750 2150 2800 2150
Wire Wire Line
	1750 1900 1750 2150
Wire Wire Line
	1750 2150 1800 2150
Wire Wire Line
	750  1900 750  2150
Wire Wire Line
	750  2150 800  2150
Wire Wire Line
	750  700  1750 700 
Connection ~ 1750 700 
Connection ~ 2750 700 
Connection ~ 3750 700 
Connection ~ 4750 700 
Connection ~ 5750 700 
Connection ~ 6750 700 
Wire Wire Line
	750  1900 800  1900
Wire Wire Line
	1750 1900 1800 1900
Wire Wire Line
	2750 1900 2800 1900
Wire Wire Line
	3750 1900 3800 1900
Wire Wire Line
	4750 1900 4800 1900
Wire Wire Line
	5750 1900 5800 1900
Wire Wire Line
	6750 1900 6800 1900
Wire Wire Line
	7750 1900 7800 1900
Wire Wire Line
	8800 4950 8800 5000
Wire Wire Line
	8800 5300 8800 5550
Wire Wire Line
	8800 5000 8950 5000
Wire Wire Line
	8300 4050 8300 4350
Wire Wire Line
	8800 2150 8800 4050
Wire Wire Line
	8700 4050 8800 4050
Connection ~ 8800 4050
Wire Wire Line
	8400 4050 8300 4050
Connection ~ 8300 4350
Wire Wire Line
	7600 5550 7550 5550
Wire Wire Line
	1650 5350 1650 5800
Wire Wire Line
	1750 5350 1750 5700
Wire Wire Line
	1850 5350 1850 5600
Wire Wire Line
	1950 5350 1950 5500
Wire Wire Line
	2050 5350 2050 5800
Wire Wire Line
	2150 5350 2150 5700
Wire Wire Line
	2250 5350 2250 5600
Wire Wire Line
	2350 5350 2350 5500
Wire Wire Line
	1550 5800 1650 5800
Connection ~ 1650 5800
Wire Wire Line
	1550 5700 1750 5700
Connection ~ 1750 5700
Wire Wire Line
	1550 5600 1850 5600
Connection ~ 1850 5600
Wire Wire Line
	1550 5500 1950 5500
Connection ~ 1950 5500
Wire Wire Line
	2450 5800 2050 5800
Connection ~ 2050 5800
Wire Wire Line
	2450 5700 2150 5700
Connection ~ 2150 5700
Wire Wire Line
	2450 5600 2250 5600
Connection ~ 2250 5600
Wire Wire Line
	2450 5500 2350 5500
Connection ~ 2350 5500
Wire Wire Line
	2850 5500 2850 5600
Connection ~ 2850 5600
Connection ~ 2850 5700
Connection ~ 2850 5800
Wire Wire Line
	1150 5500 1150 5600
Connection ~ 1150 5600
Connection ~ 1150 5700
Connection ~ 1150 5800
Wire Wire Line
	2750 6500 2750 7200
Wire Wire Line
	2450 7200 2750 7200
Connection ~ 2750 7200
Wire Wire Line
	1350 6500 1350 7200
Wire Wire Line
	1350 7200 2150 7200
Wire Wire Line
	1950 7000 2050 7000
Connection ~ 2050 7000
Wire Wire Line
	2150 7000 2150 7200
Wire Wire Line
	4650 4650 4850 4650
Wire Wire Line
	4650 4750 4950 4750
Wire Wire Line
	4650 4850 5050 4850
Wire Wire Line
	4650 4950 5150 4950
Wire Wire Line
	4650 5050 4850 5050
Wire Wire Line
	4650 5150 4950 5150
Wire Wire Line
	4850 4550 4850 4650
Connection ~ 4850 4650
Wire Wire Line
	4950 4550 4950 4750
Connection ~ 4950 4750
Wire Wire Line
	5050 4550 5050 4850
Connection ~ 5050 4850
Wire Wire Line
	5150 4550 5150 4950
Connection ~ 5150 4950
Wire Wire Line
	4850 5450 4850 5050
Connection ~ 4850 5050
Wire Wire Line
	4950 5450 4950 5150
Connection ~ 4950 5150
Wire Wire Line
	5050 5450 5050 5250
Wire Wire Line
	5150 5450 5150 5350
Wire Wire Line
	4750 5850 4850 5850
Connection ~ 5050 5850
Connection ~ 4950 5850
Connection ~ 4850 5850
Wire Wire Line
	3400 5750 3450 5750
Wire Wire Line
	3450 5450 3450 5750
Connection ~ 3450 5750
Wire Wire Line
	4150 4350 3450 4350
Wire Wire Line
	3450 4350 3450 5050
Wire Wire Line
	4750 4150 4850 4150
Connection ~ 4850 4150
Connection ~ 4950 4150
Connection ~ 5050 4150
Wire Wire Line
	3650 5050 3450 5050
Connection ~ 3450 5050
Wire Wire Line
	3650 4950 3600 4950
Wire Wire Line
	3600 4950 3600 5150
Connection ~ 3600 5750
Wire Wire Line
	3650 5150 3600 5150
Connection ~ 3600 5150
Wire Wire Line
	4650 5250 5050 5250
Wire Wire Line
	4650 5350 5150 5350
Wire Wire Line
	2550 4050 2750 4050
Wire Wire Line
	5400 5250 5400 6000
Connection ~ 5050 5250
Wire Wire Line
	5350 5350 5350 6050
Wire Wire Line
	5350 6050 6000 6050
Wire Wire Line
	6000 6050 6000 6450
Connection ~ 5150 5350
Wire Wire Line
	6000 6750 6250 6750
Wire Wire Line
	6000 6950 6000 7050
Wire Wire Line
	6000 7050 6250 7050
Wire Wire Line
	6250 7050 6250 7150
Wire Wire Line
	4800 6950 4700 6950
Wire Wire Line
	4700 6950 4700 5950
Wire Wire Line
	4700 5950 5450 5950
Wire Wire Line
	5450 5950 5450 5350
Wire Wire Line
	5450 5350 5550 5350
Wire Wire Line
	6850 5350 7600 5350
Wire Wire Line
	8300 5000 8400 5000
Wire Wire Line
	8400 5000 8400 5350
Wire Wire Line
	8800 5550 8400 5550
Wire Wire Line
	10000 2050 10100 2050
Wire Wire Line
	10000 2150 10100 2150
Wire Wire Line
	6850 4650 7000 4650
Wire Wire Line
	6850 4750 7000 4750
Wire Wire Line
	7300 4750 7450 4750
Wire Wire Line
	7300 4650 7450 4650
Wire Wire Line
	10100 2700 10050 2700
Wire Wire Line
	10050 2700 10050 2750
Wire Wire Line
	10050 4000 9800 4000
Wire Wire Line
	9800 3800 9800 4000
Wire Wire Line
	10050 3800 9800 3800
Connection ~ 9800 4000
$Comp
L replicator:+5V #PWR022
U 1 1 578CF7A7
P 10050 3900
F 0 "#PWR022" H 10050 3850 20  0001 C CNN
F 1 "+5V" H 10050 4000 30  0000 C CNN
F 2 "" H 10050 3900 60  0000 C CNN
F 3 "" H 10050 3900 60  0000 C CNN
	1    10050 3900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8650 2150 8800 2150
$Comp
L replicator:4528 U1
U 1 1 578D000A
P 3750 6650
F 0 "U1" H 3800 7050 60  0000 C CNN
F 1 "4538B" H 3950 6250 60  0000 C CNN
F 2 "replicator:SOIC-16_7.5x10.3mm_Pitch1.27mm" H 3750 6650 60  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/A200/smd_4538.pdf" H 3750 6650 60  0001 C CNN
F 4 "SMD 4538" H 3750 6650 60  0001 C CNN "Reichelt"
	1    3750 6650
	-1   0    0    1   
$EndComp
$Comp
L replicator:GND #PWR023
U 1 1 578D01A3
P 4350 6550
F 0 "#PWR023" H 4350 6550 30  0001 C CNN
F 1 "GND" H 4350 6480 30  0001 C CNN
F 2 "" H 4350 6550 60  0000 C CNN
F 3 "" H 4350 6550 60  0000 C CNN
	1    4350 6550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3750 6000 4350 6000
Wire Wire Line
	4350 6000 4350 6450
$Comp
L replicator:+5V #PWR024
U 1 1 578D0354
P 4350 6450
F 0 "#PWR024" H 4350 6400 20  0001 C CNN
F 1 "+5V" H 4350 6550 30  0000 C CNN
F 2 "" H 4350 6450 60  0000 C CNN
F 3 "" H 4350 6450 60  0000 C CNN
	1    4350 6450
	0    1    1    0   
$EndComp
NoConn ~ 4350 6750
NoConn ~ 4350 6950
NoConn ~ 3150 6350
NoConn ~ 3150 6950
$Comp
L replicator:VDD #PWR025
U 1 1 578D1304
P 600 6700
F 0 "#PWR025" H 600 6550 50  0001 C CNN
F 1 "VDD" H 600 6850 50  0000 C CNN
F 2 "" H 600 6700 50  0000 C CNN
F 3 "" H 600 6700 50  0000 C CNN
	1    600  6700
	1    0    0    -1  
$EndComp
$Comp
L replicator:VSS #PWR026
U 1 1 578D1370
P 1100 6700
F 0 "#PWR026" H 1100 6550 50  0001 C CNN
F 1 "VSS" H 1100 6850 50  0000 C CNN
F 2 "" H 1100 6700 50  0000 C CNN
F 3 "" H 1100 6700 50  0000 C CNN
	1    1100 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  6650 950  6700
Wire Wire Line
	700  6700 700  6800
Wire Wire Line
	600  6700 700  6700
Wire Wire Line
	950  6700 1100 6700
Connection ~ 950  6700
$Comp
L replicator:CP C2
U 1 1 578D1FF2
P 10300 4850
F 0 "C2" H 10325 4950 50  0000 L CNN
F 1 "100u" H 10325 4750 50  0000 L CNN
F 2 "replicator:c_elec_6.3x5.8" H 10338 4700 50  0001 C CNN
F 3 "" H 10300 4850 50  0000 C CNN
F 4 "VF 100/16 P-D" H 10300 4850 60  0001 C CNN "Reichelt"
	1    10300 4850
	1    0    0    -1  
$EndComp
$Comp
L replicator:VDD #PWR027
U 1 1 578D20F2
P 10300 4650
F 0 "#PWR027" H 10300 4500 50  0001 C CNN
F 1 "VDD" H 10300 4800 50  0000 C CNN
F 2 "" H 10300 4650 50  0000 C CNN
F 3 "" H 10300 4650 50  0000 C CNN
	1    10300 4650
	1    0    0    -1  
$EndComp
$Comp
L replicator:VSS #PWR028
U 1 1 578D2160
P 10300 5050
F 0 "#PWR028" H 10300 4900 50  0001 C CNN
F 1 "VSS" H 10300 5200 50  0000 C CNN
F 2 "" H 10300 5050 50  0000 C CNN
F 3 "" H 10300 5050 50  0000 C CNN
F 4 "VF 100/16 P-D" H 10300 5050 60  0001 C CNN "Reichelt"
	1    10300 5050
	-1   0    0    1   
$EndComp
Wire Wire Line
	10300 4650 10300 4700
Wire Wire Line
	10300 5000 10300 5050
Wire Wire Line
	1750 700  2750 700 
Wire Wire Line
	2750 700  3750 700 
Wire Wire Line
	3750 700  4750 700 
Wire Wire Line
	4750 700  5750 700 
Wire Wire Line
	5750 700  6750 700 
Wire Wire Line
	6750 700  7750 700 
Wire Wire Line
	8800 4050 8800 4300
Wire Wire Line
	8300 4350 8300 5000
Wire Wire Line
	1650 5800 1650 6000
Wire Wire Line
	1750 5700 1750 6000
Wire Wire Line
	1850 5600 1850 6000
Wire Wire Line
	1950 5500 1950 6000
Wire Wire Line
	2050 5800 2050 6000
Wire Wire Line
	2150 5700 2150 6000
Wire Wire Line
	2250 5600 2250 6000
Wire Wire Line
	2350 5500 2350 6000
Wire Wire Line
	2850 5600 2850 5700
Wire Wire Line
	2850 5700 2850 5800
Wire Wire Line
	2850 5800 2850 5900
Wire Wire Line
	1150 5600 1150 5700
Wire Wire Line
	1150 5700 1150 5800
Wire Wire Line
	1150 5800 1150 5900
Wire Wire Line
	2750 7200 2750 7250
Wire Wire Line
	2050 7000 2150 7000
Wire Wire Line
	4850 4650 5550 4650
Wire Wire Line
	4950 4750 5550 4750
Wire Wire Line
	5050 4850 5550 4850
Wire Wire Line
	5150 4950 5550 4950
Wire Wire Line
	4850 5050 5550 5050
Wire Wire Line
	4950 5150 5550 5150
Wire Wire Line
	5050 5850 5150 5850
Wire Wire Line
	4950 5850 5050 5850
Wire Wire Line
	4850 5850 4950 5850
Wire Wire Line
	3450 5750 3600 5750
Wire Wire Line
	4850 4150 4950 4150
Wire Wire Line
	4950 4150 5050 4150
Wire Wire Line
	5050 4150 5150 4150
Wire Wire Line
	3450 5050 3450 5150
Wire Wire Line
	3600 5750 4150 5750
Wire Wire Line
	3600 5150 3600 5750
Wire Wire Line
	5050 5250 5400 5250
Wire Wire Line
	5150 5350 5350 5350
Wire Wire Line
	9800 4000 9800 4100
Wire Wire Line
	950  6700 950  6800
$EndSCHEMATC
