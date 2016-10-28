/*
Created on Oct 27, 2016
@author: Ye Bai
*/
#pragma once
#include <vector>
#include <map>
#include <queue>
#include <algorithm>

using std::queue;
using std::vector;
using std::map;
using std::priority_queue;
typedef vector<bool> HuffCode;
typedef unsigned char Byte;
typedef map<Byte, HuffCode> HuffDict;

const int NBYTE = 8;
const int NSYMBOLS = 256;

const Byte mask[8] =
{
	0x80, /* 10000000 */
	0x40, /* 01000000 */
	0x20, /* 00100000 */
	0x10, /* 00010000 */
	0x08, /* 00001000 */
	0x04, /* 00000100 */
	0x02, /* 00000010 */
	0x01  /* 00000001 */
};

class BiNode {
private:
	bool internal_;
	Byte data_;
	int weight_;

	BiNode *father_;
	BiNode *left_;
	BiNode *right_;
public:
	BiNode():
		data_(0),weight_(-1),father_(nullptr),left_(nullptr),right_(nullptr), internal_(false){}
	BiNode(int w,BiNode *l,BiNode *r) :
		data_(0), weight_(w), father_(nullptr), left_(l), right_(r), internal_(false) {}
	BiNode(Byte d,int w):
		data_(d), weight_(w), father_(nullptr), left_(nullptr), right_(nullptr), internal_(false) {}
	
	void setFather(BiNode *f) { father_ = f; }
	void setWeight(int w) { weight_ = w; }
	void setData(Byte d) { data_ = d; }
	BiNode* getFather() const{ return father_; }
	BiNode* getLeft() const{ return left_; }
	BiNode* getRight() const { return right_; }
	Byte getData() const { return data_; }
	int getWeight() const { return weight_; }
	bool operator<(const BiNode& rhs)
	{
		return weight_ > rhs.getWeight(); // To use priority queue as minimum queue.
	}
	bool isLeaf() const { return left_ == nullptr&&right_ == nullptr; }
};

class CmpTree {
public:
	bool operator()(BiNode *a, BiNode *b)
	{
		return *a < *b;
	}

};

BiNode *combineTree(BiNode *a, BiNode *b)
{
	int w = a->getWeight() + b->getWeight();
	BiNode *root = new BiNode(w,a,b);
	a->setFather(root);
	b->setFather(root);
	return root;
}

void removeTree(BiNode *root)
{
	if (root == nullptr)
	{
		return;
	}
	BiNode *l = root->getLeft();
	BiNode *r = root->getRight();
	delete root;
	removeTree(l);
	removeTree(r);
}

BiNode *buildHuffTree(int freq[])
{
	priority_queue<BiNode*, vector<BiNode*>, CmpTree> pq;
	for (int i = 0; i < NSYMBOLS; i++)
	{
		if (freq[i] == 0)
		{
			continue;
		}
		pq.push(new BiNode((Byte)i,freq[i]));
	}
	while (true)
	{
		if (pq.size() == 1)
		{
			BiNode *root = pq.top();
			return root;
		}
		else
		{
			BiNode *tr1 = pq.top();
			pq.pop();
			BiNode *tr2 = pq.top();
			pq.pop();
			BiNode *newTree = combineTree(tr1, tr2);
			pq.push(newTree);
		}
	}
}

HuffDict codebook;
void mkCodeBook(BiNode *hfTree, HuffCode &currcode)
{
	if (hfTree->isLeaf())
	{
		codebook[hfTree->getData()] = currcode;
		return;
	}

	HuffCode lcode = currcode;
	HuffCode rcode = currcode;
	
	lcode.push_back(true);
	mkCodeBook(hfTree->getLeft(), lcode);
	rcode.push_back(false);
	mkCodeBook(hfTree->getRight(), rcode);
}


