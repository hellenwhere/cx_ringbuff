#ifndef __CX_RINGBUFF_H
#define __CX_RINGBUFF_H
 
 
#include "stdlib.h"
#include "string.h"
 
 
#define         MaxBuffSize        256        //��������С���������ã������Ϊ2^X
 
 
typedef enum
{
	Success = 1,
	Fail = !Success,
}ReadRetState_Enum;
 
 
typedef struct
{
	ReadRetState_Enum Readstate;                //��״̬
	unsigned char* ptr;                         //��ȡ�ֽڵ�ַ
	unsigned char  pos;                         //����֡��ע�����λ��
}ReadRetState_Typedef;
 
 
 
typedef struct
{
	unsigned char* Writepos;                    //д���ַ
	unsigned char* Readpos;                     //��ȡ��ַ
	unsigned char RingbufCount;                 //��Чδ�����ݴ�С
	ReadRetState_Typedef ReadRetStateStruct;    //���������
	unsigned char RxBuff[MaxBuffSize];          //���ݻ�����
}RingBuff_Typedef;
 
 
void RingBuff_New(RingBuff_Typedef* rb_ptr);
unsigned char* NextDataAddrHandle(RingBuff_Typedef* rb_ptr,unsigned char* addr);
ReadRetState_Typedef* ReadDataFromRingbuff(RingBuff_Typedef* rb_ptr);
void WriteDataToRingbuff(RingBuff_Typedef* rb_ptr,unsigned char data);
ReadRetState_Typedef* ReadEfectiveFrameFixLength(RingBuff_Typedef* rb_ptr, unsigned char head, unsigned char length);
ReadRetState_Enum ReadEfectiveFrame(RingBuff_Typedef* rb_ptr, const char* str);
ReadRetState_Typedef* MatchExpectFrame(RingBuff_Typedef* rb_ptr, const char** str, unsigned char ExpectFrameCount);
 
 
#endif