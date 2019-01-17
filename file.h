#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#define FILENAMELEN 256
#define LEGALFILENAMELEN 16

struct Directory {
	char dirName[LEGALFILENAMELEN];
	int ptrInode;
};

//ȡ��Сֵ
int Min(int a, int b);

//�ӵ�һ��'/'���ָ��ַ���
int StringSplitFront(char *s, char *s1, char *s2);

//�ָ����һ��Ŀ¼
//����/root/my/xxx/ �ָ�Ϊ /root/my/ �� xxx/
void SplitDirectory(char *s, char *s1, char *s2);

//�ָ��ļ�����Ŀ¼
void SplitFile(char *s, char *s1, char *s2);

//�ж��ļ��Ƿ���ж�Ȩ��
int AllowWrite(FILE *fp, int curInode, char username[]);

//�ж��ļ��Ƿ����дȨ��
int AllowRead(FILE *fp, int curInode, char username[]);

//�ж��ļ����Ƿ�Ϸ�
int LegalFilename(char filename[]);

//�����ļ�·����ȡinode
int GetInodeOfName(FILE *fp, int curInode,const char filePath[]);

//�����������ѵĻ�ȡinode����
int GetInodeOfNameWithoutError(FILE *fp, int curInode, char filePath[]);

//����Ŀ¼�ļ�
int CreateDirectory(FILE *fp, int curInode,const char dirName[], const char username[], const char group[]);

//��ʾ�ļ�Ŀ¼
void ListDirectory(FILE* fp, int curInode,const char dirName[]);

//�����ļ�
int CreateFile(FILE *fp, int curInode, const char fileName[], const char username[], const char group[]);

//д�ļ�
void WriteFile(FILE *fp, int curInode, void *data, int size,const char username[]);

//����ļ�����
void PrintFile(FILE *fp, int curInode, char username[]);

//���inode���ļ�·��
void PrintPathOfInode(FILE *fp, int curInode);

//����inode��������
void AddLink(FILE *fp, int iInode, int x);

//ɾ��inode
int DeleteInode(FILE *fp, int iInode);

//�ж�Ŀ¼�Ƿ�Ϊ��
int EmptyDirectory(FILE *fp, int iInode);

//��ȡ����
void ReadData(FILE *fp, int pInode, char *data, int size);
void ReadData1(FILE *fp, int pInode, char *data, int size);

//��������
void CleanData(FILE *fp, int iInode);

//д������
void WriteData(FILE *fp, int pInode, void *vData, int size);
void WriteData1(FILE *fp, int pInode, void *vData, int size);

#endif