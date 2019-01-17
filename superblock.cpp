#include <stdio.h>

#include "superblock.h"

void InitSuperblock(struct Superblock *superblock) {
	superblock->ptrRoot = -1;
	InitFreeInode(&superblock->freeInode);
	superblock->block[0] = 100;
	for (int i = 100; i > 0; i--) {
		superblock->block[101-i] = i;
	}
}

int GetPosOfSuperblock() {
	return 0;
}

struct Superblock GetSuperblock(FILE *fp) {
	struct Superblock superblock;
	fseek(fp, 0, SEEK_SET);
	fread(&superblock, sizeof(superblock), 1, fp);
	return superblock;
}

void WriteSuperblock(FILE *fp, struct Superblock superblock) {
	fseek(fp, GetPosOfSuperblock(), SEEK_SET);
	fwrite(&superblock, SUPERBLOCKSIZE, 1, fp);
}

void SetPtrRoot(FILE *fp, int p) {
	//¶ÁÈ¡³¬¼¶¿é
	fseek(fp, GetPosOfSuperblock(), SEEK_SET);
	struct Superblock superblock;
	fread(&superblock, SUPERBLOCKSIZE, 1, fp);

	//ÐÞ¸Ä¸ùÄ¿Â¼Ö¸Õë
	superblock.ptrRoot = p;

	//Ð´Èë³¬¼¶¿é
	WriteSuperblock(fp, superblock);
}

void showSuperblock(FILE *fp) {
	struct Superblock superblock = GetSuperblock(fp);
	printf("¿ÕÏÐiNode¿é£º%d\n", superblock.freeInode.num);
	for (int i = 0; i < INODENUM; ++i) {
		printf("%d ",superblock.freeInode.stack[i]);
	}
	printf("\n¿ÕÏÐblock¿é£º%d\n",superblock.block[0]);
	for (int i = 1; i < 101; ++i) {
		printf("%d ", superblock.block[i]);
	}
	printf("\n");
}