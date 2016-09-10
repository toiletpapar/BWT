#include "BWT/BWT.h"

#include <iostream>

using namespace std;

int main() {
	string encoding = BWT_encode("Hello World");

	cout << encoding << endl;

	return 0;
}