//
//  Writer.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Writer.hpp"

namespace TaggedFormat
{
	Writer::Writer(ResizableBuffer & buffer) : _buffer(buffer)
	{
	}
	
	Writer::~Writer()
	{
	}
	
	BufferedOffset<Header> Writer::header()
	{
		if (!_header) {
			_header = append<Header>();
			_header->magic = 42;
		}
		
		return _header;
	}
}
