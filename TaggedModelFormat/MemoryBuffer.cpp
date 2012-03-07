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
			_size = size;
		} else {
			_begin = realloc(_begin, size);
			_size = size;
			
			_capacity = size;
		}
	}
	
	MemoryBuffer::MemoryBuffer() {
		_capacity = 0;
		_begin = nullptr;
		_size = 0;
	}
	
	MemoryBuffer::MemoryBuffer(std::size_t capacity) {
		_capacity = capacity;
		_begin = malloc(_capacity);
		_size = 0;
	}
	
	MemoryBuffer::~MemoryBuffer() {
		if (_begin)
			free(_begin);
	}
	
	Buffer MemoryBuffer::buffer() {
		return (Buffer){_begin, _size};
	}
	
	OffsetT MemoryBuffer::append(std::size_t size) {
		OffsetT current = _size;
		
		resize(_size + size);
		
		return current;
	}
	
	void * MemoryBuffer::fetch(OffsetT offset) {
		return (ByteT *)_begin + offset;
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
			if (lseek(fd, _size - 1, SEEK_SET) == -1) {
				perror("lseek");
			}
			
			if (write(fd, "", 1) == -1) {
				perror("write");
			}
		}
		
		void * buffer = mmap(0, _size, PROT_WRITE, MAP_SHARED, fd, 0);
		
		memcpy(buffer, _begin, _size);
		
		if (munmap(buffer, _size) == -1) {
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
		
		memcpy(_begin, buffer, size);
				
		if (munmap(buffer, size) == -1) {
			perror("munmap");
		}
		
		if (close(fd) == -1) {
			perror("close");
		}
	}

	
}
