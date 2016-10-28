/*
Created on Oct 27, 2016
@author: Ye Bai
*/
#pragma once
#include "huffmanCode.h"
Byte rawBuf;//Buff storing a byte of file.
Byte rbuf;//Efficient bits to handle when decoding.
Byte wbuf;
bool bit;//a bit to decode.
bool fileEnd(false);
Byte alignBit;//The number of alignBit.
int rBitN;//The number of efficient bits in rbuf.


void encode(FILE *fp,FILE *out)
{
	int freq[NSYMBOLS] = { 0 };

	while (fread(&rbuf,sizeof(Byte),1,fp))
	{
		freq[rbuf]++;
	}
	rbuf = 0;
	//make codebook
	BiNode* hft = buildHuffTree(freq);
	HuffCode curr;
	curr.clear();
	if (hft->isLeaf())
	{
		curr.push_back(true);
	}
	mkCodeBook(hft, curr);
	removeTree(hft);

	
	int posMaskFlag = 0; //To represent alignment at the end of file. 
	int codeBookSize = codebook.size();
	posMaskFlag += sizeof(codeBookSize);
	fwrite(&codeBookSize, sizeof(codeBookSize), 1, out);
	Byte ch = 0;
	for (int i = 0; i < NSYMBOLS; i++)
	{		
		if (freq[ch] > 0)
		{
			fwrite(&ch, sizeof(ch), 1, out);
			fwrite(&freq[ch], sizeof(int), 1, out);
			posMaskFlag += (sizeof(ch) + sizeof(freq[ch]));
		}
		ch++;
	}
	ch = '^';
	fwrite(&ch, sizeof(Byte), 1, out);
	

	//compress
	fseek(fp, 0, 0);
	Byte buff(0);
	int  bitPos = 0;

	fread(&rbuf, sizeof(Byte), 1, fp);
	for (HuffCode::iterator it = codebook[rbuf].begin();
		it != codebook[rbuf].end(); it++)
	{
		//printf("%d", (int)(*it));
		if ((*it))
		{
			buff |= mask[bitPos];
		}
		bitPos++;
		if (bitPos == NBYTE)
		{
			fwrite(&buff, sizeof(buff), 1, out);
			bitPos = 0;
			buff = 0;
		}
	}
	while (true)
	{
		if (!fread(&rbuf, sizeof(Byte), 1, fp))
		{
			fwrite(&buff, sizeof(buff), 1, out);
			break;
		}
		else
		{
			for (HuffCode::iterator it = codebook[rbuf].begin();
				it != codebook[rbuf].end(); it++)
			{
				//printf("%d", (int)(*it));
				if ((*it))
				{
					buff |= mask[bitPos];
				}
				bitPos++;
				if (bitPos == NBYTE)
				{
					fwrite(&buff, sizeof(buff), 1, out);
					bitPos = 0;
					buff = 0;
				}
			}
		}
	}
	
	if (bitPos > 0)
	{
		ch = 8 - bitPos;
		fseek(out, posMaskFlag, 0);
		fwrite(&ch, sizeof(Byte), 1, out);
	}
	
	return;
}

Byte decodeHuff(BiNode* hft, FILE *fp)
{
	if (hft->isLeaf())
	{
		return hft->getData();
	}
	if (rBitN == 0)
	{
		rbuf = rawBuf;
		if (!fread(&rawBuf, sizeof(Byte), 1, fp))
		{
			rBitN = NBYTE - alignBit;
			fileEnd = true;
		}
		else
		{
			rBitN = NBYTE;
		}
	}

	bit = rbuf & mask[0];
	rbuf = rbuf << 1;
	rBitN--;

	if (bit == true)
	{
		decodeHuff(hft->getLeft(), fp);
	}
	else
	{
		decodeHuff(hft->getRight(), fp);
	}
}

void decode(FILE *fp, FILE *out)
{
	int codeBookSize;
	fread(&codeBookSize, sizeof(int), 1, fp);
	int freq[NSYMBOLS] = { 0 };
	for (int i = 0; i < codeBookSize; i++)
	{
		Byte ch;
		int fre;
		fread(&ch, sizeof(Byte), 1, fp);
		fread(&fre, sizeof(int), 1, fp);
		freq[ch] = fre;
	}

	
	fread(&alignBit, sizeof(Byte), 1, fp);

	BiNode* hft = buildHuffTree(freq);

	Byte ch;
	fread(&rawBuf, sizeof(Byte), 1, fp);
	rBitN = 0;

	while (!fileEnd || rBitN != 0)
	{
		ch = decodeHuff(hft, fp);
		wbuf = ch;
		fwrite(&wbuf, sizeof(Byte), 1, out);
	}
	return;
}