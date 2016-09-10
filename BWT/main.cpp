#include "BWT/BWT.h"

#include <iostream>

using namespace std;

int main() {
	string encoding = BWT_encode("Hello World!");

	cout << encoding << '\n';

	string decoding = BWT_decode(encoding);

	cout << decoding << endl;

	return 0;
}