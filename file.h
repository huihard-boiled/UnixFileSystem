#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#define FILENAMELEN 256
#define LEGALFILENAMELEN 16

struct Directory {
	char dirName[LEGALFILENAMELEN];
	int ptrInode;
};

//取最小值
int Min(int a, int b);

//从第一个'/'处分割字符串
int StringSplitFront(char *s, char *s1, char *s2);

//分割最后一级目录
//例：/root/my/xxx/ 分割为 /root/my/ 和 xxx/
void SplitDirectory(char *s, char *s1, char *s2);

//分割文件名与目录
void SplitFile(char *s, char *s1, char *s2);

//判断文件是否具有读权限
int AllowWrite(FILE *fp, int curInode, char username[]);

//判断文件是否具有写权限
int AllowRead(FILE *fp, int curInode, char username[]);

//判断文件名是否合法
int LegalFilename(char filename[]);

//根据文件路径获取inode
int GetInodeOfName(FILE *fp, int curInode,const char filePath[]);

//不带错误提醒的获取inode函数
int GetInodeOfNameWithoutError(FILE *fp, int curInode, char filePath[]);

//创建目录文件
int CreateDirectory(FILE *fp, int curInode,const char dirName[], const char username[], const char group[]);

//显示文件目录
void ListDirectory(FILE* fp, int curInode,const char dirName[]);

//创建文件
int CreateFile(FILE *fp, int curInode, const char fileName[], const char username[], const char group[]);

//写文件
void WriteFile(FILE *fp, int curInode, void *data, int size,const char username[]);

//输出文件内容
void PrintFile(FILE *fp, int curInode, char username[]);

//输出inode的文件路径
void PrintPathOfInode(FILE *fp, int curInode);

//增加inode的链接数
void AddLink(FILE *fp, int iInode, int x);

//删除inode
int DeleteInode(FILE *fp, int iInode);

//判断目录是否为空
int EmptyDirectory(FILE *fp, int iInode);

//读取数据
void ReadData(FILE *fp, int pInode, char *data, int size);
void ReadData1(FILE *fp, int pInode, char *data, int size);

//清理数据
void CleanData(FILE *fp, int iInode);

//写入数据
void WriteData(FILE *fp, int pInode, void *vData, int size);
void WriteData1(FILE *fp, int pInode, void *vData, int size);

#endif