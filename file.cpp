#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "file.h"
#include "superblock.h"
#include "inode.h"
#include "datablock.h"

int UMARK = 0022;

int Min(int a, int b) {
	if (a < b) return a;
	else return b;
}

int StringSplitFront(char *s, char *s1, char *s2) {
	
	if (strcmp(s, "") == 0) {
		strcpy(s1, "");
		strcpy(s2, "");
		return 0;
	}

	int i;
	for (i = 0; s[i] != '\0'; i++) {
		if (s[i] == '/') break;
	}

	
	if (s[i] != '/') {
		strcpy(s1, s);
		strcpy(s2, "");
		return 1;
	}

	
	for (i = 0; s[i] != '\0'; i++) {
		if (s[i] == '/') break;
	}

	i++;
	strcpy(s1, s);
	strcpy(s2, s + i);
	s1[i] = '\0';

	if (strcmp(s2, "") == 0) return 1;
	else return 2;
}

void SplitDirectory(char *s, char *s1, char *s2) {
	int i;
	for (i = strlen(s) - 2; i >= 0; i--) {
		if (s[i] == '/') break;
	}
	i++;
	strcpy(s1, s);
	strcpy(s2, s + i);
	s1[i] = '\0';
}

void SplitFile(char *s, char *s1, char *s2) {
	int i;
	for (i = strlen(s) - 1; i >= 0; i--) {
		if (s[i] == '/') break;
	}
	i++;
	strcpy(s1, s);
	strcpy(s2, s + i);
	s1[i] = '\0';
}

int AllowWrite(FILE *fp, int curInode, char username[]) {
	if (strcmp(username, "root") == 0) return 1;

	struct Inode inode = GetInode(fp, curInode);
	if (strcmp(username, inode.owner) == 0) {
		if (inode.permission & (1 << 7)) return 1;
	}
	if (strcmp(GetGroup(fp, username), inode.group) == 0) {
		if (inode.permission & (1 << 4)) return 1;
	}
	if (inode.permission & (1 << 1)) return 1;
	return 0;
}

int AllowRead(FILE *fp, int curInode, char username[]) {
	if (strcmp(username, "root") == 0) return 1;

	struct Inode inode = GetInode(fp, curInode);
	if (strcmp(username, inode.owner) == 0) {
		if (inode.permission & (1 << 8)) return 1;
	}
	if (strcmp(GetGroup(fp, username), inode.group) == 0) {
		if (inode.permission & (1 << 5)) return 1;
	}
	if (inode.permission & (1 << 2)) return 1;
	return 0;
}

int LegalFilename(char filename[]) {
	if (filename[strlen(filename) - 1] == '/') filename[strlen(filename) - 1] = '\0';
	if (strlen(filename) < LEGALFILENAMELEN) {
		int i;
		for (i = 0; filename[i] != '\0'; i++) {
			if (!('a' <= filename[i] && filename[i] <= 'z'
				|| 'A' <= filename[i] && filename[i] <= 'Z'
				|| '0' <= filename[i] && filename[i] <= '9')) {
				printf("Filename can only contain latin characters and numbers!\n");
				return 0;
			}
		}
	}
	else {
		printf("The length of filename must less than %d!\n", LEGALFILENAMELEN);
		return 0;
	}
	return 1;
}

int GetInodeOfName(FILE *fp, int curInode,const char filePath[]) {
	//printf("curInode = %d, filePath = %s\n", curInode, filePath);

	int i;

	if (strcmp(filePath, "") == 0) return curInode;

	if (curInode == -1) { 
		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		StringSplitFront(const_cast<char*>(filePath), s1, s2);
		if (strcmp(s1, "/") != 0) {
			printf("Invalid file path!\n");
			return -1;
		}

		struct Superblock superblock = GetSuperblock(fp);
		curInode = superblock.ptrRoot;

		return GetInodeOfName(fp, curInode, s2);
	}
	else { //相对路径
		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		StringSplitFront(const_cast<char*>(filePath), s1, s2);

		struct Inode inode = GetInode(fp, curInode);

		char *data = (char *)malloc(inode.size);
		ReadData(fp, curInode, data, inode.size);

		struct Directory *dir = (struct Directory *)data;
		int nDir = inode.size / sizeof(struct Directory);

		if (s1[strlen(s1) - 1] == '/') s1[strlen(s1) - 1] = '\0';

		curInode = -1;
		for (i = 0; i < nDir; i++) {
			if (strcmp(s1, dir[i].dirName) == 0) {
				curInode = dir[i].ptrInode;
				break;
			}
		}

		if (curInode == -1) {
			printf("Invalid file path!\n");
			return -1;
		}

		return GetInodeOfName(fp, curInode, s2);
	}
}

int GetInodeOfNameWithoutError(FILE *fp, int curInode, char filePath[]) {
	int i;

	if (strcmp(filePath, "") == 0) return curInode;

	if (curInode == -1) { //绝对路径
		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		StringSplitFront(filePath, s1, s2);
		if (strcmp(s1, "/") != 0) {
			return -1;
		}

		struct Superblock superblock = GetSuperblock(fp);
		curInode = superblock.ptrRoot;

		return GetInodeOfName(fp, curInode, s2);
	}
	else { //相对路径
		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		StringSplitFront(filePath, s1, s2);

		struct Inode inode = GetInode(fp, curInode);

		char *data = (char *)malloc(inode.size);
		ReadData(fp, curInode, data, inode.size);

		struct Directory *dir = (struct Directory *)data;
		int nDir = inode.size / sizeof(struct Directory);

		if (s1[strlen(s1) - 1] == '/') s1[strlen(s1) - 1] = '\0';

		curInode = -1;
		for (i = 0; i < nDir; i++) {
			if (strcmp(s1, dir[i].dirName) == 0) {
				curInode = dir[i].ptrInode;
				break;
			}
		}

		if (curInode == -1) {
			return -1;
		}

		return GetInodeOfName(fp, curInode, s2);
	}
}

int CreateDirectory(FILE *fp, int curInode,const char dirName[], const char username[], const char group[]) {
	
	if (strcmp(dirName, "/") == 0) {
		struct Superblock superblock = GetSuperblock(fp);
		if (superblock.ptrRoot != -1) {
			printf("Directory '/' has been existed!\n");
			return -1;
		}

		struct Directory *dir = (struct Directory *)malloc(sizeof(struct Directory));
		strcpy(dir->dirName, ".");
		dir->ptrInode = GetFreeInode(fp);

		int iInode = dir->ptrInode;
		struct Inode inode = GetInode(fp, iInode);
		strcpy(inode.name, "/");
		inode.size = sizeof(struct Directory);
		inode.linknum = 2;
		inode.type = 0;
		strcpy(inode.owner, username);
		strcpy(inode.group, group);
		inode.permission = 0666;
		time_t t = time(0);
		strftime(inode.time, sizeof(inode.time), "%Y/%m/%d %X %A %jday %z", localtime(&t));
		memset(inode.addr0, -1, sizeof(inode.addr0));
		memset(inode.addr1, -1, sizeof(inode.addr1));
		memset(inode.addr2, -1, sizeof(inode.addr2));
		WriteInode(fp, inode, iInode);
		WriteData(fp, iInode, dir, sizeof(struct Directory));

		
		superblock = GetSuperblock(fp);
		superblock.ptrRoot = iInode;

		
		WriteSuperblock(fp, superblock);
		return dir->ptrInode;
	}

	if (curInode == -1) {
		if (GetInodeOfName(fp, -1, const_cast<char*>(dirName)) != -1) {
			printf("This directory already exists!\n");
			return -1;
		}

		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		SplitDirectory(const_cast<char*>(dirName), s1, s2);

		//printf("s1 = %s, s2 = %s\n", s1, s2);

		int iInode = GetInodeOfName(fp, -1, s1);
		if (iInode == -1) {
			printf("Invalid file path!\n");
			return -1;
		}

		return CreateDirectory(fp, iInode, s2, username, group);
	}
	else { 
		char tmp[FILENAMELEN];
		strcpy(tmp, dirName);
		tmp[strlen(tmp) - 1] = '\0';

		int iNewInode = GetFreeInode(fp);
		struct Directory dir[2];
		strcpy(dir[0].dirName, ".");
		dir[0].ptrInode = iNewInode;
		strcpy(dir[1].dirName, "..");
		dir[1].ptrInode = curInode;

		struct Inode inodeB;
		strcpy(inodeB.name, tmp);
		inodeB.size = 2 * sizeof(struct Directory);
		inodeB.linknum = 2;
		inodeB.type = 0;
		strcpy(inodeB.owner, username);
		strcpy(inodeB.group, group);
		inodeB.permission = 0777;
		time_t t = time(0);
		strftime(inodeB.time, sizeof(inodeB.time), "%Y/%m/%d %X %A %jday %z", localtime(&t));
		memset(inodeB.addr0, -1, sizeof(inodeB.addr0));
		memset(inodeB.addr1, -1, sizeof(inodeB.addr1));
		memset(inodeB.addr2, -1, sizeof(inodeB.addr2));

		WriteInode(fp, inodeB, iNewInode);
		WriteData(fp, iNewInode, dir, 2 * sizeof(struct Directory));

		struct Directory dirData;
		strcpy(dirData.dirName, tmp);
		dirData.ptrInode = iNewInode;

		
		struct Inode inodeA = GetInode(fp, curInode);

		char *data = (char *)malloc(inodeA.size + sizeof(struct Directory));
		ReadData(fp, curInode, data, inodeA.size);

		memcpy(data + inodeA.size, &dirData, sizeof(struct Directory));
		CleanData(fp, curInode);
		WriteData(fp, curInode, data, inodeA.size + sizeof(struct Directory));

		inodeA = GetInode(fp, curInode);
		inodeA.linknum++;
		WriteInode(fp, inodeA, curInode);

		return iNewInode;
	}
}

void ListDirectory(FILE* fp, int curInode,const char dirName[]) {
	int i;
	if (curInode == -1) {
		curInode = GetInodeOfName(fp, -1, dirName);
		const_cast<char*>(dirName)[0] = '\0';
		//printf("curInode = %d\n", curInode);
		if (curInode == -1) return;
	}

	if (strcmp(dirName, "") == 0) {
		struct Inode inode = GetInode(fp, curInode);

		if (inode.type != 0) {
			printf("This is not a directory!\n");
			return;
		}

		char *data = (char *)malloc(inode.size);
		ReadData(fp, curInode, data, inode.size);
		struct Directory *dirArray = (struct Directory *)data;
		int nDir = inode.size / sizeof(struct Directory);
		for (i = 0; i < nDir; i++) {
			printf("%s ", dirArray[i].dirName);
		}
		printf("\n");
		return;
	}

	char s1[FILENAMELEN];
	char s2[FILENAMELEN];
	StringSplitFront(const_cast<char*>(dirName), s1, s2);
	if (s1[strlen(s1) - 1] == '/') s1[strlen(s1) - 1] = '\0';

	struct Inode inode = GetInode(fp, curInode);

	char *data = (char *)malloc(inode.size);
	ReadData(fp, curInode, data, inode.size);
	struct Directory *dirArray = (struct Directory *)data;
	int nDir = inode.size / sizeof(struct Directory);
	for (i = 0; i < nDir; i++) {
		if (strcmp(dirArray[i].dirName, s1) == 0) {
			curInode = dirArray[i].ptrInode;
			ListDirectory(fp, curInode, s2);
			return;
		}
	}
	printf("Invalid directory path!\n");
}

int CreateFile(FILE *fp, int curInode, const char fileName[], const char username[], const char group[]) {
	if (curInode == -1) {
		if (GetInodeOfName(fp, -1, const_cast<char*>(fileName)) != -1) {
			printf("File '%s' already exists!\n", fileName);
			return -1;
		}

		char s1[FILENAMELEN];
		char s2[FILENAMELEN];
		SplitFile(const_cast<char*>(fileName), s1, s2);

		int iInode = GetInodeOfName(fp, -1, s1);
		if (iInode == -1) {
			printf("Invalid file path!\n");
			return -1;
		}

		return CreateFile(fp, iInode, s2, username, group);
	}
	else { //相对路径
		int iNewInode = GetFreeInode(fp);

		struct Inode inodeB;
		strcpy(inodeB.name, fileName);
		inodeB.size = 0;
		inodeB.linknum = 1;
		inodeB.type = 1;
		strcpy(inodeB.owner, username);
		strcpy(inodeB.group, group);
		inodeB.permission = 0777-UMARK;
		time_t t = time(0);
		strftime(inodeB.time, sizeof(inodeB.time), "%Y/%m/%d %X %A %jday %z", localtime(&t));
		memset(inodeB.addr0, -1, sizeof(inodeB.addr0));
		memset(inodeB.addr1, -1, sizeof(inodeB.addr1));
		memset(inodeB.addr2, -1, sizeof(inodeB.addr2));

		WriteInode(fp, inodeB, iNewInode);

		char tmp[FILENAMELEN];
		strcpy(tmp, fileName);

		struct Directory dirData;
		strcpy(dirData.dirName, tmp);
		dirData.ptrInode = iNewInode;

		struct Inode inodeA = GetInode(fp, curInode);

		char *data = (char *)malloc(inodeA.size + sizeof(struct Directory));
		ReadData(fp, curInode, data, inodeA.size);

		memcpy(data + inodeA.size, &dirData, sizeof(struct Directory));
		CleanData(fp, curInode);
		WriteData(fp, curInode, data, inodeA.size + sizeof(struct Directory));
		return iNewInode;
	}
}

void WriteFile(FILE *fp, int curInode, void *data, int size,const char username[]) {
	if (!AllowWrite(fp, curInode, const_cast<char*>(username))) {
		printf("You cannot write this file!\n");
		return;
	}
	struct Inode inode = GetInode(fp, curInode);
	//PrintInode(inode);
	CleanData(fp, curInode);
	WriteData(fp, curInode, data, size);
	inode = GetInode(fp, curInode);
	//PrintInode(inode);
}

void PrintFile(FILE *fp, int curInode, char username[]) {
	if (!AllowRead(fp, curInode, username)) {
		printf("You cannot read this file!\n");
		return;
	}
	struct Inode inode = GetInode(fp, curInode);
	char *data = (char *)malloc(inode.size);
	ReadData(fp, curInode, data, inode.size);

	int i;
	for (i = 0; i < inode.size; i++) putchar(data[i]);
	printf("\n");
}

void PrintPathOfInode(FILE *fp, int curInode) {
	struct Inode inode = GetInode(fp, curInode);
	if (strcmp(inode.name, "/") == 0) {
		printf("/");
		return;
	}

	char *data = (char *)malloc(inode.size);
	ReadData(fp, curInode, data, inode.size);
	struct Directory *dirArray = (struct Directory *)data;
	PrintPathOfInode(fp, dirArray[1].ptrInode);

	printf("%s", inode.name);
	if (inode.type == 0) printf("/");
}

void AddLink(FILE *fp, int iInode, int x) {
	struct Inode inode = GetInode(fp, iInode);
	inode.linknum += x;
	WriteInode(fp, inode, iInode);
}

int DeleteInode(FILE *fp, int iInode) {
	struct Inode inode = GetInode(fp, iInode);
	if (inode.linknum == 0 && inode.type == 1
		|| inode.linknum == 1 && inode.type == 0) {
		PushFreeInode(fp, iInode);
		return 1;
	}
	return 0;
}

int EmptyDirectory(FILE *fp, int iInode) {
	struct Inode inode = GetInode(fp, iInode);
	if (inode.type == 0) {
		if (inode.size == 2 * sizeof(struct Directory)) {
			return 1;
		}
		else {
			printf("The directory is not empty!\n");
		}
	}
	else {
		printf("This directory does not exist!\n");
	}
	return 0;
}

void ReadData(FILE *fp, int pInode, char *data, int size) {
	//读取pInode指向的inode块数据
	struct Inode inode = GetInode(fp, pInode);

	int i;
	for (i = 0; i < ADDR0NUM; i++) {
		if (size == 0) return;
		int curDatablock = inode.addr0[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		int readSize = Min(size, DATABLOCKSIZE);
		fread(data, readSize, 1, fp);
		data += readSize;
		size -= readSize;
	}
	for (int i = 0; i < ADDR1NUM; i++) {
		if (size == 0) return;
		char *data1 = (char *)malloc(128 * sizeof(int));
		int curDatablock = inode.addr1[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fread(data1, DATABLOCKSIZE, 1, fp);
		int *addr = (int *)data1;
		for (int j = 0; j < NADDRS; ++j) {
			if (size == 0) return;
			if (addr[j] != -1) {
				fseek(fp, GetPosOfDatablock(addr[j]), SEEK_SET);
				int readSize = Min(size, DATABLOCKSIZE);
				fread(data, readSize, 1, fp);
				data += readSize;
				size -= readSize;
			}
		}
	}
	for (int i = 0; i < ADDR2NUM; i++) {
		if (size == 0) return;
		char *data1 = (char *)malloc(128 * sizeof(int));
		int curDatablock = inode.addr2[i];
		//printf("%d", curDatablock);
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fread(data1, DATABLOCKSIZE, 1, fp);
		int *addr = (int *)data1;
		for (int j = 0; j < NADDRS; ++j) {
			if (size == 0) return;
			if (addr[j] != -1) {
				char *data2 = (char *)malloc(128 * sizeof(int));
				fseek(fp, GetPosOfDatablock(addr[j]), SEEK_SET);
				fread(data2, DATABLOCKSIZE, 1, fp);
				int *addr1 = (int *)data2;
				for (int k = 0; k < NADDRS; ++k) {
					if (size == 0) return;
					if (addr1[k] != -1) {
						fseek(fp, GetPosOfDatablock(addr1[k]), SEEK_SET);
						int readSize = Min(size, DATABLOCKSIZE);
						fread(data, readSize, 1, fp);
						data += readSize;
						size -= readSize;
					}
				}
			}
		}
	}
}
void ReadData1(FILE *fp, int pInode, char *data, int size) {
	//读取pInode指向的inode块数据
	struct Inode inode = GetInode(fp, pInode);

	int i;
	/*for (i = 0; i < ADDR0NUM; i++) {
		if (size == 0) return;
		int curDatablock = inode.addr0[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		int readSize = Min(size, DATABLOCKSIZE);
		fread(data, readSize, 1, fp);
		data += readSize;
		size -= readSize;
	}*/
	//for (int i = 0; i < ADDR1NUM; i++) {
	//	if (size == 0) return;
	//	char *data1 = (char *)malloc(128*sizeof(int));
	//	int curDatablock = inode.addr1[i];
	//	fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
	//	fread(data1, DATABLOCKSIZE, 1, fp);
	//	int *addr = (int *)data1;
	//	//printf("%d %d\n", curDatablock, addr[0]);
	//	int naddr = 128;
	//	for (int i = 0; i < naddr; ++i) {
	//		if (size == 0) return;
	//		if (addr[i] != -1) {
	//			fseek(fp, GetPosOfDatablock(addr[i]), SEEK_SET);
	//			int readSize = Min(size, DATABLOCKSIZE);
	//			fread(data, readSize, 1, fp);
	//			data += readSize;
	//			size -= readSize;
	//		}
	//	}
	//}
	for (int i = 0; i < ADDR2NUM; i++) {
		if (size == 0) return;
		char *data1 = (char *)malloc(NADDRS * sizeof(int));
		int curDatablock = inode.addr2[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fread(data1, DATABLOCKSIZE, 1, fp);
		int *addr = (int *)data1;
		//printf("%d %d\n", curDatablock, addr[0]);
		int naddr = NADDRS;
		for (int i = 0; i < naddr; ++i) {
			if (size == 0) return;
			if (addr[i] != -1) {
				char *data2 = (char *)malloc(NADDRS * sizeof(int));
				fseek(fp, GetPosOfDatablock(addr[i]), SEEK_SET);
				fread(data2, DATABLOCKSIZE, 1, fp);
				int *addr1 = (int *)data2;
				for (int j = 0; j < naddr; ++j) {
					if (size == 0) return;
					//printf("\n%d\n", addr1[j]);
					if (addr1[j] != -1) {
						fseek(fp, GetPosOfDatablock(addr1[j]), SEEK_SET);
						int readSize = Min(size, DATABLOCKSIZE);
						fread(data, readSize, 1, fp);
						data += readSize;
						size -= readSize;
					}
				}
				
			}
		}
	}
}

void CleanData(FILE *fp, int iInode) {
	struct Inode inode = GetInode(fp, iInode);

	int i;
	for (i = 0; i < ADDR0NUM; i++) {
		if (inode.addr0[i] == -1) break;
		PushFreeDatablock(fp, inode.addr0[i]);
		inode.addr0[i] = -1;
	}
	for (i = 0; i < ADDR1NUM; i++) {
		if (inode.addr1[i] == -1) break;
		char *data1 = (char *)malloc(128 * sizeof(int));
		int curDatablock = inode.addr1[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fread(data1, DATABLOCKSIZE, 1, fp);
		int *addr = (int *)data1;
		int naddr = NADDRS;
		for (int i = 0; i < naddr; ++i) {
			if (addr[i] == -1) break;
			PushFreeDatablock(fp, addr[i]);
			addr[i] = -1;
		}
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fwrite(&addr, sizeof(addr), 1, fp);
		PushFreeDatablock(fp, inode.addr1[i]);
		inode.addr1[i] = -1;
	}
	for (i = 0; i < ADDR2NUM; i++) {
		if (inode.addr2[i] == -1) break;
		char *data2 = (char *)malloc(128 * sizeof(int));
		int curDatablock = inode.addr2[i];
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fread(data2, DATABLOCKSIZE, 1, fp);
		int *addr = (int *)data2;
		int naddr = NADDRS;
		for (int j = 0; j < NADDRS; j++) {
			if (addr[j] == -1) break;
			char *data3 = (char *)malloc(128 * sizeof(int));
			int curDatablock1 = addr[j];
			fseek(fp, GetPosOfDatablock(curDatablock1), SEEK_SET);
			fread(data3, DATABLOCKSIZE, 1, fp);
			int *addr1 = (int *)data3;
			for (int k = 0; k < NADDRS; ++k) {
				if (addr1[k] == -1) break;
				PushFreeDatablock(fp, addr1[k]);
				addr1[k] = -1;
			}
			fseek(fp, GetPosOfDatablock(curDatablock1), SEEK_SET);
			fwrite(&addr1, sizeof(addr1), 1, fp);
			PushFreeDatablock(fp, addr[j]);
			addr[j] = -1;
		}
		fseek(fp, GetPosOfDatablock(curDatablock), SEEK_SET);
		fwrite(&addr, sizeof(addr), 1, fp);
		PushFreeDatablock(fp, inode.addr2[i]);
		inode.addr2[i] = -1;
	}
	WriteInode(fp, inode, iInode);
}

void WriteData(FILE *fp, int pInode, void *vData, int size) {
	CleanData(fp, pInode);

	char *data = (char *)vData;

	//读取pInode指向的inode块数据
	struct Inode inode = GetInode(fp, pInode);
	inode.size = size;

	int i;
	for (i = 0; i < ADDR0NUM; i++) {
		if (size == 0) break;
		int writeSize = Min(size, DATABLOCKSIZE);
		int pDataBlock = GetFreeDatablock(fp);
		inode.addr0[i] = pDataBlock;
		fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
		fwrite(data, writeSize, 1, fp);
		data += writeSize;
		size -= writeSize;
	}
	for (i = 0; i < ADDR1NUM; ++i) {
		if (size == 0) break;
		int pDataBlock = GetFreeDatablock(fp);
		inode.addr1[i] = pDataBlock;
		int addr[128];
		for (int j = 0; j < NADDRS; ++j) {
			addr[j] = -1;
		}
		for (int j = 0; j < NADDRS; ++j) {
			if (size == 0) break;
			int writeSize = Min(size, DATABLOCKSIZE);
			int pDataBlock1 = GetFreeDatablock(fp);
			addr[j] = pDataBlock1;
			fseek(fp, GetPosOfDatablock(pDataBlock1), SEEK_SET);
			fwrite(data, writeSize, 1, fp);
			data += writeSize;
			size -= writeSize;
		}
		//printf("%d %d\n", pDataBlock, addr[0]);
		fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
		fwrite(&addr, sizeof(addr), 1, fp);
	}
	for (i = 0; i < ADDR2NUM; ++i) {
		if (size == 0) break;
		int pDataBlock = GetFreeDatablock(fp);
		inode.addr2[i] = pDataBlock;
		int addr[128];
		for (int j = 0; j < NADDRS; ++j) {
			addr[j] = -1;
		}
		for (int j = 0; j < NADDRS; ++j) {
			if (size == 0) break;
			int pDataBlock1 = GetFreeDatablock(fp);
			addr[j] = pDataBlock1;
			int addr1[128];
			for (int k = 0; k < NADDRS; ++k) {
				addr1[k] = -1;
			}
			for (int k = 0; k < NADDRS; ++k) {
				if (size == 0) break;
				int writeSize = Min(size, DATABLOCKSIZE);
				int pDataBlock2 = GetFreeDatablock(fp);
				addr1[k] = pDataBlock2;
				fseek(fp, GetPosOfDatablock(pDataBlock2), SEEK_SET);
				fwrite(data, writeSize, 1, fp);
				data += writeSize;
				size -= writeSize;
			}
			fseek(fp, GetPosOfDatablock(pDataBlock1), SEEK_SET);
			fwrite(&addr1, sizeof(addr), 1, fp);
		}
		fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
		fwrite(&addr, sizeof(addr), 1, fp);
	}
	time_t t = time(0);
	strftime(inode.time, sizeof(inode.time), "%Y/%m/%d %X %A %jday %z", localtime(&t));
	WriteInode(fp, inode, pInode);
}

void WriteData1(FILE *fp, int pInode, void *vData, int size) {
	CleanData(fp, pInode);

	char *data = (char *)vData;

	//读取pInode指向的inode块数据
	struct Inode inode = GetInode(fp, pInode);
	inode.size = size;

	int i;
	/*for (i = 0; i < ADDR0NUM; i++) {
	if (size == 0) break;
	int writeSize = Min(size, DATABLOCKSIZE);
	int pDataBlock = GetFreeDatablock(fp);
	inode.addr0[i] = pDataBlock;
	fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
	fwrite(data, writeSize, 1, fp);
	data += writeSize;
	size -= writeSize;
	}*/
	//for (i = 0; i < ADDR1NUM; ++i) {
	//	if (size == 0) break;
	//	int pDataBlock = GetFreeDatablock(fp);
	//	inode.addr1[i] = pDataBlock;
	//	int addr[128];
	//	for (int j = 0; j < 128; ++j) {
	//		addr[j] = -1;
	//	}
	//	for (int j = 0; j < 128; ++j) {
	//		if (size == 0) break;
	//		int writeSize = Min(size, DATABLOCKSIZE);
	//		int pDataBlock1 = GetFreeDatablock(fp);
	//		addr[j] = pDataBlock1;
	//		fseek(fp, GetPosOfDatablock(pDataBlock1), SEEK_SET);
	//		fwrite(data, writeSize, 1, fp);
	//		data += writeSize;
	//		size -= writeSize;
	//	}
	//	//printf("%d %d\n", pDataBlock, addr[0]);
	//	fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
	//	fwrite(&addr, sizeof(addr), 1, fp);
	//}
	for (i = 0; i < ADDR2NUM; ++i) {
		if (size == 0) break;
		int pDataBlock = GetFreeDatablock(fp);
		inode.addr2[i] = pDataBlock;
		int addr[NADDRS];
		for (int j = 0; j < NADDRS; ++j) {
			addr[j] = -1;
		}
		for (int j = 0; j < NADDRS; ++j) {
			if (size == 0) break;
			int pDataBlock1 = GetFreeDatablock(fp);
			addr[j] = pDataBlock1;
			int addr1[NADDRS];
			for (int j = 0; j < NADDRS; ++j) {
				addr1[j] = -1;
			}
			for (int k = 0; k < NADDRS; ++k) {
				if (size == 0) break;
				int writeSize = Min(size, DATABLOCKSIZE);
				int pDataBlock2 = GetFreeDatablock(fp);
				addr1[k] = pDataBlock2;
				fseek(fp, GetPosOfDatablock(pDataBlock2), SEEK_SET);
				fwrite(data, writeSize, 1, fp);
				data += writeSize;
				size -= writeSize;
			}
			fseek(fp, GetPosOfDatablock(pDataBlock1), SEEK_SET);
			fwrite(&addr1, sizeof(addr), 1, fp);
		}
		fseek(fp, GetPosOfDatablock(pDataBlock), SEEK_SET);
		fwrite(&addr, sizeof(addr), 1, fp);
	}
	time_t t = time(0);
	strftime(inode.time, sizeof(inode.time), "%Y/%m/%d %X %A %jday %z", localtime(&t));
	WriteInode(fp, inode, pInode);
}

