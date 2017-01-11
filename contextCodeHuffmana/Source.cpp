#define _CRT_SECURE_NO_WARNINGS
//#define STATIC_MATRIX

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <list>
#include <vector>
#include <bitset>

using namespace std;

const int sizeStream = 1000000;
const int sizeMatrixForMC = 5;

char *nameStreamFile = "stream.txt";
char *nameEncodingStreamFile = "encodingStream.txt";
char *nameDecodingFile = "decodingStream.txt";

struct node
{
	int count;
	char name;
	node *left, *right;
};

int maxNumOfCh;
node **root = new node *[maxNumOfCh];

struct compare
{
	bool operator()(const node* l, const node* r)
		const {
		return l->count < r->count;
	}
};

vector <bool> code;
map<char, vector<bool>> *codingTable;

void buildTable(node *root, map<char, vector<bool>> &codingTable)
{
	if (root->left != NULL)
	{
		code.push_back(0);
		buildTable(root->left, codingTable);
	}

	if (root->right != NULL)
	{
		code.push_back(1);
		buildTable(root->right, codingTable);
	}

	if (root->left == NULL && root->right == NULL)
	{
		codingTable[root->name] = code;
		printf("%d\t", root->name);
		for (int n = 0; n < code.size(); n++)
			cout << code[n];
		printf("\n");
	}

	if (code.size() != 0)
	{
		code.pop_back();
	}
	else
	{
		return;
	}
}
void getStreamWithMC()
{
	int maxIteration = 50;
	float randNumb;
	float countOfCol;
	bool stop;
	int count;
	int current;
	float comulP;

#ifndef STATIC_MATRIX

	float matrixMC[sizeMatrixForMC][sizeMatrixForMC] = { 0 };
	{
		for (int i = 0; i < sizeMatrixForMC; i++)
		{
			countOfCol = 0;

			for (int j = 0; j < (sizeMatrixForMC - 1); j++)
			{
				count = 0;
				stop = false;

				while (stop != true && count < maxIteration)
				{
					randNumb = (float)rand() / RAND_MAX;
					if (randNumb <= (1 - countOfCol))
					{
						matrixMC[i][j] = randNumb;
						countOfCol += randNumb;
						stop = true;
					}
					count++;
				}
			}

			matrixMC[i][sizeMatrixForMC - 1] = 1 - countOfCol;
		}
	}

#endif

#ifdef STATIC_MATRIX

	float matrixMC[5][5] =
	{ { 0.1, 0.25, 0.15, 0.3, 0.2 },
	{ 0.1, 0.25, 0.05, 0.3, 0.2 },
	{ 0.1, 0.25, 0.05, 0.3, 0.2 },
	{ 0.1, 0.25, 0.05, 0.3, 0.2 },
	{ 0.1, 0.25, 0.05, 0.3, 0.2 } };

#endif

	printf("\t\t\tMattrix for MC\n");
	for (int i = 0; i < sizeMatrixForMC; i++)
	{
		for (int j = 0; j < sizeMatrixForMC; j++)
		{
			printf("%f\t", matrixMC[i][j]);
		}
		printf("\n");
	}

	current = round(((float)rand() / RAND_MAX) * (sizeMatrixForMC - 1));

	FILE *file = fopen(nameStreamFile, "wb");
	char ch;
	int qwe = 0;
	for (int i = 0; i < sizeStream; i++)
	{
		comulP = 0;

		for (int j = 0; j < sizeMatrixForMC; j++)
		{
			comulP += matrixMC[current][j];
			randNumb = (float)rand() / RAND_MAX;

			if (randNumb < comulP)
			{
				current = j;
				ch = current + 1;
				fwrite(&ch, 1, sizeof(unsigned char), file);
				break;
			}
		}
	}

	fclose(file);
}
void readStremFromFile(char *nameStreamFile, vector<unsigned char> &stream)
{
	FILE *file = fopen(nameStreamFile, "rb");
	unsigned char ch;

	for (int i = 0; i < sizeStream; i++)
	{
		fread(&ch, 1, sizeof(unsigned char), file);
		stream.push_back(ch);
	}

	fclose(file);
}
void getTableForCoding(vector<unsigned char> &stream)
{
	for (int i = 0; i < sizeStream; i++)
	{
		if (stream[i] > maxNumOfCh)
		{
			maxNumOfCh = stream[i];
		}	
	}
	
	map<char, int> *statistics = new map<char, int>[maxNumOfCh];
	map <char, int>  :: iterator i;
	list <node*> *table = new list<node*>[maxNumOfCh];

	int currSym = stream[0];
	for (int i = 0; i < sizeStream; i++)
	{
		statistics[currSym - 1][stream[i]]++;
		currSym = stream[i];
	}
	
	for (int j = 0; j < maxNumOfCh; j++)
	{
		for (i = statistics[j].begin(); i != statistics[j].end(); i++)
		{
			node *p = new node;
			p->name = i->first;
			p->count = i->second;
			p->left = NULL;
			p->right = NULL;
			table[j].push_back(p);
		}
	}
	
	for (int j = 0; j < maxNumOfCh; j++)
	{
		while (table[j].size() != 1)
		{
			table[j].sort(compare());

			node *leftSon = table[j].front();
			table[j].pop_front();

			node *rightSon = table[j].front();
			table[j].pop_front();

			node *parent = new node;
			parent->left = leftSon;
			parent->right = rightSon;
			parent->count = leftSon->count + rightSon->count;

			table[j].push_back(parent);
		}
	}

	for (int j = 0; j < maxNumOfCh; j++)
	{
		root[j] = table[j].front();
	}
	
	printf("\t\t\tBuild table\n");
	
	codingTable = new map<char, vector<bool>>[maxNumOfCh];

	for (int j = 0; j < maxNumOfCh; j++)
	{
		printf("--------%d----------\n", j);
		buildTable(root[j], codingTable[j]);
	}
}
void writeStremInFile(char *nameStreamFile, vector<unsigned char> &stream)
{
	FILE *file = fopen(nameStreamFile, "wb");

	for (int i = 0; i < sizeStream; i++)
	{
		fwrite(&stream[i], 1, sizeof(unsigned char), file);
	}

	fclose(file);
}
void getEncodingStream(vector<unsigned char> &stream, vector<bool> &encodingStream)
{
	char ch;
	int currSym = 1;

	for (int i = 0; i < sizeStream; i++)
	{
		ch = stream[i];
		vector<bool> x = codingTable[currSym - 1][ch];
		currSym = ch;
		for (int j = 0; j < x.size(); j++)
		{
			if (x[j] == 1)
			{
				encodingStream.push_back(1);
			}
			else
			{
				encodingStream.push_back(0);
			}
		}
	}
}
void writeBits(char *nameStreamFile, vector <bool> &encodingStream)
{
	FILE *file = fopen(nameStreamFile, "wb");
	bitset <8> byte;
	unsigned char ch;
	bool exit = false;

	for (int i = 0; i < encodingStream.size(); i += 8)
	{
		for (int j = i; j < i + 8; j++)
		{
			if (j >= encodingStream.size())
			{
				exit = true;
			}
			else
			{
				byte[j - i] = encodingStream[j];
			}
		}

		ch = byte.to_ullong();
		fwrite(&ch, 1, sizeof(unsigned char), file);
		if (exit == true)
			break;
	}
	fclose(file);
}
void getDecodingStream(vector<bool> &encodingStream, vector<unsigned char> &decodingStream)
{
	bool bit;
	int currSym = 1;

	node *p = root[currSym - 1];

	for (int i = 0; i < encodingStream.size(); i++)
	{
		bit = encodingStream[i];

		if (bit && p->left != NULL && p->right != NULL)
		{
			p = p->right;
		}
		else if(!bit && p->left != NULL && p->right != NULL)
		{
			p = p->left;
		}

		if (p->left == NULL && p->right == NULL)
		{
			decodingStream.push_back(p->name);
			currSym = p->name;
			p = root[currSym - 1];
		}
	}
} 

int main()
{
	vector<unsigned char> stream;
	vector<bool> encodingStream;
	vector<unsigned char> decodingStream;

	//generation stream and write in file, read stream from file
	getStreamWithMC();
	readStremFromFile(nameStreamFile, stream);

	//get table for coding 
	getTableForCoding(stream);

	//coding stream and write it in file
	getEncodingStream(stream, encodingStream);
	writeBits(nameEncodingStreamFile, encodingStream);

	//decoding stream
	getDecodingStream(encodingStream, decodingStream);
	writeStremInFile(nameDecodingFile, decodingStream);

	//differens betwen stream and decoding stream
	for (int i = 0; i < sizeStream; i++)
	{
		int diff = stream[i] - decodingStream[i];
		if (diff > 0)
		{
			printf("Erorr:/t%f/n", diff);
		}
	}
										   
	system("pause");
}