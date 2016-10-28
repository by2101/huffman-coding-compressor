/*
Created on Oct 27, 2016
@author: Ye Bai
*/
#define _CRT_SECURE_NO_WARNINGS
#include "CmprDecmp.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

void po()
{
	printf("Compresser\n\n");
	printf("Usage:");
	printf("compresser <c/d> <sourceFile> <destFile>\n");
	printf("The parameter [c] means compresssing,"
		" and the parameter [d] means decompressing.\n\n");
}

int main(int argc, char *argv[])
{
	size_t start, end;
	if (argc != 4)
	{
		po();
		return 0;
	}
	else
	{
		if (!strcmp(argv[1],"c"))
		{
			char srcFileName[105];
			char destFileName[105];
			strcpy(srcFileName, argv[2]);
			strcpy(destFileName, argv[3]);
			FILE* src = fopen(srcFileName, "rb");
			FILE* dest = fopen(destFileName, "wb");
			start = clock();
			encode(src, dest);
			end = clock();
			fclose(src);
			fclose(dest);
			printf("The file has been compressed!\n");
			printf("The time cost is %lfs.\n", ((double)(end - start)) / CLOCKS_PER_SEC);
			return 0;
		}
		else if (!strcmp(argv[1], "d"))
		{
			FILE* src = fopen(argv[2], "rb");
			FILE* dest = fopen(argv[3], "wb");
			start = clock();
			decode(src, dest);
			end = clock();
			fclose(src);
			fclose(dest);
			printf("The file has been decompressed!\n");
			printf("The time cost is %lfs.\n", ((double)(end - start)) / CLOCKS_PER_SEC);
			return 0;
		}
		else
		{
			po();
			return 0;
		}
	}
}