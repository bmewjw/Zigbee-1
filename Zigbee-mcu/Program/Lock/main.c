#define MAIN_C_
#include "main.h"

/* ------------------------------------------------------------------------------------------------
 *                                          ������
 * ------------------------------------------------------------------------------------------------
 */


//1.��Ҫע�����һ��ʱ��ֻ����һ���жϹ�����������RF���ͺͽ���ʱ�������ܷɳ���
//2.��ʱ��Ӧ������������ʱ
//3.ͬһ��ʱ��Ҫ��ֹ����һ���ͣ��ҷ���ʱһ��Ҫ�ر����е��ж�����ֹ���ա�
//4.����ͬʱˢ����Կ�׿��ţ��Ḵλ����


void main(void)
{
    /*1.�ֲ�������ʼ��*/
    uint8_t RunMode         = ReadCardAction;                                   //��ǰ����ģʽ
    uint8_t Status          = MFRC522_ERR;                                      //����������־
    uint8_t DoorId[4]       = {0x00};                                           //����ID
    uint8_t BlockData[16]   = {0x00};                                           //M1���������� 
    uint16_t AdcValue       = 0x00;                                             //P0_0��ADֵ
    uint8_t Adc[2]          = {0x00};                                           //���͸���Ƶģ���ADֵ�ߵͰ�λ
    uint16_t k1,k2;

    /*2.ȫ�ֱ�����ʼ��*/
    IsDoorClose           = CLOSE_INIT;                                         //��״̬δ֪��ƽ���ǲ����ģ���ʱ��Ҳ���������ж�
    IsKeyOpen             = OPEN_NO;
    
    
    /*3.���жϹرա�ϵͳʱ�ӳ�ʼ��*/
    EA = 0;                                                                     //�ص����ж�
    Sysclk_Init();                                                              //ϵͳʱ��Ƶ������Ϊ32Mhz
    
    /*4.CC2530�˿ڳ�ʼ��*/
    CC2530_Init();
    
    /*5.Driver��ʼ��*/
    Spi_Init();                                                                 //SPI��ʼ��
    Timer1_Init();                                                              //��ʱ��1��ʼ��
    Timer3_Init();                                                              //��ʱ��3��ʼ��
    Timer4_Init();                                                              //������PWM��ʼ��
    Delay_Init();                                                               //��ʱ���жϼ�ʱ������ʼ��
    Key_Int_Init();                                                             //��е�����жϳ�ʼ��
    Rf_Init();                                                                  //��Ƶģ���ʼ��,���շ��ͽ�ֹ
    Sleep_Init();                                                               //ϵͳ��ʱ˯�߳�ʼ��
    //Wdt_Init();                                                                 //���Ź���ʼ��,���Ե�ʱ����ܵ�Ӱ��
    
    /*6.Device��ʼ��*/     
    MFRC522_Init();                                                             //MFRC522����ģ���ʼ��
    
    /*7.App��ʼ��*/
   
    
    /*8.�������ж�,�رյ����ж�*/
    EA = 1;                                                                     //�������ж�
    CC2530_Int(AllDisable);                                                     //�رյ��������ж�,����˯���ж�

    
    /*9.ֻ��ʼ��1�Σ����հ汾��Ҫ����*/
    PCF8563_Init();                                                             //RTCʱ��оƬ��ʼ��,�������һ��ʱ���趨
    //Card_Init();                                                              //��Ȩ������,���Գ����ã����ջ�дһ������������
    //Data_CommonCard_Init();
    Data_DoorID_Init();                                                         //����ID�洢
    
    
    /*10.��������*/
    Warn_System_Start();                                                         //ϵͳ��������
    Door_Close(LedOff);
    
    while(1)
    {     
      switch(RunMode)
      {
        
        /*0.��ص�ѹ���*/
        case BatteryDetection:  CC2530_Int(KeyEnable);                          //P0.1�ж�ʹ��
                                LED_ON();
                                Delay_Ms(10);
                                AdcValue = Adc_GetVoltage();
                                Adc[0]   = AdcValue & 0xFF;                     //����ǧ��������Ƶ������������Ƶģ�幤��ʱ����Ḵλ
                                Adc[1]   = (AdcValue >> 8) & 0xFF;
                                Rf_Send(Adc,2);
                                //Delay_Ms(1000);
                                LED_OFF();  
                                
                                //Wdt_FeetDog();                                  //������ι��
                                
                                
                                if((AdcValue >= 3500) && (AdcValue <= 3900))
                                { 
                                  LED_ON();
                                  Delay_Ms(1000);
                                  LED_OFF();                                    //��ص͵�ѹ��˸���� 
                                  Status = Card_ReadBlock(CARD_INFORMATION,BlockData);  //�����ʱ�ж�������������ȶ�������
                                  if(Status == MFRC522_OK)
                                  {
                                    RunMode = ReadCardAction;                   //ִ�ж�������
                                    break;
                                  }
                                  
                                  if(IsKeyOpen == OPEN_YES)
                                  {
                                    RunMode = KeyAction;                        //ִ�л�е�������� 
                                    break;         
                                  }
                                  
                                  RunMode = BatteryDetection;                   //����Ѿ���������ؼ���������ȣ��������񲻹���
                                  CC2530_Sleep();                               //˯��1S  
                                  break;
                                }
                                      
                                else if(AdcValue < 3500)
                                {
                                  LED_ON();
                                  CC2530_Sleep_Config(LastSleep);               //������ˢ��������Ч����Կ�׿�������
                                  //KeyDoor_Int_Disable();
                                  Sleep_SetMode(SLEEP_PM3);                     //Կ�׿��Ż����ܹ����ѵ�          
                                }
                                
                                CC2530_Int(AllDisable);                         //�رյ��������ж�,����˯���ж�
                                RunMode = ReadCardAction;                       //������ؾͼ���ִ�ж���
                                break;
        
        
        /*1.ˢ����Ȩ�Ϳ���*/             
        case ReadCardAction:    Status = Card_ReadBlock(CARD_INFORMATION,BlockData); 
                                if(Status == MFRC522_OK)
                                {
                                  /*1.1.�����δ����*/
                                  if(LOCK == LOCK_NO)
                                  {
                                    /*1.2.������������Ч(δʵ��)*/
                                    
                                    /*1.3.��Ȩ������*/
                                    Data_DoorID_Read(DoorId);                   //����ID��ȡ
                                    if((BlockData[2] == DoorId[0]) && (BlockData[3] == DoorId[1]))    //����Ǵ˹�˾��ײ¥����Ȩ��
                                    {
                                      /*1.3.1.��Ȩ����ɾȨ��*/
                                      if(((BlockData[0] == Authorization)    && (BlockData[1] == Authorization)) ||
                                         ((BlockData[0] == UnAuthorizataion) && (BlockData[1] == UnAuthorizataion)))     
                                      {
                                         Door_Open(LedOn);
                                         LED_OFF();
                                         Delay_Ms(1000);
                                         Warn_OneBuzzer_Led(LedOn);
                                         Card_Authorization(BlockData[0]);      //������Ȩ��ɾȨ����
                                         Door_Close(LedOn);  
                                      }
                                      
                                      /*1.3.2.�ܿ�*/
                                      else if((BlockData[0] == TotalCard) && (BlockData[1] == TotalCard))   
                                      {
                                        Door_Open_Close();
                                      }
                                      
                                      /*1.3.3.¥�㿨*/
                    
                                    }
                                    
                                    /*1.4.��ͨ������*/
                                    else
                                    {
                                      if(Data_CommonCard_Confirm(BlockData+12)) 
                                      {
                                        Door_Open_Close();   
                                      }
                                      
                                      else
                                      {
                                        Warn_Card_Fail();                       //��Ȩ�޿�����
                                      }
                                      
                                    }
                                  }
                                  
                                  /*1.5.�ŷ���*/
                                  else
                                  {
                                    Warn_Door_Lock();                            //�ŷ���ʱ��Ҫ��������
                                  }
     
                                  /*1.6.ˢ�������*/
                                  RunMode = BatteryDetection;                  
                                  break;
                                }
                                
                                RunMode = KeyAction;                            //���δ���ż���ִ�л�е�������� 
                                break;          
        
                                
        /*2.��е����*/
        case KeyAction:         CC2530_Int(KeyEnable);                          //P0.1�ж�ʹ��
                                   
                                if(IsKeyOpen == OPEN_YES)
                                {
                                  Warn_Key_Open();                              //�൱��һֱ�������ˣ��˵�������  
                                  
                                  for (k1=0;k1<40000;k1++)                 
                                  { 
                                     for (k2=0;k2<200;k2++)
                                     {
                                       ;;
                                     }
                                     if(KEY == TURNOFF)                         //��ɸߵ�ƽ��״̬�������ٴ��ж�
                                     {
                                       break;
                                     }
                                  }
                                  IsKeyOpen = OPEN_NO;
                                  Warn_Key_Close();
                                  
                                  RunMode = BatteryDetection;                   //Կ�׿��ż����        
                                  break;
                                }
                                  
                                CC2530_Int(AllDisable);                         //�رյ��������ж�,����˯���ж�
                                RunMode = SysIntoSleep;
                                break;
       
                              
        /*5.ϵͳ����˯��*/
        case SysIntoSleep:      //Wdt_FeetDog();                                  //������ι��
                                RunMode = ReadCardAction;               
                                CC2530_Sleep();                                 //˯��1S          
                                break;                             
                                
                                
                                    
        default:                RunMode = ReadCardAction;
                                break;
      }    
    }
}  



/* ------------------------------------------------------------------------------------------------
 *                                          �жϺ���
 * ------------------------------------------------------------------------------------------------
 */

/************************************************
 *function :  P0_ISR
 *describe :  P0_1���ڼ��Կ�׿��ţ�P0_4���ڼ�⿪��˲������˲��
 *input    :  ��
 *return   :  ��
 ***********************************************/
#pragma vector = P0INT_VECTOR
__interrupt void P0_ISR(void)
{
 EA   =  0;

 /*1.Կ�׿���*/
 if(P0IFG & 0x02)                 //P0.1�����ж�
 { 
   if((KEY == TURNON) && (IsKeyOpen == OPEN_NO))
   {
     CC2530_UnSleep_Config();     //���˯��ʱ�˿�����
     Sleep_SetMode(SLEEP_OFF);    //������������ģʽ
     IsKeyOpen = OPEN_YES;        //Կ�׿���
   }
 }
 
 /*2.����֮���жϹ���*/
 else if(P0IFG & 0x10)            //P0.4�����ж�
 {
   if((DOOR == PUSHIN) && (IsDoorClose == CLOSE_NO))
   {
     IsDoorClose = CLOSE_YES;     //����֮�������
   }
 }
 
 P0IFG = 0;                       //����жϱ�־
 P0IF  = 0;                        //���ж�δ��   
 EA    = 1;
}



/************************************************
 * @fn          ST_ISR
 * @brief       ˯�߶�ʱ���ж�
 * @param       ��
 * @return      ��
 ************************************************
 */
#pragma vector = ST_VECTOR
__interrupt void ST_ISR(void)
{
  STIF = 0x00;                  //����жϱ�־
  CC2530_UnSleep_Config();      //���˯��ʱ�˿�����
  Sleep_SetMode(SLEEP_OFF);     //������������ģʽ
}


/************************************************
 *function :  Rf_ISR()
 *describe :  RF��Ƶ�����жϺ���
 *input    :  None
 *return   :  None
 ***********************************************/
#pragma vector=RF_VECTOR  
__interrupt void rf_isr(void)  
{
  uint8_t Rx[1024];
  EA   =  0;
  
  //Wdt_Feet();
  
  if(RFIRQF0&(1<<6))                    //���յ�һ������������֡           
  {
    Rf_Receive(Rx);                     //�������ݴ�����
    S1CON     = 0;                      //���RF�жϱ�־
    RFIRQF0  &= ~(1<<6);                //���RF��������жϱ�־
    
  }
  
  EA   =  1;
}