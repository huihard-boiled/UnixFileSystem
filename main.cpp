#include <cstdio>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "fs.h"
#include "user.h"
#include "file.h"
#include "inode.h"
#include "superblock.h"
extern int UMARK;
using namespace std;
#define CMDLEN 256
#define FILESYSTEMNAME "fs.bin"

char cmd[CMDLEN];
char maincmd[CMDLEN];
char argument[CMDLEN];
char username[USERNAMELEN];
char password[PASSWORDLEN];
char directoryName[FILENAMELEN];
int curDir;
char context[70000];

int main() {
	FILE *fp;

	fp = fopen(FILESYSTEMNAME, "rb+");

	//判断文件系统是否存在
	if (fp == NULL) {
		printf("Filesystem is not existing!\n");
		CreateFilesystem(FILESYSTEMNAME);
		fp = fopen(FILESYSTEMNAME, "rb+");
	}
	//判断用户名，密码是否正确
	while (1) {
		printf("Please input your username: ");
		scanf("%s", username);
		printf("Please input your password: ");
		scanf("%s", password);
		if (JudgeUserOk(fp, username, password)) {
			break;
		}
		else {
			printf("Wrong username or password!\n");
		}
	}
	//加载目录到用户目录
	if (strcmp(username, "root") == 0) {
		curDir = GetInodeOfName(fp, -1, "/root/");
	}
	else {
		directoryName[0] = '\0';
		strcat(directoryName, "/home/");
		strcat(directoryName, username);
		strcat(directoryName, "/");
		curDir = GetInodeOfName(fp, -1, directoryName);
	}
	while (1) {
		printf("%s@fs:", username);
		PrintPathOfInode(fp, curDir);
		strcmp(username, "root") == 0 ? putchar('#') : putchar('$');
		putchar(' ');
		scanf("%s", maincmd);
		if (strcmp(maincmd, "ls") == 0) {
			ListDirectory(fp, curDir, "");
		}
		else if (strcmp(maincmd, "mkdir") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			if (LegalFilename(a)) {
				strcat(a, "/");
				CreateDirectory(fp, curDir, a, username, GetGroup(fp, username));
			}
		}
		else if (strcmp(maincmd, "pwd") == 0) {
			PrintPathOfInode(fp, curDir);
			printf("\n");
		}
		else if (strcmp(maincmd, "cd") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			int iInode;
			if (a[0] == '/') iInode = GetInodeOfName(fp, -1, a);
			else iInode = GetInodeOfName(fp, curDir, a);

			if (iInode != -1) {
				struct Inode inode = GetInode(fp, iInode);
				if (inode.type == 0) {
					curDir = iInode;
				}
				else {
					printf("This is not a directory!\n");
				}
			}
		}
		else if (strcmp(maincmd, "chmod") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1,a2);
			int pri;
			if (sscanf(a1, "%o", &pri) == 1 && 0 <= pri && pri <= 0777) {
				int iInode;

				if (a2[0] == '/') iInode = GetInodeOfName(fp, -1, a2);
				else iInode = GetInodeOfName(fp, curDir, a2);

				//printf("inode = %d\n", inode);

				if (iInode != -1) {
					struct Inode inode = GetInode(fp, iInode);
					inode.permission = pri;
					WriteInode(fp, inode, iInode);
				}
				else {
					printf("Not Found!\n");
				}
			}
		}
		else if (strcmp(maincmd, "chgrp") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1, a2);
			int iInode;
			if (a2[0] == '/') iInode = GetInodeOfName(fp, -1, a2);
			else iInode = GetInodeOfName(fp, curDir, a2);

			if (iInode != -1) {
				if (AllowWrite(fp, iInode, username)) {
					struct Inode inode = GetInode(fp, iInode);
					strcpy(inode.group, a1);
					WriteInode(fp, inode, iInode);
				}
				else {
					printf("You are not allowed to change this file!\n");
				}
			}
		}
		else if (strcmp(maincmd, "chown") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1, a2);
			if (UserExist(fp, a1)) {
				int iInode;
				if (a2[0] == '/') iInode = GetInodeOfName(fp, -1, a2);
				else iInode = GetInodeOfName(fp, curDir, a2);

				if (iInode != -1) {
					if (AllowWrite(fp, iInode, username)) {
						struct Inode inode = GetInode(fp, iInode);
						strcpy(inode.owner, a1);
						WriteInode(fp, inode, iInode);
					}
					else {
						printf("You are not allowed to change this file!\n");
					}
				}
			}
			else {
				printf("User does not exist!\n");
			}
		}
		else if (strcmp(maincmd, "rmdir")==0) {
			char a[CMDLEN];
			scanf("%s", a);
			if (LegalFilename(a)) {
				int dstInode = GetInodeOfName(fp, curDir, a);
				if (dstInode != -1) {
					if (EmptyDirectory(fp, dstInode)) {
						AddLink(fp, dstInode, -1);
						AddLink(fp, curDir, -1);
						DeleteInode(fp, dstInode);

						struct Inode inode = GetInode(fp, curDir);
						char *data = (char *)malloc(inode.size);
						ReadData(fp, curDir, data, inode.size);

						struct Directory *dirArray = (struct Directory *)data;
						int nDir = inode.size / sizeof(struct Directory);
						int i, j;
						for (i = 0; i < nDir; i++) {
							if (strcmp(dirArray[i].dirName, a) == 0) {
								for (j = i; j < nDir - 1; j++) {
									dirArray[j] = dirArray[j + 1];
								}
								break;
							}
						}

						WriteData(fp, curDir, dirArray, inode.size - sizeof(struct Directory));
					}
				}
			}
			else {
				printf("Invalid directory!\n");
			}
		}
		else if (strcmp(maincmd, "mv") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1, a2);
			if (LegalFilename(a1) && LegalFilename(a2)) {
				if (a1[strlen(a1) - 1] == '/') a1[strlen(a1) - 1] = '\0';
				if (a2[strlen(a2) - 1] == '/') a2[strlen(a2) - 1] = '\0';
				int iInode = GetInodeOfName(fp, curDir, a1);
				if (iInode == -1) {
					printf("File %s does not exist!\n", a1);
				}
				else {
					int newInode = GetInodeOfNameWithoutError(fp, curDir, a2);
					if (newInode != -1) {
						printf("File %s exists!\n", a2);
					}
					else {
						struct Inode inode = GetInode(fp, iInode);
						strcpy(inode.name, a2);
						WriteInode(fp, inode, iInode);

						struct Inode cinode = GetInode(fp, curDir);
						char *data = (char *)malloc(cinode.size);
						ReadData(fp, curDir, data, cinode.size);

						struct Directory *dirArray = (struct Directory *)data;
						int nDir = cinode.size / sizeof(struct Directory);

						int i;
						for (i = 0; i < nDir; i++) {
							if (strcmp(dirArray[i].dirName, a1) == 0) {
								strcpy(dirArray[i].dirName, a2);
								break;
							}
						}
						WriteData(fp, curDir, dirArray, cinode.size);
					}
				}
			}
			else {
				printf("Invalid command!\n");
			}
		}
		else if (strcmp(maincmd, "rm") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			if (a[strlen(a) - 1] != '\0' && LegalFilename(a)) {
				int dstInode = GetInodeOfName(fp, curDir, a);
				if (dstInode != -1) {
					struct Inode tmpInode = GetInode(fp, dstInode);
					if (tmpInode.type != 0) {

						CleanData(fp, dstInode);

						AddLink(fp, dstInode, -1);
						DeleteInode(fp, dstInode);

						struct Inode inode = GetInode(fp, curDir);
						char *data = (char *)malloc(inode.size);
						ReadData(fp, curDir, data, inode.size);

						struct Directory *dirArray = (struct Directory *)data;
						int nDir = inode.size / sizeof(struct Directory);
						int i, j;
						for (i = 0; i < nDir; i++) {
							if (strcmp(dirArray[i].dirName, a) == 0) {
								for (j = i; j < nDir - 1; j++) {
									dirArray[j] = dirArray[j + 1];
								}
								break;
							}
						}

						WriteData(fp, curDir, dirArray, inode.size - sizeof(struct Directory));
					}
					else {
						printf("Cannot delete a directory!\n");
					}
				}
			}
			else {
				printf("Invalid directory!\n");
			}
		}
		else if (strcmp(maincmd, "cp") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1, a2);
			if (LegalFilename(a2)) {
				if (a1[strlen(a1) - 1] == '/') a1[strlen(a1) - 1] = '\0';
				if (a2[strlen(a2) - 1] == '/') a2[strlen(a2) - 1] = '\0';
				int iInode = GetInodeOfName(fp, curDir, a1);
				if (iInode == -1) {
					printf("File %s does not exist!\n", a1);
				}
				else {
					int newInode = GetInodeOfNameWithoutError(fp, curDir, a2);
					if (newInode != -1) {
						printf("File %s exists!\n", a2);
					}
					struct Inode inode = GetInode(fp, iInode);
					if (inode.type == 0) {
						printf("Cannot copy directory!\n");
					}
					else {
						char *data = (char *)alloca(inode.size);
						ReadData(fp, iInode, data, inode.size);
						int dstInode = CreateFile(fp, curDir, a2, username, GetGroup(fp, username));
						WriteData(fp, dstInode, data, inode.size);
					}
				}
			}
		}
		else if (strcmp(maincmd, "ln") == 0) {
			char a1[CMDLEN], a2[CMDLEN];
			scanf("%s %s", a1, a2);
			if (a2[strlen(a2) - 1] != '/' && LegalFilename(a2)) {
				int iInode;
				if (a1[0] == '/') iInode = GetInodeOfNameWithoutError(fp, -1, a1);
				else iInode = GetInodeOfNameWithoutError(fp, curDir, a1);

				if (iInode == -1) {
					printf("File %s does not exists!\n", a1);
				}
				else {
					struct Inode inode = GetInode(fp, iInode);
					if (inode.type == 0) {
						printf("It is a directory!\n");
					}
					else {
						struct Inode inodeDir = GetInode(fp, curDir);
						char *dirData = (char *)malloc(inodeDir.size + sizeof(struct Directory));
						ReadData(fp, curDir, dirData, inodeDir.size);

						struct Directory newItem;
						strcpy(newItem.dirName, a2);
						newItem.ptrInode = iInode;

						memcpy(dirData + inodeDir.size, &newItem, sizeof(struct Directory));

						inodeDir.size += sizeof(struct Directory);
						WriteInode(fp, inodeDir, curDir);
						WriteData(fp, curDir, dirData, inodeDir.size);

						AddLink(fp, iInode, 1);
					}
				}
			}
		}
		else if (strcmp(maincmd, "stat") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			if (a[0] == '/') {
				int dstInode = GetInodeOfName(fp, -1, a);
				struct Inode inode = GetInode(fp, dstInode);
				PrintInode(inode);
			}
			else {
				int dstInode = GetInodeOfName(fp, curDir, a);
				struct Inode inode = GetInode(fp, dstInode);
				PrintInode(inode);
			}
		}
		else if (strcmp(maincmd, "touch") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			if (a[strlen(a) - 1] != '/' && LegalFilename(a)) {
				int iInode = GetInodeOfNameWithoutError(fp, curDir, a);
				if (iInode != -1) {
					printf("File %s exists!\n", a);
				}else {
					CreateFile(fp, curDir, a, username, GetGroup(fp, username));
				}
			}else {
				printf("Usage: touch filename\n");
			}
		}
		else if (strcmp(maincmd, "edit") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			int iInode;
			if (a[0] == '/') iInode = GetInodeOfName(fp, -1, a);
			else iInode = GetInodeOfName(fp, curDir, a);

			if (iInode != -1) {
				getchar();
				gets_s(context);
				WriteFile(fp, iInode, context, strlen(context), username);
			}
		}
		else if (strcmp(maincmd, "cat") == 0) {
			char a[CMDLEN];
			scanf("%s", a);
			int iInode;
			if (a[0] == '/') iInode = GetInodeOfName(fp, -1, a);
			else iInode = GetInodeOfName(fp, curDir, a);

			if (iInode != -1) {
				struct Inode inode = GetInode(fp, iInode);
				if (inode.type == 1) {
					PrintFile(fp, iInode, username);
				}
				else {
					printf("It is not a text file\n");
				}
			}
		}
		else if (strcmp(maincmd, "passwd") == 0) {
			char passwd[PASSWORDLEN];
			getchar();
			printf("Please input old password: ");
			gets_s(passwd);
			while (!JudgeUserOk(fp, username, passwd)) {
				printf("Wrong password!\n");
				printf("Please input old password: ");
				gets_s(passwd);
			}

			printf("Please input new password: ");
			gets_s(passwd);
			while (!LegalPassword(passwd)) {
				printf("Please input new password: ");
				gets_s(passwd);
			}
			ChangePassword(fp, username, passwd);
		}
		else if (strcmp(maincmd, "exit") == 0) {
			break;
		}
		else if (strcmp(maincmd, "umask") == 0) {
			char a[CMDLEN];
			gets_s(a);
			if (strlen(a) == 0) {
				printf("%04o\n",UMARK);
			}
			else {
				sscanf(a, "%o", &UMARK);
			}
		}
		else if (strcmp(maincmd, "show") == 0) {
			showSuperblock(fp);
		}
		else if (strcmp(maincmd, "help") == 0) {
			printf("ls [directory]\n");
			printf("stat [file]\n");
			printf("chmod chmod privilege file\n");
			printf("chown owner file\n");
			printf("chgrp group file\n");
			printf("pwd\n");
			printf("cd directory\n");
			printf("mkdir subdirectory\n");
			printf("rmdir emptySubdirectory\n");
			printf("mv srcFile dstFile\n");
			printf("cp srcFile dstFile\n");
			printf("rmdir file\n");
			printf("ln srcFile dstFile\n");
			printf("cat filename\n");
			printf("passwd\n");
			printf("touch filename\n");
			printf("edit filename\n");
			printf("umask\n");
			printf("umask number\n");
			printf("exit\n");
		}
		else {
			printf("Invalid command!\n");
		}
	}

	fclose(fp);
}