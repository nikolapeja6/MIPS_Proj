#include "rfid.h"

// Naming the pins.
//
sbit SoftSpi_CLK at GPIOB_ODR.B13;
sbit SoftSpi_SDI at GPIOB_IDR.B14;
sbit SoftSpi_SDO at GPIOB_ODR.B15;

sbit SPExpanderCS at GPIOE_ODR.B11;
sbit chipSelectPin at  GPIOE_ODR.B11;

sbit SPExpanderRST at GPIOE_ODR.B13;
sbit NRSTPD at GPIOE_ODR.B13;

// Initializes the RC522 module.
//
void InitRFID(){

    GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_11 | _GPIO_PINMASK_13);
    GPIO_Digital_Output(&GPIOB_BASE, _GPIO_PINMASK_13 | _GPIO_PINMASK_15);

    GPIO_Digital_Input(&GPIOB_BASE,  _GPIO_PINMASK_14 );

    chipSelectPin = 1;

    SPI2_Init();

    // Activate the RFID reader
    chipSelectPin = LOW;
    NRSTPD = HIGH;

    MFRC522_Init();

}

// Writes one byte of data onto the RC522 module.
//
void Write_MFRC522(uchar addr, uchar val)
{
    chipSelectPin = LOW;

    SPI_Write((addr<<1)&0x7E);
    SPI_Write(val);

    chipSelectPin = HIGH;
}

// Reads one byte of data from the RC522 module.
//
uchar Read_MFRC522(uchar addr)
{
    uchar val;

    chipSelectPin = LOW;

    //address format: 1XXXXXX0
    SPI_Write(((addr<<1)&0x7E) | 0x80);
    val = SPI_Read(0x00);

    chipSelectPin = HIGH;

    return val;
}

// Sets the bit mask.
//
void SetBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


// Clears the bit mask.
//
void ClearBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
}

// Turns the antenna on.
//
void AntennaOn()
{
    uchar temp;

    temp = Read_MFRC522(TxControlReg);
    if (!(temp & 0x03))
    {
            SetBitMask(TxControlReg, 0x03);
    }
}

// Turns the antenna off.
//
void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

// Resets the RC522 module.
//
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

// Initializes the RC522 module.
//
void MFRC522_Init()
{
    NRSTPD = HIGH;

    MFRC522_Reset();

    //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    //
    Write_MFRC522(TModeReg, 0x8D);                  //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E);             //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);
    Write_MFRC522(TReloadRegH, 0);

    Write_MFRC522(TxAutoReg, 0x40);                 //100%ASK
    Write_MFRC522(ModeReg, 0x3D);                   //CRC original value 0x6363        ???

    AntennaOn();                                    // open antenna
}


// Sends a request to the RC522 module.
//
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
    uchar status;
    uint backBits;                                  // bits of data received
    Write_MFRC522(BitFramingReg, 0x07);             //TxLastBists = BitFramingReg[2..0]        ???

    TagType[0] = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }

    return status;
}

// Generic function for sending data to and receiving data from the RC522 module.
//
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;

    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:               // card key authentication
        {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE:            // send data in FIFO
        {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
        {
            break;
        }
    }

    Write_MFRC522(CommIEnReg, irqEn|0x80);           // permission for interrupt request
    ClearBitMask(CommIrqReg, 0x80);                  // clear all bits of the interrupt request
    SetBitMask(FIFOLevelReg, 0x80);                  //FlushBuffer=1, FIFO initialize

    Write_MFRC522(CommandReg, PCD_IDLE);             //NO action; clear current command        ???

    // write data into FIFO
    for (i=0; i<sendLen; i++)
    {
        Write_MFRC522(FIFODataReg, sendData[i]);
    }

    // execute command
    Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);             //StartSend=1,transmission of data starts
    }


    // wait for the completion of data transmission
    // adjust i according to clock frequency, max wait time for M1 card operation 25ms        ???
    i = 2000;
    do
    {
        //CommIrqReg[7..0]
        //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);               //StartSend=0

    if (i != 0)
    {
        if(!(Read_MFRC522(ErrorReg) & 0x1B))        //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
                lastBits = Read_MFRC522(ControlReg) & 0x07;

                if (lastBits)
                {
                    *backLen = (n-1)*8 + lastBits;
                }
                else
                {
                    *backLen = n*8;
                }

                if (n == 0)
                {
                    n = 1;
                }

                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                // read the data received in FIFO
                for (i=0; i<n; i++)
                {
                    backData[i] = Read_MFRC522(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    return status;
}


uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
    uchar serNumCheck=0;
    uint unLen;

    Write_MFRC522(BitFramingReg, 0x00);                //TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        // verify card sequence number
        for (i=0; i<4; i++)
        {
            serNumCheck ^= serNum[i];
        }

        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }

    return status;
}

void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);                          //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);                         // clear FIFO pointer
    //Write_MFRC522(CommandReg, PCD_IDLE);

    // write data into FIFO
    for (i=0; i<len; i++)
    {
        Write_MFRC522(FIFODataReg, *(pIndata+i));
    }

    Write_MFRC522(CommandReg, PCD_CALCCRC);

    // wait for completion of CRC calculation
    i = 0xFF;
    do
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));                            //CRCIrq = 1

    // read result from CRC calculation
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


uchar MFRC522_SelectTag(uchar *serNum)
{
    uchar i;
    uchar status;
    uchar size;
    uint recvBits;
    uchar buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;

    for (i=0; i<5; i++)
    {
        buffer[i+2] = *(serNum+i);
    }

    CalulateCRC(buffer, 7, &buffer[7]);                //??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18))
    {
        size = buffer[0];
    }
    else
    {
        size = 0;
    }

    return size;
}

uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[12];

    // Verification instructions + block address + sector password + card sequence number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
        buff[i+2] = *(Sectorkey+i);
    }

    for (i=0; i<4; i++)
    {
        buff[i+8] = *(serNum+i);
    }

    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}

// Reads data from the RC522 module.
//
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}

// Writes data onto the RC522 module.
//
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        // write 16Byte data into FIFO
        for (i=0; i<16; i++)
        {
            buff[i] = *(writeData+i);
        }

        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

void MFRC522_Halt()
{
    uchar status;
    uint unLen;
    uchar buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}