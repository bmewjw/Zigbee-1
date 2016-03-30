#define WARN_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Warn.h"

/* ------------------------------------------------------------------------------------------------
 *                                          ϵͳ��ʾ��
 * ------------------------------------------------------------------------------------------------
 */
/****************************************
 * @fn          Warn_System_Start
 * @brief       ϵͳ������ʾ��
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_System_Start(void)
{
  LED_ON();
  BUZZER_ON();
  T4CC0   = GamutF;     
  Delay_Ms(100);
  LED_OFF();
  BUZZER_OFF();
}

/* ------------------------------------------------------------------------------------------------
 *                                          ��ͨ��ʾ��
 * ------------------------------------------------------------------------------------------------
 */
/**************************************
 * @fn          Warn_One_Buzzer
 * @brief       ��ͨ��ʾ��һ��
 * @param       ��
 * @return      ��
 **************************************
 */
void Warn_One_Buzzer(void)
{
  LED_ON();
  BUZZER_ON();
  T4CC0   = GamutB;     
  Delay_Ms(100);
  LED_OFF();
  BUZZER_OFF();
  Delay_Ms(100);
}

/***************************************
 * @fn          Warn_OneBuzzer_Led
 * @brief       ��ͨ��ʾ��һ��
 * @param       ��
 * @return      ��
 ***************************************
 */
void Warn_OneBuzzer_Led(uint8_t Led)
{
  if(Led == LedOn)
  {
    LED_ON();
  }
  
  else
  {
    LED_OFF();
  }
  
  BUZZER_ON();
  T4CC0   = GamutB;     
  Delay_Ms(100);
  BUZZER_OFF();
  Delay_Ms(100);
  
}

/***************************************
 * @fn          Warn_Two_Buzzer
 * @brief       ��ͨ��ʾ������
 * @param       ��
 * @return      ��
 ***************************************
 */
void Warn_Two_Buzzer(void)
{
  Warn_One_Buzzer();
  Warn_One_Buzzer();
}

/***************************************
 * @fn          Warn_Three_Buzzer
 * @brief       ��ͨ��ʾ������
 * @param       ��
 * @return      ��
 ***************************************
 */
void Warn_Three_Buzzer(void)
{
  Warn_One_Buzzer();
  Warn_One_Buzzer();
  Warn_One_Buzzer();
}

/* ------------------------------------------------------------------------------------------------
 *                                          ��������ʾ��
 * ------------------------------------------------------------------------------------------------
 */
/****************************************
 * @fn          Warn_Door_Open
 * @brief       ������ʾ��
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Door_Open(void)
{
  
  T4CC0   = GamutC; 
  BUZZER_ON();       
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
       
  T4CC0   = GamutDp;    
  BUZZER_ON();   
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
      
  T4CC0   = GamutG;    
  BUZZER_ON();   
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
}


/****************************************
 * @fn          Warn_Door_Close
 * @brief       ������ʾ��
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Door_Close(void)
{       
  T4CC0   = GamutG;   
  BUZZER_ON();
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
  
      
  T4CC0   = GamutDp;        
  BUZZER_ON();  
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
  
  
  T4CC0   = GamutC; 
  BUZZER_ON();
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
}


/****************************************
 * @fn          Warn_Door_Lock
 * @brief       �ŷ���LED��ʾ
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Door_Lock(void)
{
  LED_ON();
  Delay_Ms(100);
  LED_OFF();
  Delay_Ms(100);
  LED_ON();
  Delay_Ms(100);
  LED_OFF();
  Delay_Ms(100);
  LED_ON();
  Delay_Ms(100);
  LED_OFF();
  Delay_Ms(100);
}

/****************************************
 * @fn          Warn_Key_Open
 * @brief       Կ�׿�����ʾ
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Key_Open(void)
{
  LED_ON();
  Warn_Door_Open();
  Delay_Ms(1000);
}


/****************************************
 * @fn          Warn_Key_Close
 * @brief       Կ�׹�����ʾ
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Key_Close(void)
{
  Warn_Door_Close();
}


/* ------------------------------------------------------------------------------------------------
 *                                          ��Ȩ״̬��ʾ��
 * ------------------------------------------------------------------------------------------------
 */
/****************************************
 * @fn          Warn_Card_Full
 * @brief       ��ͨ���б�����
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Card_Full(void)
{
  LED_OFF();
  T4CC0   = GamutB;   
  BUZZER_ON();
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
  
      
  T4CC0   = GamutC;        
  BUZZER_ON();  
  Delay_Ms(100);
  Delay_Ms(100);
  BUZZER_OFF();
  LED_ON();
}

/****************************************
 * @fn          Warn_Card_Success
 * @brief       ��ͨ���б���ȨɾȨ�ɹ�
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Card_Success(void)
{
  LED_OFF();
  BUZZER_ON();
  T4CC0   = GamutC;     
  Delay_Ms(100);
  BUZZER_OFF();
  Delay_Ms(100);
  BUZZER_ON();
  T4CC0   = GamutB;     
  Delay_Ms(100);
  BUZZER_OFF();
  LED_ON();
}

/****************************************
 * @fn          Warn_Card_Fail
 * @brief       ��ͨ���б���ȨɾȨʧ��
 * @param       ��
 * @return      ��
 ****************************************
 */
void Warn_Card_Fail(void)
{
  Warn_One_Buzzer();
  Warn_One_Buzzer();
}



/**************************************************************************************************
 * @fn          Warn_Door_Init
 * @brief       ������ʼ����ʾ
 * @param       ��
 * @return      ��
 **************************************************************************************************
 */
void Warn_Door_Init(void)
{
  T4CC0   = GamutB;   
  BUZZER_ON();
  Delay_Ms(100);
  BUZZER_OFF();
  
      
  T4CC0   = GamutG;        
  BUZZER_ON();  
  Delay_Ms(100);
  BUZZER_OFF();
  
  
  T4CC0   = GamutE; 
  BUZZER_ON();
  Delay_Ms(100);
  BUZZER_OFF();
  
  
  T4CC0   = GamutA;   
  BUZZER_ON();
  Delay_Ms(100);
  BUZZER_OFF();
  
      
  T4CC0   = GamutF;        
  BUZZER_ON();  
  Delay_Ms(100);
  BUZZER_OFF();
  
  
  T4CC0   = GamutDp; 
  BUZZER_ON();
  Delay_Ms(100);
  BUZZER_OFF();
}