void KeypadSendReadCommAddrssMsg(void)
{
  uint8_t bInd;
  DLTReadCommAddStruct *pMsg;
  uint8_t CheckSum  =0;
  pMsg = (DLTReadCommAddStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = DLT_READ_COMM_ADDRESS;
    for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[5-bInd] = 0xaa;    
  pMsg->Header.Len= 0;
  for (bInd = 0; bInd < sizeof(DLTReadCommAddStruct) -2 ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->CheckSum = CheckSum;
  pMsg->Etx= 0x16;  // 
  DltRxTimeout = 1;	
  TmpTxsizeBuf = sizeof(DLTReadCommAddStruct);
  DltSendBuffer(TmpTxBuf,TmpTxsizeBuf); 
   
}

void KeypadSendReadDltMsg(uint8_t   *pDLTDdata, uint8_t DLTDdataLen,uint8_t DLTid, uint8_t BroadCast)
{ 
  uint8_t bInd;
  DLTReadStruct *pMsg;
  uint8_t CheckSum  =0;
  pMsg = (DLTReadStruct*)&TmpTxBuf;
  pMsg->Header.Stx = 0x68;      // STX
  pMsg->Header.Stx1 = 0x68;      // STX
  pMsg->Header.ReadWriteCmd = 0x11;
  
  // enter DI code
  for (bInd = 0; bInd < 4 ; bInd++)
  {
    pMsg->Data.DICode[3-bInd] = DLTID[DLTid*4+bInd];
    pMsg->Data.DICode[3-bInd] = pMsg->Data.DICode[3-bInd] +0x33;
  
  }
   // Convert hex addres to little endiann BCD 
  if (BroadCast)
    memset((unsigned char *)&pMsg->Header.Add[0],0xaa,6); 
  else 
  {
    for (bInd = 0; bInd < 6; bInd++)
      pMsg->Header.Add[bInd] = EMConfigMacADDR0[bInd];
  }
  pMsg->Header.Len= DLTDdataLen+sizeof(DLTReadDataStruct);
  for (bInd = 0; bInd < DLTDdataLen; bInd++)
  {
     pMsg->Info[bInd] = pDLTDdata[bInd] +0x33;
  }
  for (bInd = 0; bInd < DLTDdataLen+sizeof(DLTWriteHeader)+sizeof(DLTReadDataStruct) ; bInd++)
    CheckSum = CheckSum +TmpTxBuf[bInd];
  pMsg->Info[DLTDdataLen] = CheckSum;
  pMsg->Info[DLTDdataLen+1] = 0x16;  // ETX
  
  DltRxTimeout = 1;
  TmpTxsizeBuf = pMsg->Header.Len +sizeof(DLTWriteHeader)+2;
  DltSendBuffer(TmpTxBuf,TmpTxsizeBuf); 
     
}

35 34  33 37 

0x04, 0x00, 0x01, 0x02,

 if(pMsg->Data.DICode[3-bInd] != (uint8_t)(DLTID[DLTid*4+bInd] +0x33))

   
    68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3a  "h`9F...h.)F4.53:"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 6a 55  37 33 33 33  "333333336AjU7333"
0030:  43 34 83 a6  16                                     "C4..."
2020-09-30 08:28:15 Received 53 bytes from address 63960, from EP 1 to EP 1, travel time: 0.078 s, QoS: unknown
0000:  68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3b  "h`9F...h.)F4.53;"
0010:  33 33 33 33  33 33 33 33  33 33 33 33  33 33 33 33  "3333333333333333"
0020:  33 33 33 33  33 33 33 33  33 33 33 33  33 33 33 33  "3333333333333333"
0030:  33 33 33 f4  16                                     "333.."
0000:  68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3c  "h`9F...h.)F4.53<"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 74 55  36 33 33 33  "333333336AtU6333"
0030:  43 33 83 b0  16                                     "C3..."
       68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3d  "h`9F...h.)F4.53="
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 53 55  33 33 33 33  "333333336ASU3333"
0030:  43 36 83 90  16  



  68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3f  "h`9F...h.)F4.53?"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 5b 55  37 33 33 33  "333333336A[U7333"
0030:  43 39 83 a1  16


  68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 3f  "h`9F...h.)F4.53?"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 5b 55  37 33 33 33  "333333336A[U7333"
0030:  43 39 83 a1  16

0000:  
68 60 39 46 11 98 19 68  91 29 46 34  b3 35 33 40  "h`9F...h.)F4.53@"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 37 55  35 33 33 33  "333333336A7U5333
0030:  43 c9 7c 05  16 

15/10/2020 15:33:27.887 [TX] - FE FE 68 60 39 46 11 98 19 68 11 06 46 34 B3 35 33 40 5D 16 
15/10/2020 15:33:28.057 [RX] - 

FE FE 

68 60 39 46 11 98 19 68 91 29 46 34 B3 35 66 73 D7 67 66 66 D7 67 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 69 74 8F 88 6A 66 66 66 76 FB AF 8A 16 





       68 60 39 46  11 98 19 68  91 29 46 34  b3 35 33 43  "h`9F...h.)F4.53C"
0010:  a4 34 33 33  a4 34 33 33  33 33 33 33  33 33 33 33  ".433.43333333333"
0020:  33 33 33 33  33 33 33 33  36 41 67 55  37 33 33 33  "333333336AgU7333"
0030:  43 33 83 ab  16 "
   
"C.|.."from rs485 direct from runmetertest application 
 Tx. FE FE 68 60 39 46 11 98 19 68 11 06 46 34 B3 35 33 3E 5B 16 
Msg Rx. 
68 60 39 46 11 98 19 68 91 29 46 34 B3 35 33 3E 
A4 34 33 33 A4 34 33 33 33 33 33 33 33 33 33 33 
33 33 33 33 33 33 33 33 36 41 7A 55 36 33 33 33 
43 33 83 B8 16 
TIME 11:00
13/10/2020 12:39:13.368 [RX] - 3F 3F 31 3F 3F 3F FD 
68 60 39 46 11 98 19 68 91 29 46 34 B3 35 33 3E
A4 34 33 33 A4 34 33 33 33 33 33 33 33 33 33 33 
33 33 33 33 33 33 33 33 36 41 7A 55 36 33 33 33
43 33 83 B8 16


- FE FE 68 60 39 46 11 98 19 68 11 06 46 34 B3 35 33 3F 5C 16 
2020-10-14 22:21:28 Received 18 bytes from address 11113, from EP 1 to EP 1, travel time: 0.117 s, QoS: unknown
0000:   ggYfori17$68 44 44 04  00 00 20 68  11 06 46 34  b3 35 33 33  "hDD... h..F4.533"
0010:  5b 16                                               "[."

Yfori17$
TIME 12:00
13/10/2020 12:47:26.766 [RX] - 3F 3F 7A 1D 3F 3F CD 3F 3F 67 FF E9 
68 60 39 46 11 98 19 68 91 29 46 34 B3 35 33 3F
A4 34 33 33 A4 34 33 33 33 33 33 33 33 33 33 33
33 33 33 33 33 33 33 33 36 41 6C 55 33 33 33 33 
43 34 83 A9 16
TIME10

13/10/2020 12:57:09.236 [TX] - FE FE 68 60 39 46 11 98 19 68 11 06 46 34 B3 35 33 3D 5A 16 
13/10/2020 12:57:09.308 [RX] -
68 60 39 46 11 98 19 68 91 29 46 34 B3 35 33 3D
A4 34 33 33 A4 34 33 33 33 33 33 33 33 33 33 33 
33 33 33 33 33 33 33 33 36 41 94 55 36 33 33 33 
43 34 83 D2 16 