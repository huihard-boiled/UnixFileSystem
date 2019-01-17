#include <stdio.h>

#include "inode.h"
#include "superblock.h"
#include "datablock.h"

int GetPosOfInode(int index) {
	return SUPERBLOCKSIZE + index * INODESIZE;
}

struct Inode GetInode(FILE *fp, int p) {
	struct Inode inode;
	fseek(fp, GetPosOfInode(p), SEEK_SET);
	fread(&inode, sizeof(inode), 1, fp);
	return inode;
}

void WriteInode(FILE *fp, struct Inode inode, int p) {
	fseek(fp, GetPosOfInode(p), SEEK_SET);
	fwrite(&inode, sizeof(inode), 1, fp);
}

void PrintInode(struct Inode inode) {
	int i;

	printf("inode:\n");
	printf("    name = %s\n", inode.name);
	printf("    size = %d\n", inode.size);
	printf("    linkNum = %d\n", inode.linknum);
	printf("    type = %d\n", inode.type);
	printf("    owner = %s\n", inode.owner);
	printf("    group = %s\n", inode.group);
	printf("    permission = %o\n", inode.permission);
	printf("    time = %s\n", inode.time);
	printf("    addr0:");
	for (i = 0; i < ADDR0NUM; i++) printf(" %d", inode.addr0[i]);
	printf("\n");
	printf("    addr1:");
	for (i = 0; i < ADDR1NUM; i++) printf(" %d", inode.addr1[i]);
	printf("\n");
	printf("    addr2:");
	for (i = 0; i < ADDR2NUM; i++) printf(" %d", inode.addr2[i]);
	printf("\n");
}

void InitFreeInode(FreeInode *freeInode) {
	int i;
	freeInode->num = INODENUM;
	for (i = 0; i < INODENUM; i++) {
		freeInode->stack[i] = i;
	}
}

int GetFreeInode(FILE *fp) {
	struct Superblock superblock = GetSuperblock(fp);
	if (superblock.freeInode.num == 0) {
		printf("Inode is not enough!\n");
		return -1;
	}
	superblock.freeInode.num--;
	int inodeNmuber = superblock.freeInode.stack[superblock.freeInode.num];
	superblock.freeInode.stack[superblock.freeInode.num] = 0;
	WriteSuperblock(fp, superblock);
	//printf("%d\n", inodeNmuber);
	return inodeNmuber;
}

void PushFreeInode(FILE *fp, int index) {
	struct Superblock superblock = GetSuperblock(fp);
	superblock.freeInode.stack[superblock.freeInode.num] = index;
	superblock.freeInode.num++;
	WriteSuperblock(fp, superblock);
}