#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "inode.h"
struct Superblock {
	int ptrRoot; 
	FreeInode freeInode;
	int block[101];
};

#define SUPERBLOCKSIZE sizeof(struct Superblock)


void InitSuperblock(struct Superblock *superblock);


int GetPosOfSuperblock();


struct Superblock GetSuperblock(FILE *fp);


void WriteSuperblock(FILE *fp, struct Superblock superblock);


void SetPtrRoot(FILE *fp, int p);

void showSuperblock(FILE *fp);

#endif