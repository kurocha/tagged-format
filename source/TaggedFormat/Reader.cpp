//
//  Reader.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Reader.hpp"
#include <string>

namespace TaggedFormat {
	
	Reader::Reader(const Buffer & buffer) : _buffer(buffer)
	{
	}
	
	Reader::~Reader() {
	}
	
	const Header * Reader::header() {
		if (!_header) {
			_header = reinterpret_cast<const Header *>(_buffer.begin());
		}

		if (_header->magic != 42) {
			return nullptr;
		}

		return _header;
	}

	bool Reader::flipped() {
		auto header = this->header();
		
		if (header->tag == Header::TAG) {
			return false;
		}
		
		return true;
	}
	
	const Block * Reader::block_at_offset(OffsetT offset) {
		if (offset > _buffer.size()) {
			return nullptr;
		}
		
		return reinterpret_cast<const Block *>(_buffer + offset);
	}
}
