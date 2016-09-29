#include "BWT/BWT.h"

#include <iostream>
#include <string>
#include <vector>
#include <forward_list>

#include <sstream>

using namespace std;

int main() {
	cout << "==Encoding==\n";

	string message = "In the late 1970s and early 1980s, the developed countries of North America and Western Europe suffered economically in the face of stiff competition from Japan's ability to produce high-quality goods at competitive costs. For the first time since the start of the Industrial Revolution, the United Kingdom became a net importer of finished goods. The United States undertook its own soul-searching, expressed most pointedly in the television broadcast of If Japan Can... Why can't We? Firms began reexamining the techniques of quality control invented over the past 50 years and how those techniques had been so successfully employed by the Japanese. It was in the midst of this economic turmoil that TQM took root.";
	//string message = "Alice is the best!";
	int uncompressed_size = message.length() * CHAR_BIT;

	cout << "Message: " << message << '\n';

	string encoding = BWT_encode(message);

	cout << "BWT Encoding: " << encoding << '\n';

	forward_list<char> alphabet;
	for (char i = 32; i <= 126; i++) {
		//' ' to '~'
		alphabet.push_front(i);
	}

	vector<int> MTF_encoding = MTF_encode(encoding, alphabet);

	cout << "MTF Encoding: ";
	for (vector<int>::iterator it = MTF_encoding.begin(); MTF_encoding.end() != it; it++) {
		cout << *it << " ";
	}

	cout << '\n';

	Binary_File& RLE_encoding = Binary_File("test", fstream::in | fstream::out | fstream::trunc);
	RLE_encode(RLE_encoding, MTF_encoding);

	cout << "RLE Encoding: ";
	int bit = RLE_encoding.read();
	double compressed_size = 0;
	while (bit != EOF) {
		cout << bit;
		bit = RLE_encoding.read();
		compressed_size++;
	}

	RLE_encoding.close();

	cout << "\n\n==Decoding== \n";
	vector<int> RLE_decoding = RLE_decode(RLE_encoding);

	cout << "RLE Decoding: ";
	for (vector<int>::iterator it = RLE_decoding.begin(); RLE_decoding.end() != it; it++) {
		cout << *it << " ";
	}

	cout << '\n';

	string MTF_decoding = MTF_decode(RLE_decoding, alphabet);

	cout << "MTF Decoding: " << MTF_decoding << '\n';

	string source = BWT_decode(MTF_decoding);

	cout << "BWT Decoding: " << source << '\n';

	cout << "\nSpace Savings: " << 1 - (compressed_size / uncompressed_size)  << '\n';

	cout << "\n==Convenience Method Test==\n";
	compress("Convenience is King!", "test2");
	cout << decompress("test2") << endl;

	/*
	//Binary File Test
	Binary_File& test = Binary_File("test3", fstream::in | fstream::out | fstream::trunc);
	cout << "Binary File Test" << endl;
	test.write(1); //1
	test.write(1); //11
	test.write(0); //110
	cout << test.read();
	test.write(0); //1100
	test.write(1); //11001
	test.write(0); //110010
	cout << test.read();
	test.write(1); //1100101
	test.write(0); //11001010
	test.write(0);
	test.write(0);
	cout << test.read();
	test.write(1);
	test.write(1);
	cout << test.read();

	test.close();
	*/

	return 0;
}