#include "BinaryFile.h"

#include <string>
#include <fstream>
#include <bitset>

using namespace std;

Binary_File::Binary_File(string filename, ios_base::openmode mode) : rle_stream(filename, mode | fstream::binary),
last_op_write(true), write_buffer_cursor(CHAR_BIT - 1), read_buffer_cursor(-1), filename(filename), is_reading_from_write_buffer(false) {

	this->next_read_pos = this->rle_stream.beg;
	this->next_write_pos = this->rle_stream.beg;
	this->max_read_write_buffer_cursor = this->write_buffer_cursor;
}

Binary_File::~Binary_File() {
	if (this->rle_stream.is_open()) {
		this->close();
	}
}

void Binary_File::initialize() {
	this->read_buffer.reset();
	this->write_buffer.reset();
	this->write_buffer_cursor = CHAR_BIT - 1;
	this->read_buffer_cursor = -1;
	this->last_op_write = true;
	this->is_reading_from_write_buffer = false;
	this->next_read_pos = this->rle_stream.beg;
	this->next_write_pos = this->rle_stream.beg;
}

void Binary_File::write(bool bit) {
	if (!this->last_op_write && !this->rle_stream.eof()) {
		//Call a file positioning function to the end of the stream
		this->rle_stream.seekp(this->next_write_pos);
	}

	this->write_buffer[this->write_buffer_cursor] = bit;
	if (this->is_reading_from_write_buffer && this->max_read_write_buffer_cursor != -1) {
		this->read_buffer[this->write_buffer_cursor] = bit;
		this->max_read_write_buffer_cursor--;
	}
	this->write_buffer_cursor--;

	if (this->write_buffer_cursor == -1) {
		this->flush();
		this->next_write_pos = this->rle_stream.tellp();
	}
}

void Binary_File::flush() {
	this->rle_stream.clear();
	this->rle_stream << static_cast<char>(this->write_buffer.to_ulong());
	this->last_op_write = true;
	this->write_buffer_cursor = CHAR_BIT - 1;

	//cout << this->write_buffer << '\n';
}

void Binary_File::close() {
	if (!this->last_op_write) {
		//Call a file positioning function to the end of the stream
		this->rle_stream.seekp(this->next_write_pos);
	}

	if (this->write_buffer_cursor != CHAR_BIT - 1) {
		//Write 0's to the offset bits
		for (int i = this->write_buffer_cursor; i >= 0; i--) {
			this->write_buffer[i] = 0;
		}

		this->flush();
	}

	this->rle_stream.close();
}

int Binary_File::read() {
	//Open the file if it is not open
	if (!this->rle_stream.is_open()) {
		this->rle_stream.open(this->filename, fstream::in | fstream::out | fstream::binary);
		this->initialize();
	}

	if (this->last_op_write) {
		//Read from the last read position
		this->last_op_write = false;
		this->rle_stream.seekg(this->next_read_pos);
	}

	//Update the read buffer appropriately
	if (this->read_buffer_cursor == -1) {
		int byte;
		this->last_op_write = false;

		//If we were previously reading from the write buffer then we should skip the next character
		if (this->is_reading_from_write_buffer) {
			this->is_reading_from_write_buffer = false;
			byte = this->rle_stream.get();
		}

		byte = this->rle_stream.get();

		if (this->rle_stream.eof() || this->rle_stream.fail()) {
			//Try to load the write buffer into the read buffer if the write buffer has characters
			if (this->write_buffer_cursor != CHAR_BIT - 1) {
				this->read_buffer_cursor = CHAR_BIT - 1;
				this->is_reading_from_write_buffer = true;
				this->read_buffer = this->write_buffer;
				this->max_read_write_buffer_cursor = this->write_buffer_cursor;
			}
			else {
				return EOF;
			}
		}
		else {
			//cout << "Read Character: " << bitset<CHAR_BIT>(byte) << "\n";
			this->is_reading_from_write_buffer = false;
			this->read_buffer_cursor = CHAR_BIT - 1;
			this->read_buffer = bitset<CHAR_BIT>(byte);
			this->next_read_pos = this->rle_stream.tellg();
		}
	}

	if (!this->is_reading_from_write_buffer || this->read_buffer_cursor > this->max_read_write_buffer_cursor) {
		//Then we can only read while read_buffer_cursor > max_read_write_buffer_cursor
		return this->read_buffer[this->read_buffer_cursor--];
	}
	else {
		return EOF;
	}
}