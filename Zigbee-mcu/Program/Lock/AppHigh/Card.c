#define CARD_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Card.h"

/* ------------------------------------------------------------------------------------------------
 *                                          Local Variable
 * ------------------------------------------------------------------------------------------------
 */
uint8_t  SectorKeyA[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};                       //A������Կ

/* ------------------------------------------------------------------------------------------------
 *                                          Applications
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          Card_AccountInit
 * @brief       ��Ȩ����ʼ����ֻ��һ��
 * @param       ��       
 * @return      ��
 **************************************************************************************************
 */
void Card_Init(void)
{
  /*
    CARD_INFORMATION M1��62���� 16byte����
    |-------------------------------------------------------|
    |         | Byte0-1  | Byte2-5  |           |  
    | BLOCK62 |---------------------------------------------|
    |         | CardType | DoorId   |           |    
    |-------------------------------------------------------|
  
    CardType: 0x01 0x01 ��Ȩ����ʶ
              0x02 0x02 ɾȨ����ʶ
              0x03 0x03 �տ���ʶ
              0x04 0x04 �ܿ���ʶ(�����ſɿ�)
  
    DoorId  : Byte2 - DOOR_COMPANY   ������ͨ�����տ����ⶼ��Ҫ��Щ������Ϣ
              Byte3 - DOOR_BUILD     
              Byte4 - DOOR_FLOOR
              //Byte5 - DOOR_ID   
  */
  
  
  /*0.��Ȩ��,ɾȨ��,�ܿ���������....*/
  uint8_t Status                =  MFRC522_ERR;
  uint8_t AuthCard[BlockSize]   = {Authorization,Authorization,DOOR_COMPANY,DOOR_BUILD};         //��Ȩ��
  uint8_t UnAuthCard[BlockSize] = {UnAuthorizataion,UnAuthorizataion,DOOR_COMPANY,DOOR_BUILD};   //ɾȨ��
  uint8_t TotCard[BlockSize]    = {TotalCard,TotalCard,DOOR_COMPANY,DOOR_BUILD};                                        //�ܿ�
  uint8_t ReadData[BlockSize]  = {0x00};                                        //��M1����������                       

  /*1.��Ȩ��*/  
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);                           //������д����Ҫ��Card_ReadBlock��Reset����
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,AuthCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
  
  /*2.ɾȨ��*/
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,UnAuthCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
  
  
  /*3.�ܿ�*/
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,TotCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
 
}



/**************************************************************************************************
 * @fn          Card_Authorization
 * @brief       ��Ȩ������ˢ��Ȩ��֮��3S��Ѱ����Ȩ����
 * @param       CardType    ->  ����Ȩ���ͣ���Ȩ������ɾȨ��     
 * @return      Status      ->  MFRC522_OK  ��ȷ  
                                MFRC522_ERR ����
 **************************************************************************************************
 */
uint8_t Card_Authorization(uint8_t CardOperType)
{
  uint8_t Status               = MFRC522_ERR;                                                                       
  uint8_t ReadData[BlockSize]  = {0x00};                                        //��M1���������� 
  uint8_t CardCount            = 0;                                             //ˢ������
  uint8_t DoorId[4]            = {0x00};                                        //����ID
  uint8_t DataResult           = DATA_ERR;                                      //���ݴ�����
  
  Timer_Int_Enable();                                                           //����ʱ�ж�
  T1_3S_Flag = TIMESTART;                 
  
  do
  {
    /*1.��ȡ������������*/
    Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
    if(Status == MFRC522_OK)                
    {
      /*2.��Ȩ����*/
      if(CardOperType == Authorization)
      {
        if(((ReadData[0] == Authorization)    && (ReadData[1] == Authorization))    ||
           ((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion)) ||
           ((ReadData[0] == TotalCard)        && (ReadData[1] == TotalCard)))
        {
          Status = MFRC522_ERR;                                                 //�������Ȩ����ɾȨ�����ܿ��򱨴�
        }
        
        else                                                                    //��ͨ����Ȩ����                             
        {
          DataResult = Data_CommonCard_Auth(ReadData+12);                       //��ͨ���洢
          if(DataResult == DATA_OK)
          {
            Status =  MFRC522_OK; 
          }
          else if(DataResult == DATA_ERR)                                       //����Ȩ���Ѿ���Ȩ���ߴ洢ʧ��
          {
            Status = MFRC522_ERR;
          }
          else if(DataResult == DATA_FULL)
          {
            T1_3S_Flag = TIMEEND;                                               //��ͨ���б����������˳�
            Warn_Card_Full();
          }
        }
      }
 
      /*3.ɾȨ����*/
      else if(CardOperType == UnAuthorizataion)
      {
        Data_DoorID_Read(DoorId);                                               //����ID��ȡ
        if((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion) &&
           (ReadData[2] == DoorId[0])        && (ReadData[3] == DoorId[1]))     //ɾȨ,�����Ǹù�˾��¥���ɾȨ����Ч
        {
          CardCount ++; 
          T1_3S_Flag = TIMEEND;
          Warn_One_Buzzer();
          T1_3S_Flag = TIMESTART;                                                  
          LED_ON();                                                             //��ʾˢɾȨ��
        }
        
        else if(((ReadData[0] == Authorization)    && (ReadData[1] == Authorization))    ||
                ((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion)) ||
                ((ReadData[0] == TotalCard)        && (ReadData[1] == TotalCard)))
        {
          Status = MFRC522_ERR;                                                 //�������Ȩ�����ܿ�����������¥���ɾȨ��
        }
        
        else                                                                    //ɾ����ͨ��Ȩ��
        {
          CardCount = 0;        
          DataResult = Data_CommonCard_UnAuth(ReadData+12);
          
          if(DataResult == DATA_OK)
          {
            Status =  MFRC522_OK; 
          }
          else if(DataResult == DATA_ERR)
          {
            Status = MFRC522_ERR;
          }
        }
        
         
        if(CardCount >= 5)                                                      //��ͨ�����ݳ�ʼ��
        {                                                                                   
          T1_3S_Flag = TIMEEND;                                                 //����while
          Data_Door_Init();                                                     //������ʼ���������ͨ���б�������ռ�¼�Ⱥ�������
          LED_OFF();
          Warn_Door_Init();
          LED_ON();
        }
      }
      
      /*4.�����ɹ�*/
      if((Status == MFRC522_OK) && (DataResult != DATA_FULL) && (CardCount == 0))
      {    
        T1_3S_Flag = TIMEEND;
        Warn_Card_Success();                                                    //��֤3S����ֵ��0
        T1_3S_Flag = TIMESTART;                                                 //�����ظ���Ȩ 
        
//        DataResult = AT24C256_isNull();                                    //������
//        if(AT24C256_Result == AT24C256_NULL)                                    //�����ڳ�ʼ��������
//        {
//          Buzzer_CardInit();
//        }
      }
      
      /*5.����ʧ��*/
      if((Status == MFRC522_ERR) && (DataResult != DATA_FULL) && (CardCount == 0))
      {
        T1_3S_Flag = TIMEEND;
        Warn_Card_Fail();
        LED_ON();
        T1_3S_Flag = TIMESTART;                                                 //�����ظ���Ȩ 
          
//        AT24C256_Result = AT24C256_isNull();                                    //������
//        if(AT24C256_Result == AT24C256_NULL)                                    //�����ڳ�ʼ��������
//        {
//          Buzzer_CardInit();
//        }
      }
      
      /*6.�������*/
      Status = MFRC522_ERR;
      DataResult = DATA_ERR;
      memset(ReadData,0,BlockSize);
      memset(DoorId,0,4);
    }
    
    //Wdt_FeetDog();                                                              //����ι�������ﲢ���������жϳ����ܷɣ�������Ϊ��������ʱ���Ѿ�����1S
   
  }while(T1_3S_Flag == TIMESTART);                                              //3S��ʱ����
  
  
  T1_3S_Flag = TIMEINIT;    
  Timer_Int_Disable();

  return Status;
}








/* ------------------------------------------------------------------------------------------------
 *                                          Middle
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          Card_ReadID
 * @brief       ��ȡ����
 * @param       BlockAddr   ->  �����ַ
                BlockData   ->  ��������           
 * @return      Status      ->  MFRC522_OK  ��ȷ  
                                MFRC522_ERR ����
 **************************************************************************************************
 */
uint8_t Card_ReadID(uint8_t BlockAddr, uint8_t *CardId)
{
  uint8_t Status;
  uint8_t Block[BlockSize]  = {0x00};
  
  MFRC522_AntennaOn();                                                          //ʵ�������Ķ��������Ͷ�д�������뿪������
  //memcpy(WriteBlock,BlockData,6);                                             //��Ҫ��ֵ�������ͣ���Ȩ��������ͨ����
  Status = Card_Process(CardId,BlockAddr,Block,READ_ID);                        //��дģʽ����ʵ�������ô��俨��ID     
  MFRC522_AntennaOff();
  Delay_Ms(10);

  return Status;
}



/**************************************************************************************************
 * @fn          Card_WriteBlock
 * @brief       д������������
 * @param       BlockAddr   ->  �����ַ
                BlockData   ->  ��������           
 * @return      Status      ->  MFRC522_OK  ��ȷ  
                                MFRC522_ERR ����
 **************************************************************************************************
 */
uint8_t Card_WriteBlock(uint8_t BlockAddr, uint8_t *BlockData)
{
  uint8_t CardId[6]       = {0x00};
  uint8_t WriteBlock[BlockSize]  = {0x00};
  uint8_t ReadBlock[BlockSize] = {0x00};   
  uint8_t Status;
  
  /*1.д�뿨������*/
  MFRC522_Reset();                                                              //д�������ã����Ծ�Reset��   
  MFRC522_AntennaOn();                                                          //ʵ�������Ķ��������Ͷ�д�������뿪������
  memcpy(WriteBlock,BlockData,BlockSize);                                       //��ֵ���Ż�����Ϣ                 
  Status = Card_Process(CardId,BlockAddr,WriteBlock,WRITE_BLOCK);               //��дģʽ����ʵ�������ô��俨��ID
  MFRC522_AntennaOff();
  Delay_Ms(10);
  if(Status == MFRC522_ERR)                                                     //�������Ȩ�������Ӧ��ֱ�ӷ�����
  {
    return MFRC522_ERR;
  }
  
  /*2.���д��ɹ������ȡд�������*/
  MFRC522_Reset();                                                              //Ҫʵ�������Ķ�д������������Reset 
  MFRC522_AntennaOn();  
  Status = Card_ReadBlock(CARD_INFORMATION,ReadBlock);
  if(Status == MFRC522_OK)
  {
    if(strncmp((const char*)ReadBlock,(const char*)BlockData,4)!= 0)
    {
      Status = MFRC522_ERR;                                                     //д��ĺͶ�ȡ�Ĳ�һ��
    }
    
    else
    {
      Status = MFRC522_OK;                                                      //д��ĺͶ�ȡ��һ��
      return Status;
    }
  }
  
  /*3.���д���ȡ��һ�£�������д��*/
  if(Status == MFRC522_ERR)
  { 
    MFRC522_Reset();                                                            //д�������ã����Ծ�Reset��   
    MFRC522_AntennaOn();                                                        //ʵ�������Ķ��������Ͷ�д�������뿪������              
    Status = Card_Process(CardId,BlockAddr,WriteBlock,WRITE_BLOCK);             //��дģʽ����ʵ�������ô��俨��ID
    MFRC522_AntennaOff();
    Delay_Ms(10);
    if(Status == MFRC522_ERR)                                                   //�������Ȩ�������Ӧ��ֱ�ӷ�����
    {
      return MFRC522_ERR;
    }
    
    /*���¶�ȡд�������*/
    MFRC522_Reset();                                                            //Ҫʵ�������Ķ�д������������Reset 
    MFRC522_AntennaOn();  
    Status = Card_ReadBlock(CARD_INFORMATION,ReadBlock);
    if(Status == MFRC522_OK)
    {
      if(strncmp((const char*)ReadBlock,(const char*)BlockData,4)!= 0)          //д��ĺͶ�ȡ�Ĳ�һ��
      {
        Status = MFRC522_ERR;
      }
        
      else
      {
        Status = MFRC522_OK;                                                    //д��ĺͶ�ȡ��һ��
      }
    } 
  }
  
  return Status;
}


/**************************************************************************************************
 * @fn          Card_ReadBlock
 * @brief       ��������������
 * @param       BlockAddr   ->  �����ַ
                BlockData   ->  ��������           
 * @return      Status      ->  MFRC522_OK  ��ȷ  
                                MFRC522_ERR ����
 **************************************************************************************************
 */
uint8_t Card_ReadBlock(uint8_t BlockAddr, uint8_t *BlockData)
{
  uint8_t CardId[6]       = {0x00};
  uint8_t ReadBlock[BlockSize] = {0x00};
  uint8_t Status;
  
  //MFRC522_Reset();                                                              //Ҫʵ�������Ķ�д������������Reset�����������Ҫע��
  MFRC522_AntennaOn();  
  
  Status = Card_Process(CardId,BlockAddr,ReadBlock,READ_BLOCK);
  if(Status == MFRC522_ERR)
  {
    return MFRC522_ERR;
  }
  memcpy(BlockData,ReadBlock,BlockSize);                                        //���Ż�����Ϣ
  memcpy(BlockData+12,CardId,BlockSize-12);                                     //����ID
 
  MFRC522_AntennaOff();
  Delay_Ms(10);
  
  return Status;
}


/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          Card_Process
 * @brief       ��������
 * @param       CardId      ->  ��ȡ�Ŀ���ID
                BlockAddr   ->  �����ַ
                BlockData   ->  ��������
                ProcessType ->  ��������  
                                WRITE_BLOCK д��������
                                READ_BLOCK  ����������
 * @return      Status      ->  MFRC522_OK  ��ȷ  
                                MFRC522_ERR ����
 **************************************************************************************************
 */
uint8_t Card_Process(uint8_t *CardId, uint8_t BlockAddr, uint8_t *BlockData, uint8_t ProcessType)
{
  
  
  uint8_t Status;
  uint8_t CardData[BlockSize] = {0x00};
  //uint16_t CardType;

  /*1.Ѱ��,���ؿ�������*/
  Status = MFRC522_RequestCard(PICC_REQIDL,CardData);
  if(Status == MFRC522_OK)
  {
    //CardType = (CardData[0]<<8)| CardData[1];                                 //Mifare_One(S50) У԰��
  }
  
  /*2.����ײ�����ؿ�������*/
  Status = MFRC522_Anticoll(CardData);
  if(Status == MFRC522_OK)
  {
    memcpy(CardId,CardData,5);   
  }
  
  if(ProcessType == WRITE_BLOCK)
  {
    /*3.ѡ��,���ؿ�����*/ 
    MFRC522_SelectTag(CardId);                                                  //Size = 0x08
    
    /*4.д����*/
    Status = MFRC522_Auth(PICC_AUTHENT1A,BlockAddr,SectorKeyA,CardId);
    if(Status == MFRC522_OK)
    {
      Status = MFRC522_WriteToBlock(BlockAddr,BlockData);
    }
  }
 
  else if(ProcessType == READ_BLOCK)
  {
    /*3.ѡ��,���ؿ�����*/ 
    MFRC522_SelectTag(CardId);                
    
    /*4.������*/
    Status = MFRC522_Auth(PICC_AUTHENT1A,BlockAddr,SectorKeyA,CardId);
    if(Status == MFRC522_OK)
    {
      Status = MFRC522_ReadFromBlock(BlockAddr,CardData);
      if(Status == MFRC522_OK)
      {
        memcpy(BlockData,CardData,BlockSize);                                   //�������ȷ
      }
    }  
  }
  
  return Status;
}




