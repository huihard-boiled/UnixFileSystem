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
	char name[LEGALFILENAMELEN]; //�ļ���
	int size;//�ļ���С
	int linknum;//�ļ����Ӽ���
	char owner[LEGALUSERNAMELEN];//�ļ�ӵ����
	char group[LEGALGROUPLEN];//�ļ�������
	int permission;//�ļ�Ȩ��
	int type;//���(0��ʾ�ļ��У�1��ʾ�ļ�)
	char time[83];//�ļ�����޸�ʱ��
	
	int addr0[ADDR0NUM]; //ֱ�ӿ��
	int addr1[ADDR1NUM]; //һ�μ�ַ
	int addr2[ADDR2NUM]; //���μ�ַ
};
#define INODESIZE sizeof(struct Inode)

//��ȡ���ݿ�ĵ�ַ
int GetPosOfInode(int index);

//��ȡinode
struct Inode GetInode(FILE *fp, int p);

//д��inode
void WriteInode(FILE *fp, struct Inode inode, int p);

//���inode����
void PrintInode(struct Inode inode);

struct FreeInode {
	int num; //����inode��Ŀ
	int stack[INODENUM]; //����inodeջ
};

#define FREEINODESIZE sizeof(struct FreeInode)


//��ʼ������inodeջ
void InitFreeInode(FreeInode *freeInode);

//��ȡ����inode
int GetFreeInode(FILE *fp);

//���inode
void PushFreeInode(FILE *fp, int index);

#endif