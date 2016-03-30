#define PCF8563_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "PCF8563.h"

/* ------------------------------------------------------------------------------------------------
 *                                          Applications
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          PCF8563_AlarmInit
 * @brief       RTCʱ��оƬ�����趨��INT�͵�ƽ��Ч,�ж����˾��Զ�����ϵͳ��˯��״̬
 * @param       ��    
 * @return      ��
 **************************************************************************************************
 */
void PCF8563_Init(void) 
{
//  //uint8_t i;
//  uint8_t VL;                                                     //ֻ������������ʱ���ʱ����ж���û�е��磬ʱ��׼��׼�������0�Ļ�ֻҪ�������ʱ��ͰѷּĴ�����VLλ�����
  PCF8563_Time Time = {0x16,0x01,0x12,0x02,0x14,0x36,0x00};       //Ĭ��ʱ�䣬ע��������0-6 0��������
//  PCF8563_ReceiveByte(PCF8563_WRITE_ADDR,PCF8563_SEC_ADDR,&VL);   //��ȡVL��ֵ��VL�������־λ���͵�ѹ�����ʱ����1
//
  PCF8563_SetTime(Time);
//  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_CON1_ADDR,      0x00); //Ĭ��ֵ
//  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_CON2_ADDR,      0x11); //INT������Ч����ʱ���ж���Ч,�����ж���Ч
//  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_TIME_ADDR,      0x81); //��ʱ����Ч����ʱ��ʱ��Ƶ��64Hz 
//  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_TIMECOUNT_ADDR, 0x40); //0x40/64 = 1s, ���Ź���1s��������ʵҪС��1s,ע��0x00�ǹرն�ʱ����
 
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_MIN_ALARM_ADDR, 0x80); //AE = 1����Ӧ�ı���������Ч
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_HOUR_ALARM_ADDR,0x80);  
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_DAY_ALARM_ADDR, 0x80); 
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_WEEK_ALARM_ADDR,0x80);    
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_CLKOUT_ADDR,    0x00); //CLKOUT�������ֹ����ɸ���̬
}




/**************************************************************************************************
 * @fn          PCF8563_SetTime
 * @brief       RTCʱ��оƬʱ���趨
 * @param       Time -> ��Ҫ���õ�ʱ��      
 * @return      ��
 **************************************************************************************************
 */
uint8_t PCF8563_SetTime(PCF8563_Time Time)
{
  uint8_t SetFlag = PCF8563_FAIL;
  PCF8563_Time ReadTime;
  
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_SEC_ADDR,   Time.Sec);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_MIN_ADDR,   Time.Min);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_HOUR_ADDR,  Time.Hour);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_DAY_ADDR,   Time.Day);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_WEEK_ADDR,  Time.Week);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_MONTH_ADDR, Time.Month);
  PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_YEAR_ADDR,  Time.Year);
  
  ReadTime = PCF8563_ReadTime();
  
  if((ReadTime.Min == Time.Min)   && (ReadTime.Hour == Time.Hour)   && (ReadTime.Day == Time.Day) &&
     (ReadTime.Week == Time.Week) && (ReadTime.Month == Time.Month) && (ReadTime.Year == Time.Year))
  {
    SetFlag = PCF8563_SUCCESS;
    return SetFlag;
  }
  
  else
  {
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_SEC_ADDR,   Time.Sec);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_MIN_ADDR,   Time.Min);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_HOUR_ADDR,  Time.Hour);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_DAY_ADDR,   Time.Day);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_WEEK_ADDR,  Time.Week);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_MONTH_ADDR, Time.Month);
    PCF8563_SendByte(PCF8563_WRITE_ADDR,PCF8563_YEAR_ADDR,  Time.Year);
    
    ReadTime = PCF8563_ReadTime();
  
    if((ReadTime.Min == Time.Min)   && (ReadTime.Hour == Time.Hour)   && (ReadTime.Day == Time.Day) &&
       (ReadTime.Week == Time.Week) && (ReadTime.Month == Time.Month) && (ReadTime.Year == Time.Year))
    {
      SetFlag = PCF8563_SUCCESS;
    }
    
    else
    {
      SetFlag = PCF8563_FAIL;
    }
    
  }
  
  return SetFlag;
  
}



/**************************************************************************************************
 * @fn          PCF8563_ReadTime
 * @brief       RTCʱ��оƬʱ���趨
 * @param       ��      
 * @return      PCF8563_Time -> ��ȡ��ʱ��
 **************************************************************************************************
 */

PCF8563_Time PCF8563_ReadTime(void)
{
  uint8_t time[7] = {0};
  PCF8563_Time Time;
  PCF8563_ReceiveBuff(PCF8563_WRITE_ADDR,PCF8563_SEC_ADDR,time,7);
  
  Time.Sec    = time[0];
  Time.Min    = time[1];
  Time.Hour   = time[2]; 
  Time.Day    = time[3]; 
  Time.Week   = time[4];
  Time.Month  = time[5]; 
  Time.Year   = time[6]; 
  return Time;
}


/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          PCF8563_SendByte
 * @brief       RTCʱ��оƬ��������
 * @param       SlaveAddr -> �ӻ���ַ
                RegAddr   -> �Ĵ�����ַ
                Data      -> �����͵�����
 * @return      AckFlag   -> 0 ��Ӧ�� 1 ��Ӧ�� 
 **************************************************************************************************
 */
uint8_t PCF8563_SendByte(uint8_t SlaveAddr,uint8_t RegAddr,uint8_t Data)
{
  uint8_t AckFlag;
  
  I2C_Start();                          //��������
  AckFlag = I2C_SendByte(SlaveAddr);    //���ʹӻ���ַ��������ַ��
  if(AckFlag == 1)                      //��û��Ӧ��
  {
    return 0;
  }
  
  AckFlag = I2C_SendByte(RegAddr);      //���ʹӻ��ļĴ�����ַ
  if(AckFlag == 1)                      //��û��Ӧ��
  {
    return 0;
  }
  
  AckFlag = I2C_SendByte(Data);         //��������
  if(AckFlag == 1)                      //��û��Ӧ��
  {
    return 0;
  }
  
  I2C_Stop();                           //��������
  return 1;
}



/**************************************************************************************************
 * @fn          PCF8563_ReceiveByte
 * @brief       RTCʱ��оƬ��������
 * @param       SlaveAddr -> �ӻ���ַ
                RegAddr   -> �Ĵ�����ַ
                ch        -> �����յ�����
 * @return      AckFlag   -> 0 ��Ӧ�� 1 ��Ӧ�� 
 **************************************************************************************************
 */
uint8_t PCF8563_ReceiveByte(uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *ch)
{
  uint8_t AckFlag;
  I2C_Start();       

  AckFlag = I2C_SendByte(SlaveAddr);      //���ʹӻ���ַ��������ַ��
  if(AckFlag == 1)                        //��û��Ӧ��
  {
    return 0;
  }
  
  AckFlag = I2C_SendByte(RegAddr);        //���ʹӻ��ļĴ�����ַ
  if(AckFlag == 1)                        //��û��Ӧ��
  {
    return 0;
  }
  
  I2C_Start();     
  AckFlag = I2C_SendByte(SlaveAddr + 1);  //�����Ƕ����ݣ�ע��Ĭ�����λ��0������д���ݣ�+1���1
  if(AckFlag == 1)                        
  {
    return 0;
  }
  
  *ch = I2C_ReceiveByte();                //��ȡ����
  I2C_SendAck(1);                         //���һ���ֽ� No Acknowledge ������ҪʹSDA = 1 
                           
  I2C_Stop();                             //��������
  return 1;
}



/**************************************************************************************************
 * @fn          PCF8563_ReceiveBuff
 * @brief       RTCʱ��оƬ��������
 * @param       SlaveAddr -> �ӻ���ַ
                RegAddr   -> �Ĵ�����ַ
                Buff      -> �����յ�����
                Len       -> ��ȡ�ֽ���
 * @return      AckFlag   -> 0 ��Ӧ�� 1 ��Ӧ�� 
 **************************************************************************************************
 */
uint8_t PCF8563_ReceiveBuff(uint8_t SlaveAddr,uint8_t RegAddr,uint8_t *Buff,uint8_t Len)
{
  uint8_t i,AckFlag;
  
  I2C_Start();       
  
  AckFlag = I2C_SendByte(SlaveAddr);      //���ʹӻ���ַ��������ַ��
  if(AckFlag == 1)                        //��û��Ӧ��
  {
    return 0;
  }
  
  AckFlag = I2C_SendByte(RegAddr);        //���ʹӻ��ļĴ�����ַ
  if(AckFlag == 1)                        //��û��Ӧ��
  {
    return 0;
  }
  
  I2C_Start();     
  AckFlag = I2C_SendByte(SlaveAddr + 1);  //�����Ƕ����ݣ�ע��Ĭ�����λ��0������д���ݣ�+1���1
  if(AckFlag == 1)                        //��û��Ӧ��
  {
    return 0;
  }
  
  for(i=0; i<Len-1; i++)
  {
    Buff[i] = I2C_ReceiveByte();
    I2C_SendAck(0);  
  }
  
  
  Buff[Len-1] = I2C_ReceiveByte();
  I2C_SendAck(1);
  
  I2C_Stop();                             //��������
  return 1;
}









