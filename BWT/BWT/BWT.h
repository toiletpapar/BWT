#ifndef BWT_H
#define BWT_H

#include <string>
#include <vector>
#include <forward_list>
#include <fstream>
#include <bitset>

using namespace std;

string BWT_encode(string source);
string BWT_decode(string encoding);
vector<int> MTF_encode(string source, forward_list<char> alphabet);
string MTF_decode(vector<int> encoding, forward_list<char> alphabet);

class RLE_encoding {
public:
	RLE_encoding();
	~RLE_encoding();

	int get_offset();
	void write(bool bit);
	void flush();
	void close();			//flush the buffer into the stream, write difference between char_bit and the remaining bits in buffer (the offset) as 0's in the least sig bits
private:
	fstream rle_stream;
	bitset<CHAR_BIT * sizeof(int)> buffer;
	int buffer_cursor;
	int offset;				//Since the encoding may necessarily be divisible by a byte I keep track of the offset of the least significant bits to ignore
};

void RLE_encode(RLE_encoding& encoding, vector<int> source);
vector<int> RLE_decode(RLE_encoding& encoding);

#endif
