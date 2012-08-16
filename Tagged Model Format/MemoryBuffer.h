//
//  MemoryBuffer.h
//  This file is part of the "Tagged Model Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 6/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_MEMORY_BUFFER_H
#define _TAGGED_MODEL_FORMAT_MEMORY_BUFFER_H

#include "Writer.h"

namespace TaggedModelFormat {
	
	class MemoryBuffer : public virtual IWriteBuffer {
	protected:
		void * _begin;
		std::size_t _size;
		
		std::size_t _capacity;
		
		void resize(std::size_t size);
		
	public:
		MemoryBuffer();
		MemoryBuffer(std::size_t capacity);
		~MemoryBuffer();
		
		Buffer buffer();
		
		virtual OffsetT append(std::size_t size);
		virtual void * fetch(OffsetT offset);
		
		void write_to_file(std::string path) const;
		void read_from_file(std::string path);
	};
	
}

#endif
