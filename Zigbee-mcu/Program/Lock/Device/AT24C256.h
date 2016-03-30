#ifndef AT24C256_H_
#define AT24C256_H_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "I2c.h"
#include <string.h>
/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */
/******************I2C���ߴӻ���ַ*********************/
#define AT24C256_WRITE_ADDR      0xA0   //д������ַ
#define AT24C256_READ_ADDR       0xA1   //��������ַ����ʵ����д��ַ+1��



/******************�洢���򻮷�*********************/

/*1.����ID*/
#define DOORID_BASE_ADDR             0x0000     //����ID��Ϣ
  #define DOORID_MAX_ADDR            0x0003

#define DOORID_RESERVED_ADDR         0x000F     //����

/*2.��ͨ���б�*/
#define COMMONCARD_BASE_ADDR         0x0010     //��ͨ�����100�ţ�4byte/�ţ�
  #define COMMONCARD_MAX_ADDR        0x019F     
  #define COMMONCARD_MAX1_ADDR       0x0023     //�����ã����Ȳ���5����ͨ��

#define COMMONCARD_RESERVED_ADDR     0x01FF     //����








/*���洢��ַ*/
#define EEPROM_MAX_ADDR               0x7FFF



//#define ORGCARDID_BASE_ADDR           0x0000    //��ͨ�����90�ţ�4byte/�ţ�
//  #define ORGCARDID_MAX_ADDR          0x0167
//
//
//#define AUTHCARDID_BASE_ADDR          0x0190    //��Ȩ�����3�ţ�4byte/�ţ�����Ȩ�����������ǰ����մ洢����
//  #define AUTHCARDID_MAX_ADDR         0x019B
//
//#define UNAUTHCARDID_BASE_ADDR        0x019C    //ɾȨ�����3�ţ�4byte/�ţ�����Ȩ�����������ǰ����մ洢����
//  #define UNAUTHCARDID_MAX_ADDR       0x01A7
//
//#define TOTALCARD_BASE_ADDR           0x01A8    //�ܿ����10�ţ�4byte/�ţ�����Ȩ�����������ǰ����մ洢����
//  #define TOTALCARD_MAX_ADDR          0x01CF
//
//#define FLOORCARD_BASE_ADDR           0x01D0    //¥�㿨���10�ţ�4byte/�ţ�����Ȩ�����������ǰ����մ洢����
//  #define FLOORCARD_MAX_ADDR          0x01F7
//
//
//#define BLACKCARD_BASE_ADDR           0x0200    //���������200����Ϊ��ʧ��Ȩ������(��������ͨ��)��4byte/�ţ�
//  #define BLACKCARD_MAX_ADDR          0x051F    
//
//#define CARD_RESERVED_ADDR            0x05FF    //������������
//
///*2.����ˢ����¼*/
//#define AUTHCARD_RECORD_BASE_ADDR     0x0600    //��Ȩ����ȨɾȨ��¼200����16Byte/�� ������־1Byte + ʱ��6byte +������1byte + ��Ȩ��4byte + ��ͨ��4byte,����������ͨ��������ȫ��00��
//  #define AUTHCARD_RECORD_MAX_ADDR    0x127F
//
//#define AUTHCARD_P_BASE_ADDR          0x1300    //��Ȩ��ָ��8����2Byte/�� ��ǰ�洢�ĵ�ַ1Byte + �洢λ��1Byte��
//#define AUTHCARD_P_MAX_ADDR           0x130F
//  
//
//#define AUTHCARD_RECORD_RESERVED_ADDR 0x13FF
//
//#define ORGCARD_RECORD_BASE_ADDR      0x1400    //��ͨ��(�����ܿ���¥�㿨)ˢ����¼20*30*2*12��������2���£�12Byte/�� ������־1Byte + ʱ��6Byte+������1Byte + ��4Byte��
//  #define ORGCARD_RECORD_MAX_ADDR     0x4C3F    
//
//#define ORGCARD_P_BASE_ADDR           0x4D00    //��ͨ��ָ��256����2Byte/�� ��ǰ�洢�ĵ�ַ1Byte + �洢λ��1Byte��
//#define ORGCARD_P_MAX_ADDR            0x4EFF
//
//
//#define RECORD_RESERVED_ADDR          0x4FFF    //��¼������







/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
void AT24C256_DelayMs(uint16_t n);
uint8_t AT24C256_WriteByte(uint8_t SlaveAddr,uint16_t ByteAddr,uint8_t Data);
uint8_t AT24C256_ReadByte(uint8_t SlaveAddr,uint16_t ByteAddr,uint8_t *Data);
uint8_t AT24C256_WriteBuff(uint8_t SlaveAddr, uint16_t ByteAddr, uint8_t *Data, uint8_t Len);
uint8_t AT24C256_ReadBuff(uint8_t SlaveAddr, uint16_t ByteAddr, uint8_t *Data, uint8_t Len);



#endif