#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "file.h"
#include "inode.h"

struct User CreateUser(const char username[], const char password[], const char group[]) {
	struct User user;
	strcpy(user.username, username);
	strcpy(user.password, password);
	strcpy(user.group, group);
	return user;
}

int LegalUsername(char username[]) {
	int i;
	if (strlen(username) >= LEGALUSERNAMELEN) {
		printf("The length of username must be less than %d\n", LEGALUSERNAMELEN);
		return 0;
	}
	for (i = 0; username[i] != '\0'; i++) {
		char ch = username[i];
		if (!('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || '0' <= ch && ch <= '9')) {
			printf("Username can only contain latin characters and numbers!\n");
			return 0;
		}
	}
	return 1;
}

int LegalPassword(char password[]) {
	int i;
	if (strlen(password) >= LEGALPASSWORDLEN) {
		printf("The length of password must be less than %d\n", LEGALPASSWORDLEN);
		return 0;
	}
	for (i = 0; password[i] != '\0'; i++) {
		char ch = password[i];
		if (!(32 <= ch && ch <= 126)) {
			printf("Password cannot contain illegal characters!\n");
			return 0;
		}
	}
	return 1;
}

char *GetGroup(FILE *fp, char username[]) {
	int iInode = GetInodeOfName(fp, -1, "/sys/userinfo");
	struct Inode inode = GetInode(fp, iInode);
	char *data = (char *)malloc(inode.size);
	ReadData(fp, iInode, data, inode.size);

	struct User *userArray = (struct User *)data;
	int nUser = inode.size / sizeof(struct User);

	int i;
	for (i = 0; i < nUser; i++) {
		if (strcmp(userArray[i].username, username) == 0) {
			char *ans = (char *)malloc(LEGALGROUPLEN);
			strcpy(ans, userArray[i].group);
			return ans;
		}
	}
	return NULL;
}

int JudgeUserOk(FILE *fp, char username[], char password[]) {
	int iInode = GetInodeOfName(fp, -1, "/sys/userinfo");
	//printf("iInode = %d\n", iInode);
	struct Inode inode = GetInode(fp, iInode);
	//PrintInode(inode);
	char *data = (char *)malloc(inode.size);
	ReadData(fp, iInode, data, inode.size);

	struct User *userArray = (struct User *)data;
	int nUser = inode.size / sizeof(struct User);

	int i;
	for (i = 0; i < nUser; i++) {
		//printf("%s %s\n", userArray[i].username, userArray[i].password);
		if (strcmp(userArray[i].username, username) == 0) {
			if (strcmp(userArray[i].password, password) == 0) return 1;
			else return 0;
		}
	}

	return 0;
}

int UserExist(FILE *fp, char username[]) {
	int iInode = GetInodeOfName(fp, -1, "/sys/userinfo");
	//printf("iInode = %d\n", iInode);
	struct Inode inode = GetInode(fp, iInode);
	//PrintInode(inode);
	char *data = (char *)malloc(inode.size);
	ReadData(fp, iInode, data, inode.size);

	struct User *userArray = (struct User *)data;
	int nUser = inode.size / sizeof(struct User);

	int i;
	for (i = 0; i < nUser; i++) {
		//printf("%s %s\n", userArray[i].username, userArray[i].password);
		if (strcmp(userArray[i].username, username) == 0) {
			return 1;
		}
	}

	return 0;
}

void ChangePassword(FILE *fp, char username[], char password[]) {
	int iInode = GetInodeOfName(fp, -1, "/sys/userinfo");
	//printf("iInode = %d\n", iInode);
	struct Inode inode = GetInode(fp, iInode);
	//PrintInode(inode);
	char *data = (char *)malloc(inode.size);
	ReadData(fp, iInode, data, inode.size);

	struct User *userArray = (struct User *)data;
	int nUser = inode.size / sizeof(struct User);

	int i;
	for (i = 0; i < nUser; i++) {
		//printf("%s %s\n", userArray[i].username, userArray[i].password);
		if (strcmp(userArray[i].username, username) == 0) {
			strcpy(userArray[i].password, password);
			break;
		}
	}
	WriteData(fp, iInode, userArray, inode.size);
}