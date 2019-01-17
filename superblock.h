#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "inode.h"
struct Superblock {
	int ptrRoot; //指向根目录的指针
	FreeInode freeInode;
	int block[101];//空闲盘块号栈
};

#define SUPERBLOCKSIZE sizeof(struct Superblock)

//初始化超级块
void InitSuperblock(struct Superblock *superblock);

//获取超级块的地址
int GetPosOfSuperblock();

//读取超级块
struct Superblock GetSuperblock(FILE *fp);

//写入超级块
void WriteSuperblock(FILE *fp, struct Superblock superblock);

//设置根目录指针
void SetPtrRoot(FILE *fp, int p);

//显示超级块内容
void showSuperblock(FILE *fp);

#endif