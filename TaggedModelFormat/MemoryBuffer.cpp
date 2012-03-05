//
//  MemoryBuffer.cpp
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 6/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "MemoryBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace TaggedModelFormat {
	
	void MemoryBuffer::resize(std::size_t size) {
		if (size < _capacity) {
			_buffer.size = size;
		} else {
			_buffer.begin = realloc(_buffer.begin, size);
			_buffer.size = size;
			
			_capacity = size;
		}
	}
	
	MemoryBuffer::MemoryBuffer() {
		_capacity = 0;
		_buffer.begin = nullptr;
		_buffer.size = 0;
	}
	
	MemoryBuffer::MemoryBuffer(std::size_t capacity) {
		_capacity = capacity;
		_buffer.begin = malloc(_capacity);
		_buffer.size = 0;
	}
	
	MemoryBuffer::~MemoryBuffer() {
		if (_buffer.begin)
			free(_buffer.begin);
	}
	
	Buffer MemoryBuffer::buffer() {
		return _buffer;
	}
	
	OffsetT MemoryBuffer::append(std::size_t size) {
		OffsetT current = _buffer.size;
		
		resize(_buffer.size + size);
		
		return current;
	}
	
	void * MemoryBuffer::fetch(OffsetT offset) {
		return _buffer.at(offset);
	}
	
	void MemoryBuffer::write_to_file(std::string path) const {
		// Create the file on disk:
		int fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
		
		if (fd == -1) {
			perror("open");
			
			return;
		}
		
		{
			// Resize the file on disk:			
			if (lseek(fd, _buffer.size - 1, SEEK_SET) == -1) {
				perror("lseek");
			}
			
			if (write(fd, "", 1) == -1) {
				perror("write");
			}
		}
		
		void * buffer = mmap(0, _buffer.size, PROT_WRITE, MAP_SHARED, fd, 0);
		
		memcpy(buffer, _buffer.begin, _buffer.size);
		
		if (munmap(buffer, _buffer.size) == -1) {
			perror("munmap");
		}
		
		if (close(fd) == -1) {
			perror("close");
		}
	}
	
	void MemoryBuffer::read_from_file(std::string path) {
		int fd = open(path.c_str(), O_RDWR);
		
		if (fd == -1) {
			perror("open");
			
			return;
		}
		
		std::size_t size = lseek(fd, 0, SEEK_END);
		resize(size);
				
		void * buffer = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
		
		memcpy(_buffer.begin, buffer, size);
				
		if (munmap(buffer, size) == -1) {
			perror("munmap");
		}
		
		if (close(fd) == -1) {
			perror("close");
		}
	}

	
}
