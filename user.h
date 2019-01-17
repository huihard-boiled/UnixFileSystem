#ifndef USER_H
#define USER_H

#include <stdio.h>

#define USERNAMELEN 256
#define LEGALUSERNAMELEN 16
#define PASSWORDLEN 256
#define LEGALPASSWORDLEN 16
#define GROUPLEN 256
#define LEGALGROUPLEN 16

struct User {
	char username[LEGALUSERNAMELEN]; //用户名
	char password[LEGALPASSWORDLEN]; //密码
	char group[LEGALGROUPLEN]; //用户所在组
};

#define USERSIZE sizeof(struct User)

//创建用户
struct User CreateUser(const char username[], const char password[], const char group[]);

//判断用户名是否合法
int LegalUsername(char username[]);

//判断密码是否合法
int LegalPassword(char password[]);

//获取用户所在组
char *GetGroup(FILE *fp, char username[]);

//判断用户名密码是否正确
int JudgeUserOk(FILE *fp, char username[], char password[]);

//判断用户是否存在
int UserExist(FILE *fp, char username[]);

//修改密码
void ChangePassword(FILE *fp, char username[], char password[]);

#endif
