#ifndef INODE_H
#define INODE_H

#include <stdio.h>

#include "user.h"
#include "file.h"
#define INODENUM 40
#define ADDR0NUM 4
#define ADDR1NUM 1
#define ADDR2NUM 1
#define NADDRS 2
struct Inode {
	char name[LEGALFILENAMELEN]; //文件名
	int size;//文件大小
	int linknum;//文件联接计数
	char owner[LEGALUSERNAMELEN];//文件拥有者
	char group[LEGALGROUPLEN];//文件所属组
	int permission;//文件权限
	int type;//类别(0表示文件夹，1表示文件)
	char time[83];//文件最后修改时间
	
	int addr0[ADDR0NUM]; //直接块号
	int addr1[ADDR1NUM]; //一次间址
	int addr2[ADDR2NUM]; //二次间址
};
#define INODESIZE sizeof(struct Inode)

//获取数据块的地址
int GetPosOfInode(int index);

//获取inode
struct Inode GetInode(FILE *fp, int p);

//写入inode
void WriteInode(FILE *fp, struct Inode inode, int p);

//输出inode数据
void PrintInode(struct Inode inode);

struct FreeInode {
	int num; //空闲inode数目
	int stack[INODENUM]; //空闲inode栈
};

#define FREEINODESIZE sizeof(struct FreeInode)


//初始化空闲inode栈
void InitFreeInode(FreeInode *freeInode);

//获取空闲inode
int GetFreeInode(FILE *fp);

//添加inode
void PushFreeInode(FILE *fp, int index);

#endif