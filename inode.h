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
	char name[LEGALFILENAMELEN]; 
	int size;
	int linknum;
	char owner[LEGALUSERNAMELEN];
	char group[LEGALGROUPLEN];
	int permission;
	int type;
	char time[83];
	
	int addr0[ADDR0NUM]; 
	int addr1[ADDR1NUM]; 
	int addr2[ADDR2NUM]; 
};
#define INODESIZE sizeof(struct Inode)


int GetPosOfInode(int index);


struct Inode GetInode(FILE *fp, int p);


void WriteInode(FILE *fp, struct Inode inode, int p);


void PrintInode(struct Inode inode);

struct FreeInode {
	int num; 
	int stack[INODENUM]; 
};

#define FREEINODESIZE sizeof(struct FreeInode)



void InitFreeInode(FreeInode *freeInode);


int GetFreeInode(FILE *fp);


void PushFreeInode(FILE *fp, int index);

#endif