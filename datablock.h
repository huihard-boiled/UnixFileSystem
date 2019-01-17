#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <stdio.h>

#define DATABLOCKSIZE 512
#define DATABLOCKNUM 1000

int GetPosOfDatablock(int index);



//int GetPosOfFreeDatablock();


//void InitFreeDatablock(FILE *fp);


int GetFreeDatablock(FILE *fp);


void PushFreeDatablock(FILE *fp, int index);

#endif