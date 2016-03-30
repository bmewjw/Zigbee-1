#define DATA_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Data.h"

/***************************������Ϣ��ʼ��*************************************/

/*******************************************
 * @fn          Data_BaseID_Init
 * @brief       ��վID��Ϣ��ʼ��
 * @param       ��
 * @return      ��
 *******************************************
 */
void Data_BaseID_Init(void)
{
  uint8_t BaseId[4] = {0x00};
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR,BASE_ID1);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+1,BASE_ID2);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+2,BASE_ID3);
  AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+3,BASE_ID4);
  
  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR,BaseId,4);
  
  if((BaseId[0] != BASE_ID1) || (BaseId[1] != BASE_ID2) ||
     (BaseId[2] != BASE_ID3) || (BaseId[3] != BASE_ID4) )
  {
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR,BASE_ID1);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+1,BASE_ID2);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+2,BASE_ID3);
    AT24C256_WriteByte(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR+3,BASE_ID4);
  }
}




/***************************����ID���ݲ���*************************************/

/*******************************************
 * @fn          Data_BaseID_Read
 * @brief       ��վID��Ϣ��ȡ
 * @param       BaseId -> ��վID
 * @return      ��
 *******************************************
 */
void Data_BaseID_Read(uint8_t *BaseId)
{
  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,BASEID_BASE_ADDR,BaseId,4);
}














/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */

///*******************************************
// * @fn          Data_Storage
// * @brief       �洢�ж�
// * @param       WriteId   -> ��Ҫ�洢�Ŀ���
//                WriteAddr -> �洢��λ��
//
// * @return      Result
//                 -> AT24C256_OK     �洢�ɹ�
//                 -> AT24C256_ERR    �洢ʧ��
// *******************************************
// */
//uint8_t Data_Storage(uint8_t *WriteId, uint16_t WriteAddr)
//{
//  uint8_t ReadId[4] = {0x00};
//  uint8_t DataResult = DATA_OK;
//  
//  AT24C256_WriteBuff(AT24C256_WRITE_ADDR,WriteAddr,WriteId,4);
//    
//  AT24C256_ReadBuff(AT24C256_WRITE_ADDR,WriteAddr,ReadId,4);
//  if(strncmp((const char*)ReadId,(const char*)WriteId,4) != 0)                  //�жϴ洢���Ƿ�ɹ�
//  {
//    AT24C256_WriteBuff(AT24C256_WRITE_ADDR,WriteAddr,WriteId,4);
//    if(strncmp((const char*)ReadId,(const char*)WriteId,4) != 0)                //�ٴ��жϴ洢���Ƿ�ɹ�
//    {
//      DataResult = DATA_ERR;                                                    //�洢ʧ��
//    }
//  } 
//
//  return DataResult;
//}