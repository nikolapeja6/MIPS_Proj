#include "rfid.h"

uchar MFRC522_Write(uchar blockAddr, uchar *writeData);

// Stores the tags serial number when it is read.
//
uchar serNum[5];

// Serial number of the RFID tag that is will be accepted.
//
uchar acceptSerNum[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void transitionToSecure();

uchar RFID_iteration(){
    uchar i,tmp;
    uchar status;
    uchar str[MAX_LEN];
    uchar RC_size;
    uchar blockAddr;	//Select the address of the operation 0~63

    int diff = 0;

    // searching card, return card type
    status = MFRC522_Request(PICC_REQIDL, str);
    if (status == MI_OK)
    {
    }

	status = MFRC522_Anticoll(str);
	memcpy(serNum, str, 5);
	if (status == MI_OK)
	{
        if(acceptSerNum[0] == 0xFF){
        for(i =0; i< 5; i++)
            acceptSerNum[i] = serNum[i];
        }

        for (i = 0; i < 5; i++)
        {
            if (acceptSerNum[i] != serNum[i])
            {
                diff = 1;
            }
        }
        if(diff)
        {
            return 0;
        }
        else
        {
            transitionToSecure();
            return 1;
        }
        return 111;
	}

	return 000;
}