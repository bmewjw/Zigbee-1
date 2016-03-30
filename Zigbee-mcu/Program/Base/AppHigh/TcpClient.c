#define TCPCLIENT_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "TcpClient.h"

/*ȫ�ֱ���*/
typedef struct{
  uint8_t RxStatus;         //����λ�����������͵�����֡���������
  uint8_t RxProgress;       //��ǰһ֡���ݵĽ��ս���
  
  uint8_t CMD;              //������
  uint8_t LENG;             //֡����
  uint8_t SEQ;              //֡����
  uint8_t END_DEV;          //Ŀ���豸����
  uint8_t DEV_ID[8];        //Ŀ���豸ID
  uint8_t DATA[Rx_BUFF];    //����
  uint8_t LRC;              //У��
  
  uint8_t RESP;             //����״̬
  
  uint8_t RxCount;          //����ֵ
  
}Typedef_Frame;

Typedef_Frame Frame; 


/* ------------------------------------------------------------------------------------------------
 *                                          ����֡���մ���
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          TcpClient_Init
 * @brief       ����֡������ʼ��
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void TcpClient_Init(void)
{
  Frame.RxStatus   =  FRAME_RX_START;  
  Frame.RxProgress =  FRAME_RX_SOP_START;
  
  Frame.CMD        =  0x00;
  Frame.LENG       =  0x00;
  Frame.SEQ        =  0x00;
  Frame.END_DEV    =  END_DEV_NONE;
  memset(Frame.DEV_ID,0x00,8);
  memset(Frame.DATA,0x00,Rx_BUFF);
  Frame.LRC        =  0x00;
  
  Frame.RESP       =  0x00;
  
  Frame.RxCount    =  0x00;
}





/**************************************************************************************************
 * @fn          TcpClient_Receive
 * @brief       �ͻ��˽��ܷ��������͵�TCP�����ֽ�
 * @param       Rx_Data -> ���ڽ��յ�����
 * @return      ��
 **************************************************************************************************
 */
void TcpClient_Receive(uint8_t Rx_Data)
{
  if(Frame.RxStatus == FRAME_RX_START)                                          //���������ʱ��ȥ�����������ݵ�ʵʱ�Կ��ܻ���Ӱ��
  {
    TcpClient_Process(Rx_Data);
  }
}

/**************************************************************************************************
 * @fn          TcpClient_Process
 * @brief       ��������֡����
 * @param       Rx_Data -> ���ڽ��յ�����
 * @return      ��
 **************************************************************************************************
 */
void TcpClient_Process(uint8_t Rx_Data)
{
  /*-----------------------------------------------------------------------------------------------------*/
  /* Command    0      1        2      3       4         5       6             7            8     9      */
  /*-----------------------------------------------------------------------------------------------------*/
  /* Name      SOP    CMD     LENG    SEQ  START_DEV  END_DEV  DEV_ID   Data[0]_Data[n-1]  LRC    EOP    */
  /*-----------------------------------------------------------------------------------------------------*/
  /* Values    0xAA                                                                               0x0E   */
  /*-----------------------------------------------------------------------------------------------------*/
  /* No.Byte   2byte  1byte   1byte  1byte   1byte      1byte   8byte        n-byte       1-byte  1-byte */
  /*-----------------------------------------------------------------------------------------------------*/
  
  
  switch(Frame.RxProgress)
  {
    
    case FRAME_RX_SOP_START:          if(Rx_Data != SOP)
                                      {
                                        break;
                                      }

                                      Frame.RxProgress = FRAME_RX_SOP_CHECK;
                                      break;
                                      
    case FRAME_RX_SOP_CHECK:          if(Rx_Data == SOP)
                                      {
                                        Frame.LRC        = 0x00;
                                        Frame.RxProgress = FRAME_RX_CMD;
                                      }
                                      else
                                      {
                                        Frame.RESP = BASE_RESP_SOP_ERR;         //֡ͷ����
                                        Frame.RxProgress = FRAME_RX_CMD;        
                                      }
                                      break;
                                      
    //����֪�����͵�����                                 
     case FRAME_RX_CMD:               if(Frame.RESP == BASE_RESP_SOP_ERR)
                                      {
                                        Frame.CMD = Rx_Data;
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        //ǿ�н�����������
                                      }

                                      else if(TcpClient_IsCmdExist(Rx_Data))    //�鿴�����Ƿ����
                                      {
                                        Frame.CMD = Rx_Data;
                                        Frame.LRC = Rx_Data;
                                        Frame.RxProgress = FRAME_RX_LENG;
                                      }
                                      else
                                      {
                                        Frame.CMD = Rx_Data;              
                                        Frame.RESP = BASE_RESP_CMD_ERR;         //�������
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        
                                      }
                                      break;                                              
                                      
    case FRAME_RX_LENG:               if(Rx_Data >= 11)                         //��SEQ->Data[n-1]�����ݳ��ȣ����ڵ���12Byte
                                      {
                                        Frame.LENG = Rx_Data;
                                        Frame.LRC = Frame.LRC ^ Rx_Data;
                                        Frame.RxProgress = FRAME_RX_SEQ;
                                      }
                                      else                          
                                      {
                                        Frame.RESP = BASE_RESP_LENG_ERR;        //����ȱʧ
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        
                                      }
                                      break;
                                      
    case FRAME_RX_SEQ:                if(Rx_Data == SEq)
                                      {
                                        Frame.SEQ = Rx_Data;
                                        Frame.LRC = Frame.LRC ^ Rx_Data;
                                        Frame.RxProgress = FRAME_RX_START_DEV;
                                      }
                                      else
                                      {
                                        Frame.RESP = BASE_RESP_SEQ_ERR;         //֡���д���
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        
                                      }
                                      break;
                                                                         
    case FRAME_RX_START_DEV:          if(Rx_Data == START_DEV_PC)
                                      {
                                        Frame.LRC = Frame.LRC ^ Rx_Data;
                                        Frame.RxProgress = FRAME_RX_END_DEV;
                                      }
                                      else
                                      {
                                        Frame.RESP = BASE_START_DEV_ERR;        //��ʼ�豸ʶ�����
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        
                                      } 
                                      break;
                                      
    case FRAME_RX_END_DEV:            if(Rx_Data == END_DEV_BASE)               //����ֻ�·�����վ
                                      {
                                        Frame.END_DEV = END_DEV_BASE; 
                                      }
                                      else if(Rx_Data == END_DEV_DOOR)          //�����·�������
                                      {
                                        Frame.END_DEV = END_DEV_DOOR;
                                      }
                                      else
                                      {
                                        Frame.RESP = BASE_END_DEV_ERR;          //Ŀ���豸ʶ�����
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        
                                        break;
                                      }
                                      
                                      Frame.LRC = Frame.LRC ^ Rx_Data;
                                      Frame.RxCount    = 0;
                                      Frame.RxProgress = FRAME_RX_DEV_ID; 
                                      break;
                                      
    case FRAME_RX_DEV_ID:             Frame.DEV_ID[Frame.RxCount++] = Rx_Data;
                                      Frame.LRC = Frame.LRC ^ Rx_Data;
                                      
                                      if(Frame.RxCount ==  4)                   
                                      {
                                         if(Frame.DEV_ID[3] != BASE_ID4)        //���BaseId����ȷ��
                                         {
                                            Frame.RxCount = 0;  
                                            Frame.RESP = BASE_END_DEV_ERR;      //��Ȼ��Ŀ���豸ʶ�����
                                            Frame.RxStatus = FRAME_RX_ERR;
                                            Frame.RxProgress = FRAME_RX_END;    
                                            break;
                                         }
                                      }
      
                                      else if(Frame.RxCount == 8)
                                      {
                                        Frame.RxCount = 0;                      //���ﻹȱʶ������ID�ļ��
                                        Frame.RxProgress = FRAME_RX_DATA;
                                      }
                                      break;     
                                      
                                      
    case FRAME_RX_DATA:               if(Frame.LENG == 11)                      //û�����ݣ�ֻ��CMD
                                      {
                                        Frame.RxProgress = FRAME_RX_LRC;
                                      }
                                       
                                      else
                                      { 
                                        
                                      }
                                      
                                      
    case FRAME_RX_LRC:                if(Frame.LRC == Rx_Data)
                                      {
                                        Frame.RxProgress = FRAME_RX_EOP;
                                      }       
                                      else
                                      {
                                        Frame.RESP = BASE_RESP_LRC_ERR;
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        //ǿ�н�����������
                                      }
                                      break;
                                      
    case FRAME_RX_EOP:                if(Rx_Data == EOP)
                                      {
                                        Frame.RxStatus = FRAME_RX_OK;           //һ֡������ȷ
                                      }
                                      else
                                      {
                                        Frame.RESP = BASE_RESP_EOP_ERR;
                                        Frame.RxStatus = FRAME_RX_ERR;
                                        Frame.RxProgress = FRAME_RX_END;        //ǿ�н�����������
                                      }     
                                      break;
 
                                      
    default:                          break;       
    
  }
  
  
  
  
  
}


/**************************************************************************************************
 * @fn          TcpClient_IsCmdExist
 * @brief       ƥ�������Ƿ����
 * @param       RxCmd -> ���յ�����
 * @return      true  -> ����
                false -> ������
 **************************************************************************************************
 */

bool TcpClient_IsCmdExist(uint8_t RxCmd)
{
  bool flag = false;
  
  switch(RxCmd){
    case BASE_CMD_READID:  flag = true;
                           break;
    
    
    default:               break;
  }
  
  return flag;
}







/* ------------------------------------------------------------------------------------------------
 *                                          main������ѯ����֡
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          TcpClient_IsFullFrame
 * @brief       �ж��Ƿ�����������֡
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void TcpClient_IsFullFrame(void)
{
  /*1.��ȷ������֡*/
  if(Frame.RxStatus == FRAME_RX_OK)
  { 
    if(Frame.END_DEV == END_DEV_BASE)                                         //�������͵���վ
    {
      TcpClient_BaseCMD_Process();
    }
    
    else if(Frame.END_DEV == END_DEV_DOOR)                                    //�������͵�����
    {
      
    }    
    else
    {
      
    }
    
    TcpClient_Init();                                                           //�������½�������֡
  }
  
  /*2.���������֡*/
  else if(Frame.RxStatus == FRAME_RX_ERR)
  {
    TcpClient_ErrFrame_Process();
    TcpClient_Init();
  }
  
  /*3.����֡��������в��ֶ�ʧ����һ֡���ݿ��ܻᲹȫswitch����������֡�Ѿ�������*/
  else 
  {
    
  }
}


/* ------------------------------------------------------------------------------------------------
 *                                          ��վ���������[����֡���մ���]
 * ------------------------------------------------------------------------------------------------
 */
void TcpClient_ErrFrame_Process(void)
{
  
  uint8_t Data = 0X00;
  uint8_t frame[18] = {0x00};
                                     
  TcpClient_createFrame(frame,SEq,Frame.CMD,Frame.RESP,&Data,0);
  Uart0_SendString(frame,18);                                                   //��ִ�����PC
}










/* ------------------------------------------------------------------------------------------------
 *                                          ��վ���������[����֡������ȷ]
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          TcpClient_BaseCMD_Process
 * @brief       PC���͸���վ�������
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */

void TcpClient_BaseCMD_Process(void)
{
  switch(Frame.CMD)
  {
    case BASE_CMD_READID:      TcpClient_BaseCMD_ReadID();  
                               break;
    
    
    
    default:                   break;

  }
  
}



/**************************************************************************************************
 * @fn          TcpClient_BaseCMD_ReadID
 * @brief       ��վID�鿴
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void TcpClient_BaseCMD_ReadID(void)
{
  uint8_t BaseId[4] = {BASE_ID1,BASE_ID2,BASE_ID3,BASE_ID4};
  uint8_t frame[22] = {0x00};
  
  //Data_BaseID_Read(BaseId);                                                     //��ȡ��վID
  TcpClient_createFrame(frame,Frame.SEQ,Frame.CMD,BASE_RESP_OK,BaseId,4);
  Uart0_SendString(frame,22);                                                   //��ִ�����PC
}















/* ------------------------------------------------------------------------------------------------
 *                                          ���͸�PC������֡��������
 * ------------------------------------------------------------------------------------------------
 */

void TcpClient_createFrame(uint8_t* frame,uint8_t SEQ, uint8_t FPB_CMD, uint8_t RESP, uint8_t *data,uint8_t dataLength)
{
  /*------------------------------------------------------------------------------------------------------------*/
  /* Command    0       1        2       3       4        5       6       7          8            8     9       */
  /*------------------------------------------------------------------------------------------------------------*/
  /* Name      SOP    FPB_CMD   LENG    SEQ  START_DEV  END_DEV  DEV_ID  RESP   Data[0]_Data[n-1]  LRC    EOP   */
  /*------------------------------------------------------------------------------------------------------------*/
  /* Values    0xAA                                                                                       0x0E  */
  /*------------------------------------------------------------------------------------------------------------*/
  /* No.Byte   2byte   1byte    1byte   1byte   1byte     1byte   8byte  1byte     n-byte        1-byte  1-byte */
  /*------------------------------------------------------------------------------------------------------------*/
  uint8_t i;
  uint8_t LRC;
  uint8_t BaseId[4] = {BASE_ID1,BASE_ID2,BASE_ID3,BASE_ID4};
  uint8_t TxCount = 0;
 
  
  frame[TxCount++] = SOP;
  frame[TxCount++] = SOP;
  
  frame[TxCount++] = FPB_CMD;
  
  frame[TxCount++] = dataLength + 12;
  frame[TxCount++] = SEQ;
  frame[TxCount++] = START_DEV_BASE;                                            //��ʼ�豸Ϊ��վ���ǻ�վ������PC
  frame[TxCount++] = END_DEV_PC;
  
  
  //Data_BaseID_Read(BaseId);
  for(i=0; i<4; i++)
  {
    frame[TxCount++] = BaseId[i];
  }
  
  for(i=0; i<4; i++)                                                            //����ǻ�վ����
  {
    frame[TxCount++] = 0x00;
  }
  
  frame[TxCount++] = RESP;
  
  if(dataLength != 0x00)
  {
    for(i=0; i<dataLength;i++)
    {
      frame[TxCount++] = data[i];
    }  
  }
  
  LRC = frame[2];
  
  for(i=3; i<TxCount; i++)
  {
    LRC = LRC ^ frame[i];
  }
  
  frame[TxCount++] = LRC;
  
  frame[TxCount++] = EOP;
  
}