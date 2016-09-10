#include "BWT.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

//This implementation can be improved by using suffix tries for sorting
//This implementation currently doesn't handle special characters like ' and " and – very well.
using namespace std;

//This implementation assumes the $ as the special EOT character that does not appear in the source text.
const char EOT_CHAR = '$';

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