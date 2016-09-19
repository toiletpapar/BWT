#ifndef BWT_H
#define BWT_H

#include <string>
#include <vector>
#include <forward_list>
#include <fstream>
#include <bitset>

#include "../Binary_File/BinaryFile.h"

using namespace std;

string BWT_encode(string source);
string BWT_decode(string encoding);
vector<int> MTF_encode(string source, forward_list<char> alphabet);
string MTF_decode(vector<int> encoding, forward_list<char> alphabet);
void RLE_encode(Binary_File& encoding, vector<int> source);
vector<int> RLE_decode(Binary_File& encoding);

void compress(string source, string filename);
string decompress(string filename);

#endif
