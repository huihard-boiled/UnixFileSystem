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
	char username[LEGALUSERNAMELEN]; //�û���
	char password[LEGALPASSWORDLEN]; //����
	char group[LEGALGROUPLEN]; //�û�������
};

#define USERSIZE sizeof(struct User)

//�����û�
struct User CreateUser(const char username[], const char password[], const char group[]);

//�ж��û����Ƿ�Ϸ�
int LegalUsername(char username[]);

//�ж������Ƿ�Ϸ�
int LegalPassword(char password[]);

//��ȡ�û�������
char *GetGroup(FILE *fp, char username[]);

//�ж��û��������Ƿ���ȷ
int JudgeUserOk(FILE *fp, char username[], char password[]);

//�ж��û��Ƿ����
int UserExist(FILE *fp, char username[]);

//�޸�����
void ChangePassword(FILE *fp, char username[], char password[]);

#endif
