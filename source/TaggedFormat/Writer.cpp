//
//  Writer.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Writer.hpp"

namespace TaggedFormat {
	
	IWriteBuffer::~IWriteBuffer() {
		
	}
	
//MARK: -
	
	Writer::Writer(IWriteBuffer * write_buffer) : _write_buffer(write_buffer) {	
		
	}
	
	Writer::~Writer() {
		
	}
	
	BufferedOffset<Header> Writer::header() {
		auto header = append<Header>();

		header->magic = 42;
		
		return header;
	}
}
