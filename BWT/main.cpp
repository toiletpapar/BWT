#include "BWT/BWT.h"

#include <iostream>
#include <string>
#include <vector>
#include <forward_list>

#include <sstream>

using namespace std;

int main() {
	cout << "==Encoding==\n";

	string message = "Hello World! My name is Tyler and I have a compression algorithm here!";

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

	cout << "\n ==Decoding== \n";
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

	cout << "Space Savings: " << 1 - (compressed_size / uncompressed_size)  << '\n';

	cout << "Convenience Method Test:\n";
	compress("Convenience is King!", "test2");
	cout << decompress("test2") << endl;

	return 0;
}