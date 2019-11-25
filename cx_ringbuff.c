/**
  ******************************************************************************
  * @file    cx_ringbuff.c
  * @author  CX
  * @version V1.0.0.2
  * @date    2016-07-13
  * @brief   1.0.0.2 ����֡�߼��Ż�
		     �޸�ƥ������֡���������
		     ����ƥ������֡����
             1.0.0.1 �Ż������
		     �Ż���ȡ�߼�
		     ���Ӷ໺����֧��
	     1.0.0.0 ����ܹ������ɶ�д���νṹ��
  ******************************************************************************
  * @attention
  *
  * ��Ŀ   ��None
  * ����   : None
  * ʵ���� ��None
  *
  ******************************************************************************
  */

#include "cx_ringbuff.h"


/* ���˱���Ļ�������û�����ⲿģ�鹫��һ��������ʵ�ָ��ھۣ������ */
RingBuff_Typedef  RingBuffStruct;


/**
* @brief   ������ʵ����
* @param   rb_ptr �������ṹ���ַ
* @retval  None
* @notice  None
*/
void RingBuff_New(RingBuff_Typedef* rb_ptr)
{
	rb_ptr->Readpos = rb_ptr->RxBuff;
	rb_ptr->Writepos = rb_ptr->RxBuff;
	rb_ptr->ReadRetStateStruct.Readstate = Fail;
	rb_ptr->ReadRetStateStruct.ptr = NULL;
	rb_ptr->RingbufCount = 0;
	memset(rb_ptr->RxBuff, 0, MaxBuffSize);
}


/**
  * @brief   ��λ����
  * @param   rb_ptr �������ṹ���ַ,addr ��ǰ��д��ַ
  * @retval  addr+1 or 0 
  * @notice  None
  */
unsigned char* NextDataAddrHandle(RingBuff_Typedef* rb_ptr, unsigned char* addr)
{
	return (addr + 1) == (rb_ptr->RxBuff + MaxBuffSize) ? rb_ptr->RxBuff : (addr + 1);
}


/**
  * @brief   ���ֽں���
  * @param   rb_ptr �������ṹ���ַ
  * @retval  data
  * @notice  None
  */
ReadRetState_Typedef* ReadDataFromRingbuff(RingBuff_Typedef* rb_ptr)
{
	if (rb_ptr->RingbufCount > 0)
	{
		rb_ptr->ReadRetStateStruct.ptr = rb_ptr->Readpos;
		rb_ptr->ReadRetStateStruct.Readstate = Success;
		rb_ptr->Readpos = NextDataAddrHandle(rb_ptr, rb_ptr->Readpos);
		rb_ptr->RingbufCount--;
		return &rb_ptr->ReadRetStateStruct;
	}
	rb_ptr->ReadRetStateStruct.Readstate = Fail;
	rb_ptr->ReadRetStateStruct.ptr = NULL;
	return &rb_ptr->ReadRetStateStruct;
}


/**
  * @brief   д�ֽں���
  * @param   rb_ptr �������ṹ���ַ,data д������
  * @retval  None
  * @notice  None
  */
void WriteDataToRingbuff(RingBuff_Typedef* rb_ptr, unsigned char data)
{
	*(rb_ptr->Writepos) = data;
	rb_ptr->Writepos = NextDataAddrHandle(rb_ptr,rb_ptr->Writepos);
	rb_ptr->RingbufCount++;
}


/**
  * @brief   ��ָ����������
  * @param   rb_ptr �������ṹ���ַ,head ��ͷ,length ����
  * @retval  ��ȡ״̬�����ݰ���ַ
  * @notice  lengthΪ�������ݰ�����ͷ��β
*/
ReadRetState_Typedef* ReadEfectiveFrameFixLength(RingBuff_Typedef* rb_ptr, unsigned char head, unsigned char length)
{
	unsigned char count = rb_ptr->RingbufCount;              //��Ǵ���ǰ��Ч���ݴ�С
	if (count < length)
	{
		rb_ptr->ReadRetStateStruct.Readstate = Fail;
		rb_ptr->ReadRetStateStruct.ptr = NULL;
		return &rb_ptr->ReadRetStateStruct;
	}
	while (count >= length)
	{
		rb_ptr->ReadRetStateStruct = *ReadDataFromRingbuff(rb_ptr);
		count--;
		if (rb_ptr->ReadRetStateStruct.Readstate == Success && *rb_ptr->ReadRetStateStruct.ptr == head)
		{
			return &rb_ptr->ReadRetStateStruct;
		}
	}
	rb_ptr->ReadRetStateStruct.Readstate = Fail;
	rb_ptr->ReadRetStateStruct.ptr = NULL;
	return &rb_ptr->ReadRetStateStruct;
}


/**
  * @brief   ������֡
  * @param   rb_ptr �������ṹ���ַ,str ����֡
  * @retval  ��ȡ״̬
  * @notice  None
*/
ReadRetState_Enum ReadEfectiveFrame(RingBuff_Typedef* rb_ptr, const char* str)
{
	unsigned char count = rb_ptr->RingbufCount;                 //��Ǵ���ǰ��Ч���ݴ�С                
	unsigned char length = strlen(str);
	unsigned char i = 0;
	if (count < length)
	{
		return Fail;
	}
	while (count >= length)
	{
		rb_ptr->ReadRetStateStruct = *ReadDataFromRingbuff(rb_ptr);
		count--;
		if (rb_ptr->ReadRetStateStruct.Readstate == Success && *rb_ptr->ReadRetStateStruct.ptr == *(str + i))
		{
			count++;
			i++;
		}
		else
		{
			i = 0;
		}
		if (i == length)
		{
			return Success;
		}
	}
	return Fail;
}


/**
  * @brief   ƥ������֡
  * @param   rb_ptr �������ṹ���ַ,str ����֡����ע����ַ, ExpectFrameCount ����֡ע���ĸ���
  * @retval  ��ȡ״̬
  * @notice  None
*/
ReadRetState_Typedef* MatchExpectFrame(RingBuff_Typedef* rb_ptr, const char** str, unsigned char ExpectFrameCount)
{
	unsigned char Retcount = rb_ptr->RingbufCount;              
	unsigned char* RetPos = rb_ptr->Readpos;

	unsigned char i = 0;
	for (i = 0; i < ExpectFrameCount; i++)
	{
		if (ReadEfectiveFrame(rb_ptr, *(str + i)) == Success)
		{
			rb_ptr->ReadRetStateStruct.pos = i;
			rb_ptr->ReadRetStateStruct.Readstate = Success;
			return &rb_ptr->ReadRetStateStruct;
		}
		else
		{
			rb_ptr->RingbufCount = Retcount;
			rb_ptr->Readpos = RetPos;
		}
	}
	rb_ptr->ReadRetStateStruct.Readstate = Fail;
	return &rb_ptr->ReadRetStateStruct;
}

