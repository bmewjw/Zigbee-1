#define MFRC522_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "MFRC522.h"

/* ------------------------------------------------------------------------------------------------
 *                                          Applications
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          MFRC522_Init
 * @brief       MFRC522��ʼ��
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_Init(void)
{
  //uint8_t tmp;
  MFRC522_Reset();
  MFRC522_AntennaOff();	
  //MFRC522_AntennaOn();
  //tmp = MFRC522_Read(TxControlReg);����
  MFRC522_ISO14443A();	
}

/**************************************************************************************************
 * @fn          MFRC522_Reset
 * @brief       MFRC522��λ�׶�����
 * @param       n
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_Reset(void)
{
  MFRC522_RST = 1;    
  MFRC522_Delay(1);
  MFRC522_RST = 0;    //�͵�ƽʱ�ж��ڲ��������գ��ر��������Ͽ�����ܽ����ⲿ��·������
  MFRC522_Delay(1);   
  MFRC522_RST = 1;    //�������������ڲ���λ�׶�
  MFRC522_Delay(1);
  
  MFRC522_Write(CommandReg, PCD_RESETPHASE);  //BIT4=PowerDown(0):�������ѹ��� BIT3-BIT0(1111):��λMFRC522
  

  /*TPrescaler 12λԤ��Ƶ�� 110100111110(3390)**/
  MFRC522_Write(TModeReg,0x8D);               //BIT7=TAuto(1):��ʱ������  BIT3-BIT0=TPrescaler_Hi(1101):Ԥ��Ƶ����4λ
  MFRC522_Write(TPrescalerReg,0x3E);          //BIT7-BIT0(00111110)=TPrescaler_Lo:��ʱ��Ԥ��Ƶ����8λ
  /*TreloadVal 16λ��ʱ����װֵ 30*/
  MFRC522_Write(TReloadRegL,0x30);            //��ʱ��Ƶ��f = 6.78Mhz/TPrescaler  T = 1/f = TPrescaler/6.78Mhz
  MFRC522_Write(TReloadRegH,0);               //t = TreloadVal * T =  15ms   ���ҪΪ24ms��TReloadRegL = 0x30
  
  MFRC522_Write(ModeReg, 0x3D);               //BIT1-BIT0=CRCPreset(01):6363
  MFRC522_Write(TxAutoReg, 0x40);	      //100%ASK
}


/**************************************************************************************************
 * @fn          MFRC522_AntennaOff
 * @brief       �ر�����
 * @param       n
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_AntennaOff(void)
{
  MFRC522_ClearBitMask(TxControlReg, 0x03); //TX2 TX1��0,���ر�������
}


/**************************************************************************************************
 * @fn          MFRC522_AntennaOn
 * @brief       ��������
 * @param       n
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_AntennaOn(void)
{
  uint8_t tmp;
  tmp = MFRC522_Read(TxControlReg);
  if(!(tmp & 0x03))
  {
    MFRC522_SetBitMask(TxControlReg,0x03);
  }
}

/**************************************************************************************************
 * @fn          MFRC522_ConfigISOType
 * @brief       ����RC632�Ĺ�����ʽ,ISO14443AЭ���׼
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_ISO14443A(void)
{
   MFRC522_SetBitMask(Status2Reg, 0x08); //��λ����ָʾMIFARE Cypto1��Ԫ��ͨ��������п�������ͨ�ű����ܵ������MFCrypto1On=0
   MFRC522_Write(RxSelReg , 0x86);       //84 �ǽӴ�ʽUART������ѡ���ڲ�ģ�ⲿ�ֵĵ����ź�
   MFRC522_Write(RFCfgReg , 0x7F);       //4F �������źŵ�ѹ����������48dB
   MFRC522_Delay(1);
   MFRC522_AntennaOn();
}


/**************************************************************************************************
 * @fn          MFRC522_CrcCheck
 * @brief       ��MF522����CRCУ��
 * @param       CrcData   -> ҪУ��CRC������
                CrcLen    -> ���ݳ���
                CheckData -> CRCУ����  
 * @return      Size -> �������� 
 **************************************************************************************************
 */
void MFRC522_CrcCheck(uint8_t *CrcData, uint8_t CrcLen, uint8_t *CheckData)
{
  uint8_t i;
  uint8_t CrcIrq;
  
  MFRC522_ClearBitMask(DivIrqReg,0x04);     //BIT2=CRCIrq(0)
  MFRC522_SetBitMask(FIFOLevelReg,0x80);    //BIT7=FlushBuffer(1), FIFO��ʼ��
  
  /*1.��FIFO��д������*/
  for(i=0; i<CrcLen; i++)
  {
    MFRC522_Write(FIFODataReg,CrcData[i]);
  }
  MFRC522_Write(CommandReg,PCD_CALCCRC);    //����CRCЭ��������ִ���Բ���
  
  /*2.�ȴ�CRCУ�����*/
  i = 0xFF;
  do
  {
    CrcIrq = MFRC522_Read(DivIrqReg);       //BIT2=CRCIrq,CRCЭ�������ڴ�����FIFO���������������ݺ���λCRCIRq��־
    i--;
  }
  while((i!=0)&&!(CrcIrq & 0x04));          //1)i=0��ʱ���� 2)������FIFO��������������������
  
  /*3.��ȡCRCУ����*/
  CheckData[0] = MFRC522_Read(CRCResultRegL); 
  CheckData[1] = MFRC522_Read(CRCResultRegM);
  
}



/**************************************************************************************************
 * @fn          MFRC522_RequestCard
 * @brief       Ѱ������ȡ�����ͺ�
 * @param       ReqMode   ->  Ѱ����ʽ
                CardType  ->  ���ؿ�Ƭ����
                    	        0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)  //Mifare_One(S50) У԰��
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * @return      ReqStatus ->  Ѱ�����  
 **************************************************************************************************
 */

uint8_t MFRC522_RequestCard(uint8_t ReqMode,uint8_t *CardType)
{   
  uint8_t ReqStatus = MFRC522_ERR;  
  uint32_t ReqBits;   //���յ���������Чλ��
  
  MFRC522_Write(BitFramingReg,0x07);  //BIT2-BIT0=TxLastBists(111)?
  //ReqStatus = MFRC522_Read(BitFramingReg);
  
  CardType[0] = ReqMode;
  
  ReqStatus = MFRC522_ToCard(PCD_TRANSCEIVE, CardType, 1, CardType, &ReqBits);
  
  if((ReqStatus != MFRC522_OK)||(ReqBits != 0x10))       //������صĲ���16bit 0x0400������16bit
  {
    ReqStatus = MFRC522_ERR;
  }
  
  return ReqStatus; 
}


/**************************************************************************************************
 * @fn          MFRC522_Anticoll
 * @brief       RC522��ISO14443��ͨѶ
 * @param       CardData  -> ��������
 * @return      ReqStatus -> Ѱ����� 
 **************************************************************************************************
 */
uint8_t MFRC522_Anticoll(uint8_t *CardData)
{
  uint8_t i;
  uint8_t AntiStatus = MFRC522_ERR;
  uint32_t AntiBits;    //���յ���������Чλ��
  uint8_t CardDataCheck = 0;

  MFRC522_ClearBitMask(Status2Reg,0x08);  //�ر��ڲ��¶ȴ�����
  MFRC522_Write(BitFramingReg,0x00);      //BIT2-BIT0=TxLastBists(000)  ���巢�͵����һ���ֽڵ�λ����000��ʾ���һ���ֽڵ�����λ��Ӧ����
  MFRC522_ClearBitMask(CollReg,0x80);     //BIT7=ValuesAfterColl(1) 
  
  CardData[0] = PICC_ANTICOLL;
  CardData[1] = 0x20;
  
  AntiStatus = MFRC522_ToCard(PCD_TRANSCEIVE,CardData,2,CardData,&AntiBits);
  
  if(AntiStatus == MFRC522_OK)
  {
    /*У�鿨��*/
    for(i=0; i<4; i++)
    {
      CardDataCheck ^= CardData[i];      //1.datasheet/RFID/MifareCard/1.M1�򵥽��ܣ�4byte���� 1byteУ����
    }
    
    if(CardDataCheck != CardData[i])
    {
      AntiStatus = MFRC522_ERR;
    }
  }
  
  MFRC522_SetBitMask(CollReg,0x80);      //BIT7=ValuesAfterColl(1) 
  return AntiStatus;
}

/**************************************************************************************************
 * @fn          MFRC522_SelectTag
 * @brief       ѡ������ȡ���洢������
 * @param       CardData  -> ��������
 * @return      Size -> �������� 
 **************************************************************************************************
 */
uint8_t MFRC522_SelectTag(uint8_t *CardData)
{
  uint8_t Buff[9];
  uint8_t i;
  uint32_t Bits;
  uint8_t Size;
  uint8_t Status  = MFRC522_ERR; 
  
  MFRC522_ClearBitMask(Status2Reg,0x08);    //BIT3=MFCrypto1On(1)
  Buff[0] = PICC_SElECTTAG;
  Buff[1] = 0x70;                           //?
  for(i=0; i<5; i++)
  {
    Buff[i+2] = CardData[i];
  }
  
  MFRC522_CrcCheck(Buff,7,&Buff[7]);        //buff[7] buff[8]����CRCУ��ĵͰ�λ�͸߰�λ
    
  Status = MFRC522_ToCard(PCD_TRANSCEIVE, Buff, 9, Buff, &Bits);
  
  if((Status == MFRC522_OK) && (Bits == 0x18)) //0x18 = 24 24/8 =3Byte
  {
    Size = Buff[0];
  }
  
  else
  {
    Size = 0;
  }
  
  return Size;
}


/**************************************************************************************************
 * @fn          MFRC522_Auth
 * @brief       ��֤��Ƭ����
 * @param       authMode  -> ������֤ģʽ
                                          0x60 = ��֤A��Կ
                                          0x61 = ��֤B��Կ 
                BlockAddr -> ���ַ(��0-63��)�����п�0���˳��̵ĳ�ʼ���ݣ��������ŵȡ�
                SectorKey -> �������� 6byte
                CardData  -> ���ţ�4Byte
 * @return      �ɹ�����  -> MI_OK 
 **************************************************************************************************
 */
uint8_t MFRC522_Auth(uint8_t AuthMode, uint8_t BlockAddr, uint8_t *SectorKey, uint8_t *CardData)
{
  uint8_t Buff[12];
  uint8_t i;
  uint32_t Bits;
  uint8_t Status  = MFRC522_ERR; 
  
  /*��ָ֤��1byte+���ַ1byte����������6byte�������к�4byte*/
  Buff[0] = AuthMode;
  Buff[1] = BlockAddr;
  
  for(i=0; i<6; i++)
  {
    Buff[i+2] = SectorKey[i];
  }
  
  for(i=0; i<4; i++)
  {
    Buff[i+8] = CardData[i];
  }
  
  Status = MFRC522_ToCard(PCD_AUTHENT, Buff, 12, Buff, &Bits);
  
  if ((Status != MFRC522_OK) || (!(MFRC522_Read(Status2Reg) & 0x08)))    //BIT3=MFCryptolOn(1) �ɹ�ִ��Auth��Ż���1����дģʽʱʹ��
  {   
    Status = MFRC522_ERR;   
  }
  return Status;
}


/**************************************************************************************************
 * @fn          MFRC522_WriteToBlock
 * @brief       д������
 * @param       BlockAddr -> ���ַ(��0-63��)�����п�0���˳��̵ĳ�ʼ���ݣ��������ŵȡ�
                WriteData -> ���д16�ֽ�����
 * @return      �ɹ�����  -> MI_OK 
 **************************************************************************************************
 */
uint8_t MFRC522_WriteToBlock(uint8_t BlockAddr, uint8_t *WriteData)
{
  uint8_t Buff[18];
  uint8_t i;
  uint8_t Status;
  uint32_t Bits;
  
  Buff[0] = PICC_WRITE;
  Buff[1] = BlockAddr;
  
  MFRC522_CrcCheck(Buff,2,&Buff[2]);
  Status = MFRC522_ToCard(PCD_TRANSCEIVE, Buff, 4, Buff, &Bits);
  
  if((Status != MFRC522_OK) || (Bits != 4) || ((Buff[0] & 0x0F) != 0x0A))
  {
     Status = MFRC522_ERR;
  }
  
  if(Status == MFRC522_OK)
  {
    for(i=0; i<16; i++)
    {
      Buff[i] = WriteData[i];
    }
    MFRC522_CrcCheck(Buff,16,&Buff[16]);
    Status = MFRC522_ToCard(PCD_TRANSCEIVE, Buff, 18, Buff, &Bits);
    if((Status != MFRC522_OK) || (Bits != 4) || ((Buff[0] & 0x0F) != 0x0A))
    {
       Status = MFRC522_ERR;
    }
  }
  
  return Status;
}


/**************************************************************************************************
 * @fn          MFRC522_ReadFromBlock
 * @brief       ��������
 * @param       BlockAddr -> ���ַ(��0-63��)�����п�0���˳��̵ĳ�ʼ���ݣ��������ŵȡ�
                ReadData  -> ���д16�ֽ�����
 * @return      �ɹ�����  -> MI_OK 
 **************************************************************************************************
 */
uint8_t MFRC522_ReadFromBlock(uint8_t BlockAddr, uint8_t *ReadData)
{
  uint8_t Status;
  uint32_t Bits;
  ReadData[0] = PICC_READ;
  ReadData[1] = BlockAddr;
  MFRC522_CrcCheck(ReadData, 2, &ReadData[2]);
  
  Status = MFRC522_ToCard(PCD_TRANSCEIVE, ReadData, 4, ReadData, &Bits);
  if((Status != MFRC522_OK) || (Bits != 0x90))    //18*8 = 144 
  {
     Status = MFRC522_ERR;
  }
  return Status;
}


/**************************************************************************************************
 * @fn          MFRC522_Sleep
 * @brief       ���Ƭ��������״̬
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
uint8_t MFRC522_Sleep(void)
{
  uint32_t Bits;
  uint8_t Buff[4];
  
  Buff[0] = PICC_HALT;
  Buff[1] = 0;
  
  MFRC522_CrcCheck(Buff, 2, &Buff[2]);
  
  MFRC522_ToCard(PCD_TRANSCEIVE, Buff, 4, Buff, &Bits);
  
  return MFRC522_OK;
  
}






/**************************************************************************************************
 * @fn          MFRC522_ToCard
 * @brief       RC522��ISO14443��ͨѶ
 * @param       Command       ->  MF522������
                SendData      ->  ��RC522���͵���Ƭ������,
                SendLen       ->  ���͵����ݳ���
                ReceiveData   ->  ���յ��Ŀ�Ƭ��������
                ReveiveLen    ->  �������ݵ�λ����
 * @return      ReqStatus     ->  Ѱ����� 
 **************************************************************************************************
 */
uint8_t MFRC522_ToCard(uint8_t Command, uint8_t *SendData, uint8_t SendLen, uint8_t *ReceiveData, uint32_t *ReveiveBits)
{
  uint8_t Status  = MFRC522_ERR; 
  uint8_t IrqEn   = 0x00;
  uint8_t WaitIrq = 0x00; 
  uint32_t i;
  uint8_t IRqStatus;
  uint8_t FIFOLevel  = 0x00;       //FIFOLevelReg BIT6-BIT0 FIFO�������б�����ֽ�����BIT7����ʱ����Ϊ0���뵣��
  uint8_t RxLastBits = 0x00;        //ControlReg   BIT2-BIT0 ��ʾ�����յ����ֽڵ���Чλ��Ŀ�������λΪ0�������ֽ���Ч 
    
  switch(Command)
  {
    case PCD_AUTHENT:     IrqEn   = 0x12;    //��֤����
                          WaitIrq = 0x10;
                          break;
                      
    case PCD_TRANSCEIVE:  IrqEn   = 0x77;    //����FIFO������
                          WaitIrq = 0x30;
                          break;
                        
    default:              break;
  }
  
  MFRC522_Write(CommIEnReg, IrqEn|0x80);     //�����ж�����
  MFRC522_ClearBitMask(CommIrqReg, 0x80);    //��������ж�����λ
  MFRC522_SetBitMask(FIFOLevelReg, 0x80);    //BIT7=FlushBuffer(1), FIFO��ʼ��
  MFRC522_Write(CommandReg, PCD_IDLE);       //�޶�����ȡ����ǰ������ִ��
  
  /*1.��FIFO��д������*/
  for(i=0; i<SendLen; i++)
  {
    MFRC522_Write(FIFODataReg,SendData[i]);
  }
  
  /*2.ִ�з��������FIFO�����������ݷ��͵����ߣ�������ɺ��Զ����������*/
  MFRC522_Write(CommandReg,Command);
  if((Command == PCD_TRANSCEIVE) ||(Command == PCD_AUTHENT))               
  { 
    MFRC522_SetBitMask(BitFramingReg,0x80);   //BIT7=StartSend(1):�������ݷ���
  }
  
  /*3.�ȴ����ݽ������*/
  i = 2000;                                   //i����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��15ms?ע������MFRC522_Reset�����ж�ʱ��������ʱ����15ms
  do
  {
    IRqStatus = MFRC522_Read(CommIrqReg);
    i--;
  }
  while((i!=0)&& !(IRqStatus&0x01) && !(IRqStatus&WaitIrq)); //1)i=0��ʱ���� 2)��ʱ��(15ms)����0ʱ���� 3)������ֹ���߽����������
  MFRC522_ClearBitMask(BitFramingReg,0x80);   //ֹͣ���ݷ���
  
  
  /*4.���Ѱ��״̬*/
  if(i != 0)
  {
    /*4.1.Ѱ��δ��������*/
    if(!(MFRC522_Read(ErrorReg) & 0x1B))      //δ�������´���BufferOvfl Collerr CRCErr ProtecolErr
    {
      Status = MFRC522_OK;                    //���������һ���ϵ�鿴,����δ��������
      
      if(IRqStatus & IrqEn & 0x01)            //��ʱ���ݼ�����,Ѱ����ʱ
      {
        Status = MFRC522_TIMEOUT;              
      }
      
       /*4.1.1 ���յ������ݴ���*/
      if(Command == PCD_TRANSCEIVE)                     
      {
         FIFOLevel  = MFRC522_Read(FIFOLevelReg);       //���յ����ֽ���
         RxLastBits = MFRC522_Read(ControlReg) & 0x07;  //���һ���ֽڵ���Ч��
         
         if(RxLastBits)                                 //�������0�����һ���ֽڲ����������ֽ���Ч
         {
           *ReveiveBits = (FIFOLevel-1)*8 + RxLastBits;  
         }
         else                                           //�����0�������ֽ���Ч
         {
           *ReveiveBits = FIFOLevel * 8;
         }
         if(FIFOLevel == 0)
         {
           FIFOLevel = 1;
         }
         if(FIFOLevel > FIFOLEVEL_MAX)
         {
           FIFOLevel = FIFOLEVEL_MAX;
         }
         
         /*4.1.2 ��ȡ���յ�������*/
         for(i=0; i<FIFOLevel; i++)
         {
           ReceiveData[i] = MFRC522_Read(FIFODataReg);    //��ȡFIFO������������
         }
      }
    }
    
    /*4.2.Ѱ���������󣬴�������δ֪*/
    else                                                  
    {
      Status = MFRC522_ERR;
    }
  }
  
  /*5.���*/
  MFRC522_SetBitMask(ControlReg,0x80);  //ֹͣ��ʱ��
  MFRC522_Write(CommandReg,PCD_IDLE);   //�޶�����ȡ����ǰ������ִ��
  
  return Status;
}


/* ------------------------------------------------------------------------------------------------
 *                                          Middle
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          MFRC522_ClearBitMask
 * @brief       ��RC522�Ĵ���λ
 * @param       RegAddr   -> �Ĵ�����ַ
                Mask      -> ��λֵ
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_ClearBitMask(uint8_t RegAddr,uint8_t Mask)
{
  uint8_t tmp;
  tmp = MFRC522_Read(RegAddr);
  MFRC522_Write(RegAddr, tmp&(~Mask));
}

/**************************************************************************************************
 * @fn          MFRC522_SetBitMask
 * @brief       ��RC522�Ĵ���λ
 * @param       RegAddr   -> �Ĵ�����ַ
                Mask      -> ��λֵ
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_SetBitMask(uint8_t RegAddr,uint8_t Mask)
{
  uint8_t tmp;
  tmp = MFRC522_Read(RegAddr);
  MFRC522_Write(RegAddr, tmp | Mask);
}





/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          MFRC522_Delay
 * @brief       1us���ҵ���ʱ
 * @param       n
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_Delay(uint8_t n)
{
  uint8_t i;
  for (i=0; i<n; i++)
  {
    asm("nop");
  }
}


/**************************************************************************************************
 * @fn          MFRC522_Write
 * @brief       CC2530��������
 * @param       RegAddr   -> �Ĵ�����ַ
                Data      -> �����͵�����
 * @return      ��
 **************************************************************************************************
 */
void MFRC522_Write(uint8_t RegAddr,uint8_t Data)
{
  uint8_t Addr;
  CS  = 0;  //Ƭѡ�źŵ͵�ƽ���ܴ�������
  SCK = 0;  //ʱ�ӵ͵�ƽ����
  
/*|----------------------------------------------|
  |      |       BIT7       | BIT6-BIT1 |  BIT0  |
  | MOSI |---------------------------------------|
  |      | Read(1) Write(0) |    Addr   | RFU(0) |
  |----------------------------------------------|*/
  
  Addr = ((RegAddr << 1)&0x7E);   //����1λ������ͷβ��0�� BIT7(0):д�������BIT0(0) 
  
  Spi_WriteByte(Addr);              
  Spi_WriteByte(Data);  
  
  CS  = 1;  //�ر�Ƭѡ
  SCK = 1; 

}

/**************************************************************************************************
 * @fn          MFRC522_Read
 * @brief       CC2530��ȡ����
 * @param       RegAddr   -> �Ĵ�����ַ
 * @return      ��
 **************************************************************************************************
 */
uint8_t MFRC522_Read(uint8_t RegAddr)
{
  uint8_t Data,Addr;
  
  CS  = 0; 
  SCK = 0; 
  
  Addr = ((RegAddr << 1)&0x7E)|0x80; //����1λ������ͷ��1��β��0��BIT7(1):��ȡ������BIT0(0) 
  
  Spi_WriteByte(Addr); 
  Data = Spi_ReadByte();
  
  CS  = 1;  //�ر�Ƭѡ
  SCK = 1; 
  return Data;
}