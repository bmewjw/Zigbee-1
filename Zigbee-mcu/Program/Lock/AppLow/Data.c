#define DATA_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Data.h"

/***************************������Ϣ��ʼ��*************************************/

/*******************************************
 * @fn          Data_Door_Init
 * @brief       ������ʼ��
 * @param       ��
 * @return      DataResult -> DATA_ERR  ��ʼ��ʧ��
                DataResult -> DATA_OK   ��ʼ���ɹ�
 *******************************************
 */
uint8_t Data_Door_Init(void)
{
  uint8_t DataResult = DATA_OK;                                                 //���ݲ�����ʶ
  
  DataResult = Data_CommonCard_Init();                                          //��ͨ���б��ʼ��
  
  
  return DataResult;
}





/*******************************************
 * @fn          Data_CommonCard_Init
 * @brief       ��ͨ���б��ʼ��
 * @param       ��
 * @return      DataResult -> DATA_ERR  ��ʼ��ʧ��
                           -> DATA_OK   ��ʼ���ɹ�
 *******************************************
 */
uint8_t Data_CommonCard_Init(void)
{
  uint8_t NullCardRom[4] = {'=','=','=','='};                                   //��ͨ�����ڴ��ʶ
  uint8_t ReadData[4]    = {0x00};                                            
  uint8_t DataResult = DATA_OK;                                                 //���ݲ�����ʶ
  uint16_t i;
  
  /*1.��ͨ���ڴ����*/
  for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4)                                        
  {
    AT24C256_WriteBuff(AT24C256_WRITE_ADDR,i,NullCardRom,4);
  }
  
  for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4)                     //��ͨ���ڴ������֤
  {
    AT24C256_ReadBuff(AT24C256_WRITE_ADDR,i,ReadData,4);
    if(strncmp((const char*)ReadData,(const char*)NullCardRom,4) != 0)          //������ݲ����
    {
      DataResult = DATA_ERR;
      break;
    }
  }
  
  
  /*2.���д���ȡ��һ�£��ٴ����*/
  if(DataResult == DATA_ERR)
  {
    for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4)                                     
    {
      AT24C256_WriteBuff(AT24C256_WRITE_ADDR,i,NullCardRom,4);
    }
    
    for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4)                   //��ͨ���ڴ������֤
    {
      AT24C256_ReadBuff(AT24C256_WRITE_ADDR,i,ReadData,4);
      if(strncmp((const char*)ReadData,(const char*)NullCardRom,4) != 0)        //������ݲ����
      {
        DataResult = DATA_ERR;
        break;
      }
    }
  }
  
  return DataResult;
  
}

/*******************************************
 * @fn          Data_DoorID_Init
 * @brief       ����ID��Ϣ��ʼ��
 * @param       ��
 * @return      ��
 *******************************************
 */
void Data_DoorID_Init(void)
{
  uint8_t DoorId[4] = {0x00};
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR,DOOR_COMPANY);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+1,DOOR_BUILD);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+2,DOOR_FLOOR);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+3,DOOR_ID);
  
  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR,DoorId,4);
  
  if((DoorId[0] != DOOR_COMPANY) || (DoorId[1] != DOOR_BUILD) ||
     (DoorId[2] != DOOR_FLOOR)   || (DoorId[3] != DOOR_ID)     )
  {
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR,DOOR_COMPANY);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+1,DOOR_BUILD);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+2,DOOR_FLOOR);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR+3,DOOR_ID);
  }
}




/***************************����ID���ݲ���*************************************/

/*******************************************
 * @fn          Data_DoorID_Read
 * @brief       ����ID��Ϣ��ȡ
 * @param       DoorId -> ����ID
 * @return      ��
 *******************************************
 */
void Data_DoorID_Read(uint8_t *DoorId)
{
  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,DOORID_BASE_ADDR,DoorId,4);
}


/***************************��ͨ�����ݲ���*************************************/

/*******************************************
 * @fn          Data_CommonCard_Auth
 * @brief       ��ͨ����Ȩ�洢
 * @param       CardId -> ��Ҫ��Ȩ�Ŀ���
 * @return      Result
                 -> DATA_OK      ��Ȩ�ɹ�
                 -> DATA_FULL    �������� 100��
                 -> DATA_ERR     ��������Ȩ���ߴ洢ʧ��
 *******************************************
 */
uint8_t Data_CommonCard_Auth(uint8_t *CardId)
{
  uint8_t DataResult    = DATA_OK;
  uint8_t ReadData[4]   = {0x00};
  uint8_t NullRom[4]    = {'=','=','=','='};                                    //���ڴ��ʶ
  uint8_t UsedRom[4]    = {'-','-','-','-'};                                    //ʹ�ù����ڴ��ʶ
  uint16_t NullPosition = COMMONCARD_MAX1_ADDR;                                 //���ڴ�λ��
  uint16_t UsedPosition = COMMONCARD_MAX1_ADDR;                                 //ʹ�ù��ڴ�
  uint16_t i;
  
  for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4)                                                
  {
    AT24C256_ReadBuff(AT24C256_WRITE_ADDR,i,ReadData,4);
    if(strncmp((const char*)CardId,(const char*)ReadData,4)== 0)                //�ж��Ƿ��Ѿ���Ȩ���ÿ�
    {
      DataResult = DATA_ERR;
      break;
    }
    
    else if((NullPosition == COMMONCARD_MAX1_ADDR) || (UsedPosition == COMMONCARD_MAX1_ADDR))
    {
      if(NullPosition == COMMONCARD_MAX1_ADDR)
      {
        if(strncmp((const char*)ReadData,(const char*)NullRom,4)== 0)           //Ѱ�ҿ��ڴ�
        {
          NullPosition = i;                                                     //(RomPosition + 1) % 4  == 0
        }
      }
      
      if(UsedPosition == COMMONCARD_MAX1_ADDR)
      {
        if(strncmp((const char*)ReadData,(const char*)UsedRom,4)== 0)           //Ѱ��ʹ�ù����ڴ�
        {
          UsedPosition = i;                                                     //(UsedPosition + 1) % 4  == 0
        }
      }
    }
  }
  
  if(DataResult == DATA_OK)
  {
    if((NullPosition != COMMONCARD_MAX1_ADDR) && ((NullPosition % 4) == 0))
    {
      DataResult = Data_Storage(CardId,NullPosition);                           //��ͨ���洢 
    }
    
    else if((UsedPosition != COMMONCARD_MAX1_ADDR) && ((UsedPosition % 4) == 0))
    {
      DataResult = Data_Storage(CardId,UsedPosition);                           //��ͨ���洢 
    }
    
    else if((NullPosition == COMMONCARD_MAX1_ADDR) && (UsedPosition == COMMONCARD_MAX1_ADDR))
    {
      DataResult = DATA_FULL;                                                   //�洢��������
    }
  }
  
  return DataResult;
}

/*******************************************
 * @fn          Data_CommonCard_UnAuth
 * @brief       ��ͨ��ɾȨ�洢
 * @param       CardId -> ��ҪɾȨ�Ŀ���
 * @return      Result
                 -> DATA_OK     ɾ��ͨ���ɹ�
                 -> DATA_ERR    ɾ��ͨ��ʧ�ܣ�û�����ſ�,���ߴ洢����ʧ��
 *******************************************
 */
uint8_t Data_CommonCard_UnAuth(uint8_t *CardId)
{
  uint8_t DataResult  = DATA_ERR;
  uint8_t ReadData[4] = {0x00};
  uint8_t UsedRom[4]  = {'-','-','-','-'};                                      //ʹ�ù����ڴ��ʶ
  uint16_t i;
  
  for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4) 
  {
    AT24C256_ReadBuff(AT24C256_WRITE_ADDR,i,ReadData,4);
    if(strncmp((const char*)CardId,(const char*)ReadData,4) == 0)               //�ж��Ƿ�����Ȩ���ÿ�
    {
      DataResult = Data_Storage(UsedRom,i);
      break;
    }
  }
  
  return DataResult;
}


/*******************************************
 * @fn          Data_CommonCard_Confirm
 * @brief       ��ͨ��ȷ��
 * @param       CardId -> ��Ҫȷ�ϵĿ���
 * @return      Result
                 -> DATA_OK     �иÿ�
                 -> DATA_ERR    �޸ÿ�
 *******************************************
 */
uint8_t Data_CommonCard_Confirm(uint8_t *CardId)
{
  uint8_t DataResult = DATA_ERR;
  uint8_t ReadData[4] = {0x00};
  uint16_t i;
  
  for(i=COMMONCARD_BASE_ADDR; i<COMMONCARD_MAX1_ADDR; i+=4) 
  {
    AT24C256_ReadBuff(AT24C256_WRITE_ADDR,i,ReadData,4);
    if(strncmp((const char*)CardId,(const char*)ReadData,4)== 0)                //�ж��Ƿ�����Ȩ���ÿ�
    {
      DataResult = DATA_OK;
      break;
    }
  }
  
  return DataResult;
}
















/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */

/*******************************************
 * @fn          Data_Storage
 * @brief       �洢�ж�
 * @param       WriteId   -> ��Ҫ�洢�Ŀ���
                WriteAddr -> �洢��λ��

 * @return      Result
                 -> AT24C256_OK     �洢�ɹ�
                 -> AT24C256_ERR    �洢ʧ��
 *******************************************
 */
uint8_t Data_Storage(uint8_t *WriteId, uint16_t WriteAddr)
{
  uint8_t ReadId[4] = {0x00};
  uint8_t DataResult = DATA_OK;
  
  AT24C256_WriteBuff(AT24C256_WRITE_ADDR,WriteAddr,WriteId,4);
    
  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,WriteAddr,ReadId,4);
  if(strncmp((const char*)ReadId,(const char*)WriteId,4) != 0)                  //�жϴ洢���Ƿ�ɹ�
  {
    AT24C256_WriteBuff(AT24C256_WRITE_ADDR,WriteAddr,WriteId,4);
    if(strncmp((const char*)ReadId,(const char*)WriteId,4) != 0)                //�ٴ��жϴ洢���Ƿ�ɹ�
    {
      DataResult = DATA_ERR;                                                    //�洢ʧ��
    }
  } 

  return DataResult;
}