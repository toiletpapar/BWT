#include "BWT.h"

#include <algorithm>
#include <bitset>
#include <string>
#include <iterator>
#include <iostream>
#include <forward_list>
#include <vector>

//Next Steps: Tests and Exception Handling
//This implementation can be improved by using suffix tries for sorting
//This implementation currently doesn't handle special characters like ' and " and – very well.
using namespace std;

//This implementation assumes the $ as the special EOT character that does not appear in the source text.
const char EOT_CHAR = '$';

//Convenience for boilerplate
void compress(string source, string filename) {
	forward_list<char> alphabet;
	for (char i = 32; i <= 126; i++) {
		//' ' to '~'
		alphabet.push_front(i);
	}

	Binary_File& binary_file = Binary_File(filename, fstream::out | fstream::trunc);

	RLE_encode(binary_file, MTF_encode(BWT_encode(source), alphabet));
}

string decompress(string filename) {
	forward_list<char> alphabet;
	for (char i = 32; i <= 126; i++) {
		//' ' to '~'
		alphabet.push_front(i);
	}

	Binary_File& binary_file = Binary_File(filename, fstream::in);

	return BWT_decode(MTF_decode(RLE_decode(binary_file), alphabet));
}

//Do a cyclic shift on the source text
//For s[0..n-1] return the concatenation of s[i+1..n-1] and s[0..i] where i is the distance
string cyclic_shift(string source, int distance) {
	return source.substr(distance, string::npos) + source.substr(0, distance);
}

string BWT_encode(string source) {
	//Append special EOT character
	source.append(&EOT_CHAR);

	//Generate the n cyclic shifts and put them in a list
	vector<string> shifts;
	
	for (unsigned int i = 0; i < source.length(); i++) {
		string shift = cyclic_shift(source, i);

		shifts.push_back(shift);
	}

	//Sort the list lexicographically
	sort(shifts.begin(), shifts.end());

	//Return a string comprised of the last letter of each word in the sorted list
	string encoding = "";

	for (vector<string>::iterator it = shifts.begin(); it != shifts.end(); it++) {
		encoding = encoding + it->at(it->length() - 1);
	}

	return encoding;
}

bool comp_tuples(pair<char, int> a, pair<char, int> b) {
	return (a.first < b.first);
}

string BWT_decode(string encoding) {
	if (encoding.empty()) {
		return "";
	}

	int j;

	//Put the encoded characters in tuples (c[i], i)
	vector<pair<char, int>> coding_tuples;

	for (unsigned int i = 0; i < encoding.length(); i++) {
		char coded_char = encoding.at(i);

		//Find the index of the special end of text character
		if (coded_char == EOT_CHAR) {
			j = i;
		}

		coding_tuples.push_back(pair<char, int>(encoding.at(i), i));
	}

	//Sort the tuples by their character preserving order (stable)
	/* 
	 * This is because the order in which we encountered the same character provides us
	 * with a different index leading to a different cyclic shift and thus a different following character
	 */
	stable_sort(coding_tuples.begin(), coding_tuples.end(), comp_tuples);

	//Follow the indicies to retrieve the letters in the source text
	string decoding = "";

	do {
		decoding = decoding + coding_tuples.at(j).first;
		j = coding_tuples.at(j).second;
	} while (coding_tuples.at(j).first != EOT_CHAR);

	return decoding;
}

//Move-to-front encoding
//Gives empty encoding if empty alphabet or empty source, or source contains a character not in the alphabet
vector<int> MTF_encode(string source, forward_list<char> alphabet) {
	vector<int> encoding;

	//Parameter checking
	if (alphabet.begin() != alphabet.end()) {
		for (string::iterator c = source.begin(); c != source.end(); c++) {
			//Search the alphabet for the index of the letter in source
			int index = 0;
			bool found = false;
			forward_list<char>::iterator character = alphabet.begin();
			forward_list<char>::iterator next_character = next(alphabet.begin(), 1);

			if (*character != *c) {
				while (!found && next_character != alphabet.end()) {
					index++;
					if (*next_character == *c) {
						found = true;
					}
					else {
						character++;
						next_character++;
					}
				}

				if (!found) {
					//character was not in the alphabet provided, something went wrong
					return vector<int>();
				}

				//Move the element at index to the front of the list
				alphabet.erase_after(character);
				alphabet.push_front(*c);
			}

			//Add the index to the integer sequence encoding
			encoding.push_back(index);
		}
	}
	
	return encoding;
}

string MTF_decode(vector<int> encoding, forward_list<char> alphabet) {
	string source = "";

	if (alphabet.begin() != alphabet.end()) {
		for (vector<int>::iterator index = encoding.begin(); index != encoding.end(); index++) {
			if (*index != 0) {
				forward_list<char>::iterator character = next(alphabet.begin(), (*index) - 1);
				forward_list<char>::iterator next_character = next(character, 1);

				//Append the character at index in the alphabet to the source text
				source = source + *next_character;

				//Move the element at index to the front
				alphabet.push_front(*next_character);
				alphabet.erase_after(character);
			}
			else {
				source = source + *(alphabet.begin());
			}
			
		}
	}

	return source;
}

//index starts from least significant bit and at 0
//Example: read_bit(0x0000000f, 3) -> 1
bool read_bit(int number, int index) {
	return (number >> index) & 1;
}

const int SIG_BIT_INDEX = CHAR_BIT * sizeof(int) - 1;

//Run-length encoding
//Encode bit length of runs as 0's
void RLE_encode(Binary_File& encoding, vector<int> source) {
	if (!source.empty()) {
		bool running_bit = read_bit(source.at(0), SIG_BIT_INDEX);

		//Encode the first bit
		encoding.write(running_bit);

		int count_running_bit = 0;
		

		for (vector<int>::iterator source_it = source.begin(); source_it != source.end(); source_it++) {
			for (int i = SIG_BIT_INDEX; i >= 0; i--) {
				bool bit_read = read_bit(*source_it, i);

				/*
				 * Consider the following cases where x is the bit read and y is the length of the previous run then:
				 * 1. x is not the last bit (of the last number) and continues the run then increment y
				 * 2. x is the last bit and continues the run then increment y and encode y
				 * 3. x is not the last and and stops the run then encode y
				 * 4. x is the last bit and stops the run then encode y and encode x (by appending a 1 to the bitset)
				 */

				//Should consider removing leading 0's from the integers given by source?
				//Case 1, 2: Increment y
				if (running_bit == bit_read) {
					count_running_bit++;
				}

				//Case 2, 3 (Encode y)
				if (running_bit != bit_read || (source_it == --source.end() && i == 0)) {
					//Encode the bit length of the run [(floor(lg(k)) + 1] minus one since the run contains at least one bit
					int binary_length = static_cast<int>(floor(log2(count_running_bit)));

					for (int i = 0; i < binary_length; i++) {
						encoding.write(0);
					}

					//Encode the actual length of the run
					bitset<CHAR_BIT * sizeof(int)> running_bits = bitset<CHAR_BIT*sizeof(int)>(count_running_bit);

					//Here we skip the leading 0 bits
					bool leading = true;

					for (int i = running_bits.size() - 1; i >= 0; i--) {
						if (leading && running_bits[i] != false) {
							leading = false;
							encoding.write(running_bits[i]);
						}
						else if (!leading) {
							encoding.write(running_bits[i]);
						}
					}

					//Case 4 (Encode x)
					if (running_bit != bit_read && source_it == --source.end() && i == 0) {
						encoding.write(1);
					}

					//Flip the running bit
					running_bit = !running_bit;

					//Reset the count
					count_running_bit = 1;
				}
			}
		}
	}
}

vector<int> RLE_decode(Binary_File& encoding) {
	vector<int> decoding;
	bitset<CHAR_BIT * sizeof(int)> buffer;
	int buffer_position = CHAR_BIT * sizeof(int) - 1;	//Next position in the buffer to write to

	int read_bit = encoding.read();

	if (read_bit != EOF) {
		//The first bit is the bit the decoding starts with
		bool running_bit = static_cast<bool>(read_bit);

		while (read_bit != EOF) {
			//The length + 1 of the next run of 0's is the length of the binary representation of the length of the running bit
			int binary_length = 0;
			do {
				binary_length++;
				read_bit = encoding.read();
			} while (read_bit != EOF && !static_cast<bool>(read_bit));

			//If we encountered EOF at the stage then terminate since the encoding alg would write 0's to fill the encoding until it reached a multiple of bits in int
			if (read_bit == EOF) {
				return decoding;
			}

			//Read the next length + 1 bits and put the length of the running bit into the buffer
			int run_length = 0;
			for (int x = binary_length - 1; x >= 0; x--) {
				//We account for the set bit that we read during the determination of the binary length by going straight to the decoding without reading
				if (x != binary_length - 1) {
					read_bit = encoding.read();
				}
				
				if (x == binary_length - 1 || (read_bit != EOF && static_cast<bool>(read_bit))) {
					for (double y = 0; y < exp2(x); y++) {
						buffer[buffer_position] = running_bit;
						buffer_position--;

						//If our buffer reaches the size of an int then write it to the decoding and reset the buffer
						if (buffer_position == -1) {
							buffer_position = CHAR_BIT * sizeof(int) - 1;
							decoding.push_back(static_cast<int>(buffer.to_ulong()));
						}
					}
				}
				else if (read_bit == EOF) {
					//Something went horribly wrong with the file
					cout << "File was not RLE appropriate";
					return vector<int>();
				}
			}

			running_bit = !running_bit;
		}
	}

	return decoding;
}