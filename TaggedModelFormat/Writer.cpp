//
//  Writer.cpp
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Writer.h"

namespace TaggedModelFormat {
	
	IWriteBuffer::~IWriteBuffer() {
		
	}
	
#pragma mark -
	
	Writer::Writer(IWriteBuffer * write_buffer) : _write_buffer(write_buffer) {	
		
	}
	
	Writer::~Writer() {
		
	}
	
	BufferedOffset<Header> Writer::header() {
		Header header;
		clear(header);
		
		header.magic = 42;
		
		return append(&header);
	}
}
