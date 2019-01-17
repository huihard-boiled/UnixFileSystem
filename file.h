#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#define FILENAMELEN 256
#define LEGALFILENAMELEN 16

struct Directory {
	char dirName[LEGALFILENAMELEN];
	int ptrInode;
};


int Min(int a, int b);

int StringSplitFront(char *s, char *s1, char *s2);


void SplitDirectory(char *s, char *s1, char *s2);


void SplitFile(char *s, char *s1, char *s2);


int AllowWrite(FILE *fp, int curInode, char username[]);


int AllowRead(FILE *fp, int curInode, char username[]);


int LegalFilename(char filename[]);


int GetInodeOfName(FILE *fp, int curInode,const char filePath[]);


int GetInodeOfNameWithoutError(FILE *fp, int curInode, char filePath[]);


int CreateDirectory(FILE *fp, int curInode,const char dirName[], const char username[], const char group[]);


void ListDirectory(FILE* fp, int curInode,const char dirName[]);


int CreateFile(FILE *fp, int curInode, const char fileName[], const char username[], const char group[]);


void WriteFile(FILE *fp, int curInode, void *data, int size,const char username[]);


void PrintFile(FILE *fp, int curInode, char username[]);


void PrintPathOfInode(FILE *fp, int curInode);


void AddLink(FILE *fp, int iInode, int x);


int DeleteInode(FILE *fp, int iInode);


int EmptyDirectory(FILE *fp, int iInode);


void ReadData(FILE *fp, int pInode, char *data, int size);
void ReadData1(FILE *fp, int pInode, char *data, int size);


void CleanData(FILE *fp, int iInode);


void WriteData(FILE *fp, int pInode, void *vData, int size);
void WriteData1(FILE *fp, int pInode, void *vData, int size);

#endif