//
//  Reader.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Reader.h"
#include <string>

namespace TaggedFormat {
	
	Reader::Reader(Buffer buffer) : _buffer(buffer), _header(nullptr) {		
	}
	
	Reader::~Reader() {
	}
	
	Header * Reader::header() {
		if (!_header) {
			_header = (Header *)_buffer.begin;
		}

		if (_header->magic != 42) {
			return nullptr;
		}

		return _header;
	}

	bool Reader::flipped() {
		Header * header = this->header();
		
		if (header->tag == Header::TAG) {
			return false;
		}
		
		return true;
	}
	
	std::string Reader::tag_name(TagT tag) {
		unsigned char name[4];

		for (std::size_t i = 1; i <= 4; i += 1) {
			name[4 - i] = tag % 256; tag >>= 8;
		}
		
		return std::string(&name[0], &name[4]);
	}
	
	Block * Reader::block_at_offset(OffsetT offset) {
		if (offset > _buffer.size) {
			return nullptr;
		}
		
		return (Block *)((ByteT *)_buffer.begin + offset);
	}
}
