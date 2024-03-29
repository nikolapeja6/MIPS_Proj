#ifndef _rfid_h_
#define _rfid_h_

#include "general.h"

#define MAX_LEN               16

// MF522command word
#define PCD_IDLE              0x00               // NO action; cancel current command
#define PCD_AUTHENT           0x0E               // verify key
#define PCD_RECEIVE           0x08               // receive data

#define PCD_TRANSMIT          0x04               // send data
#define PCD_TRANSCEIVE        0x0C               // receive and send data
#define PCD_RESETPHASE        0x0F               // reset
#define PCD_CALCCRC           0x03               // CRC calculation

// Mifare_One Card command word
#define PICC_REQIDL           0x26               // line-tracking area is dormant
#define PICC_REQALL           0x52               // line-tracking area is interfered
#define PICC_ANTICOLL         0x93               // Anti collision
#define PICC_SElECTTAG        0x93               // choose cards
#define PICC_AUTHENT1A        0x60               // Verify A key
#define PICC_AUTHENT1B        0x61               // Verify B key
#define PICC_READ             0x30               // Reader Module
#define PICC_WRITE            0xA0               // letter block

#define PICC_DECREMENT        0xC0
#define PICC_INCREMENT        0xC1
#define PICC_RESTORE          0xC2               // Transfer data to buffer
#define PICC_TRANSFER         0xB0               // Save buffer data
#define PICC_HALT             0x50               // Dormancy

// MF522 Error code returned when communication
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

//------------------MFRC522 Register---------------
// Page 0:Command and Status
#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivlEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A

#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F

// Page 1:Command
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19

#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F

// Page 2:CFG
#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg              0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F

// Page 3:TestRegister
#define     Reserved30            0x30

#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34            0x3F


// Initializes the RC522 module.
//
void MFRC522_Init();

// Writes a byte of data into the given address of the RFID module.
//
void Write_MFRC522(uchar addr, uchar val);

// Reads a byte of data from the given address of the RFID module.
//
uchar Read_MFRC522(uchar addr);

// Sends a request to the RC522 module.
//
uchar MFRC522_Request(uchar reqMode, uchar *TagType);

// Generic function for communicating with the RC522 module.
//
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen);

void MFRC522_Halt();

uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);

uchar MFRC522_SelectTag(uchar *serNum);

void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData);

uchar MFRC522_Anticoll(uchar *serNum);

#endif
