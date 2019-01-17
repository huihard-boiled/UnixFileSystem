#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "inode.h"
struct Superblock {
	int ptrRoot; //ָ���Ŀ¼��ָ��
	FreeInode freeInode;
	int block[101];//�����̿��ջ
};

#define SUPERBLOCKSIZE sizeof(struct Superblock)

//��ʼ��������
void InitSuperblock(struct Superblock *superblock);

//��ȡ������ĵ�ַ
int GetPosOfSuperblock();

//��ȡ������
struct Superblock GetSuperblock(FILE *fp);

//д�볬����
void WriteSuperblock(FILE *fp, struct Superblock superblock);

//���ø�Ŀ¼ָ��
void SetPtrRoot(FILE *fp, int p);

//��ʾ����������
void showSuperblock(FILE *fp);

#endif