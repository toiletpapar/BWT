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

class Binary_File {
public:
	Binary_File(string filename);
	~Binary_File();

	int get_offset();
	void write(bool bit);
	int read();				//read the next bit from the binary file, return int to accomodate EOF
	void close();			//flush the buffer into the stream, write difference between char_bit and the remaining bits in buffer (the offset) as 0's in the least sig bits
private:
	bool last_op_write;		/* 
							 *  http://stackoverflow.com/questions/17536570/reading-and-writing-to-the-same-file-using-the-same-fstream
							 *	Since we're intermixing read and write, we must:
							 *		1. flush the stream or call a file positioning function when reading after writing
							 *		2. call a file positioning function when writing after reading
							 */
	streampos next_read_pos;
	streampos next_write_pos;
	string filename;
	fstream rle_stream;
	bitset<CHAR_BIT> write_buffer;
	int write_buffer_cursor;
	bitset<CHAR_BIT> read_buffer;
	int read_buffer_cursor;
	int offset;				//Since the encoding may necessarily be divisible by a byte I keep track of the offset of the least significant bits to ignore

	void flush();
};

void RLE_encode(Binary_File& encoding, vector<int> source);
vector<int> RLE_decode(Binary_File& encoding);

#endif
