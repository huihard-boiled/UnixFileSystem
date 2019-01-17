#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "superblock.h"
#include "inode.h"
#include "datablock.h"
#include "file.h"
#include "user.h"

//创建一个新的文件系统
void CreateFilesystem(const char fsName[]) {
	printf("Creating filesystem...\n");
	FILE *fp;
	fp = fopen(fsName, "wb+");
	int sumSize = SUPERBLOCKSIZE + INODENUM * INODESIZE + DATABLOCKNUM * DATABLOCKSIZE;
	char fillChar = 0;
	fwrite(&fillChar, 0, sumSize, fp);

	//初始化超级块
	printf("Initializing superblock...\n");
	struct Superblock superblock;
	InitSuperblock(&superblock);
	WriteSuperblock(fp, superblock);
	//成组链接
	int blockNumber = superblock.block[1];
	for (; blockNumber <= DATABLOCKNUM; blockNumber += 100) {
		int block[101];
		block[0] = Min(100, DATABLOCKNUM-blockNumber);
		if (block[0] == 100) {
			for (int j = 100; j > 0; j--) {
				block[101 - j] = j + blockNumber;
			}
		}
		else {
			block[1] = 0;
			for (int j = 0; j < block[0]; ++j) {
				block[j + 2] = blockNumber + block[0] - j;
			}
		}
		fseek(fp, GetPosOfDatablock(blockNumber), SEEK_SET);
		fwrite(&block, sizeof(block), 1, fp);
	}


	//创建根目录
	printf("Creating directory '/'...\n");
	int currentInode;
	currentInode = CreateDirectory(fp, -1, "/", "root", "root");
	SetPtrRoot(fp, currentInode);
	if (currentInode == -1) {
		printf("Create directory '/' failed.\n");
		remove(fsName);
		exit(1);
	}
	else {
		printf("Create directory '/' successfully.\n");
	}

	//PrintInode(GetInode(fp, currentInode));
	//创建root目录
	printf("Creating directory '/root/'...\n");
	if (CreateDirectory(fp, currentInode, "root/", "root", "root") == -1) {
		printf("Create directory '/root/' failed.\n");
		remove(fsName);
		exit(2);
	}
	else {
		printf("Create directory '/root/' successfully.\n");
	}
	//创建home目录
	printf("Creating directory '/home/'...\n");
	if (CreateDirectory(fp, currentInode, "home/", "root", "root") == -1) {
		printf("Creating directory '/home/' failed.\n");
		remove(fsName);
		exit(3);
	}
	else {
		printf("Create directory '/home/' successfully.\n");
	}

	//创建sys目录
	printf("Creating directory '/sys/'...\n");
	currentInode = CreateDirectory(fp, currentInode, "sys/", "root", "root");
	if (currentInode == -1) {
		printf("Creating directory '/sys/' failed.\n");
		remove(fsName);
		exit(4);
	}
	else {
		printf("Create directory '/sys/' successfully.\n");
	}

	//创建密码文件
	struct User user[3];
	user[0] = CreateUser("root", "root", "root");
	user[1] = CreateUser("a", "a", "a");
	user[2] = CreateUser("b", "b", "a");
	int tmpInode = CreateFile(fp, currentInode, "userinfo", "root", "root");
	struct Inode inode = GetInode(fp, tmpInode);
	inode.permission = 0750;
	WriteInode(fp, inode, tmpInode);
	WriteFile(fp, tmpInode, user, sizeof(user), "root");

	CreateDirectory(fp, -1, "/home/a/", "root", "root");
	CreateDirectory(fp, -1, "/home/b/", "root", "root");

	printf("Create filesystem successfully.\n");
}