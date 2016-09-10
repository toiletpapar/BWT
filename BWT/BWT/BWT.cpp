#include "BWT.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

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
	
	for (int i = 0; i < source.length(); i++) {
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