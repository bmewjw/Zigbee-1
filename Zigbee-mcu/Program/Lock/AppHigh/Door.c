#define DOOR_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Door.h"

/**************************************************************************************************
 * @fn          Door_Open
 * @brief       ����(����)
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void Door_Open(uint8_t Led)
{
  Warn_OneBuzzer_Led(Led);                             
  Motor_H1ON();                                                                 //��ֹ���ѿ����ɶ�һ��
  Motor_H1ON();
  Delay_Ms(10);       
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);
  Motor_H1ON();
  Motor_H1ON();
  Delay_Ms(10);       
  Delay_Ms(10);
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);
  Delay_Ms(10);
  Motor_H1ON();
  Motor_H1ON();
  Delay_Ms(10);       
  Delay_Ms(10);
  Delay_Ms(10);       
  Delay_Ms(10);
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);
  Delay_Ms(10);
  Delay_Ms(10);       
  Delay_Ms(10);
  Motor_H1H2OFF();
  Delay_Ms(100);      
  Motor_H1ON();
  Motor_H1ON();
  Delay_Ms(100);      
  Delay_Ms(100);
  Motor_H1H2OFF();
  Motor_H1H2OFF();
  Warn_Door_Open();   
}

/**************************************************************************************************
 * @fn          Door_Close
 * @brief       ����
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void Door_Close(uint8_t Led)
{
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);   
  Motor_H1ON();       
  Motor_H1ON();
  Delay_Ms(10);
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);  
  Delay_Ms(10);
  Motor_H1ON();       
  Motor_H1ON();
  Delay_Ms(10);
  Delay_Ms(10);
  Motor_H1ON();
  Motor_H1ON();
  Delay_Ms(10);
  Delay_Ms(10);
  Delay_Ms(10);       
  Delay_Ms(10);
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(10);
  Delay_Ms(10);
  Delay_Ms(10);       
  Delay_Ms(10);
  Motor_H1H2OFF();
  Delay_Ms(100);
  Motor_H2ON();
  Motor_H2ON();
  Delay_Ms(100);  
  Delay_Ms(100);
  Motor_H1H2OFF();
  Motor_H1H2OFF();
  Warn_Door_Close(); 
  
  if(Led == LedOff)
  {
    LED_OFF();
  }
  
  else
  {
    LED_ON();
  }
  
}




/**************************************************************************************************
 * @fn          Door_OpenClose
 * @brief       ���Ų�����
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */

void Door_Open_Close(void)
{  
  uint8_t count = 0;
  Door_Open(LedOn);
  
  Door_Int_Enable();            //P0.4�ж�ʹ��
  IsDoorClose = CLOSE_NO;       //�����жϼ����״̬ 
  
  Timer_Int_Enable();           //����ʱ�ж�
  T1_3S_Flag = TIMESTART;
  
  do
  {
    if(IsDoorClose == CLOSE_YES)
    {
      Delay_Ms(10);             //��������
      if(DOOR == PUSHOUT)       //��ɸߵ�ƽ��״̬�������ٴ��ж�
      {
        IsDoorClose = CLOSE_NO;
        count ++;
      }
    }
    
    if(count >= 1)              //��������������Ҳ���ܵ��Ź��ϵ���Ź���
    {
      count = 0;
      T1_3S_Flag = TIMEEND;     //�������ţ�����3S����       
      Delay_Ms(10);             //Ϊ����T1_3S_Count�������������ʱ
    }
    
    
    //Wdt_FeetDog();
    
  }while(T1_3S_Flag == TIMESTART);
  
  T1_3S_Flag = TIMEINIT;        //ע�����û�г�ʱ��������ôҪ�ǵ��������ֵcount
  Timer_Int_Disable();          //�رն�ʱ�ж�
  
  Door_Close(LedOn);
  
  KeyDoor_Int_Disable();        //����P0.1��P0.4�жϽ�ֹ
  IsDoorClose = CLOSE_INIT;     //��ֹ�жϼ����״̬����ֹ���濪��ʱ���°��������ж�
  
}