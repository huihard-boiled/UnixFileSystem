#include <stdio.h>

#include "datablock.h"
#include "superblock.h"
#include "inode.h"

int GetPosOfDatablock(int index) {
	return SUPERBLOCKSIZE + INODENUM * INODESIZE + index * DATABLOCKSIZE;
}

//int GetPosOfFreeDatablock() {
//	return SUPERBLOCKSIZE + FREEINODESIZE;
//}

//void InitFreeDatablock(FILE *fp) {
//	int i;
//	struct FreeDatablock freeDatablock;
//	freeDatablock.num = DATABLOCKNUM;
//	for (i = 0; i < DATABLOCKNUM; i++) {
//		freeDatablock.stack[i] = i;
//	}
//
//	fseek(fp, GetPosOfFreeDatablock(), SEEK_SET);
//	fwrite(&freeDatablock, FREEDATABLOCKSIZE, 1, fp);
//}

int GetFreeDatablock(FILE *fp) {
	struct Superblock superblock = GetSuperblock(fp);
	if (superblock.block[0] == 1 && superblock.block[1] == 0) return -1;
	if (superblock.block[0] > 1) {
		int blockNumber = superblock.block[superblock.block[0]];
		superblock.block[superblock.block[0]] = 0;
		superblock.block[0]--;
		WriteSuperblock(fp, superblock);
		return blockNumber;
	}
	else {
		int newblock[101];
		fseek(fp, GetPosOfDatablock(superblock.block[1]), SEEK_SET);
		fread(&newblock, sizeof(newblock), 1, fp);
		for (int i = 0; i < 101; ++i) {
			superblock.block[i] = newblock[i];
		}
		WriteSuperblock(fp, superblock);
		return GetFreeDatablock(fp);
	}
}

//添加一个数据块
void PushFreeDatablock(FILE *fp, int index) {
	struct Superblock superblock = GetSuperblock(fp);
	if (superblock.block[0] < 100) {
		superblock.block[superblock.block[0] + 1] = index;
		superblock.block[0]++;
	}
	else {
		int newblock[101] = { 0 };
		newblock[0] = 2;
		newblock[1] = superblock.block[1] - 100;
		newblock[2] = index;
		for (int i = 0; i < 101; ++i) {
			superblock.block[i] = newblock[i];
		}
	}
	
	WriteSuperblock(fp, superblock);
}