#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <stdio.h>

#define DATABLOCKSIZE 512
#define DATABLOCKNUM 1000

//��ȡ���ݿ�ĵ�ַ
int GetPosOfDatablock(int index);


//��ȡ�������ݿ�ĵ�ַ
//int GetPosOfFreeDatablock();

//��ʼ���������ݿ�
//void InitFreeDatablock(FILE *fp);

//��ȡһ���������ݿ�
int GetFreeDatablock(FILE *fp);

//���һ�����ݿ�
void PushFreeDatablock(FILE *fp, int index);

#endif