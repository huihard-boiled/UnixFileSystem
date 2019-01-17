#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <stdio.h>

#define DATABLOCKSIZE 512
#define DATABLOCKNUM 1000

//获取数据块的地址
int GetPosOfDatablock(int index);


//获取空闲数据块的地址
//int GetPosOfFreeDatablock();

//初始化空闲数据块
//void InitFreeDatablock(FILE *fp);

//获取一个空闲数据块
int GetFreeDatablock(FILE *fp);

//添加一个数据块
void PushFreeDatablock(FILE *fp, int index);

#endif