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
	char username[LEGALUSERNAMELEN]; 
	char password[LEGALPASSWORDLEN]; 
	char group[LEGALGROUPLEN];
};

#define USERSIZE sizeof(struct User)


struct User CreateUser(const char username[], const char password[], const char group[]);


int LegalUsername(char username[]);


int LegalPassword(char password[]);


char *GetGroup(FILE *fp, char username[]);


int JudgeUserOk(FILE *fp, char username[], char password[]);


int UserExist(FILE *fp, char username[]);


void ChangePassword(FILE *fp, char username[], char password[]);

#endif
