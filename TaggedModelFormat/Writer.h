//
//  Writer.h
//  TaggedModelFormat
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_WRITER_H
#define _TAGGED_MODEL_FORMAT_WRITER_H

#include "Block.h"
#include <string>

namespace TaggedModelFormat {
	
	class IWriteBuffer {
	public:
		virtual ~IWriteBuffer();
		
		/// Append size bytes to the buffer, return the offset to the start.
		virtual OffsetT append(std::size_t size) = 0;
		
		/// Dereference an offset into the buffer:
		virtual void * fetch(OffsetT offset) = 0;
	};
	
	template <typename BlockT>
	class BufferedOffset {
	protected:
		OffsetT _offset;
		IWriteBuffer * _write_buffer;
	
	public:
		BufferedOffset(OffsetT offset, IWriteBuffer * write_buffer) : _offset(offset), _write_buffer(write_buffer) {
		}
		
		BlockT * operator*() {
			return (BlockT *)_write_buffer->fetch(_offset);
		}
		
		BlockT * operator->() {
			return (BlockT *)_write_buffer->fetch(_offset);
		}
		
		operator OffsetT() {
			return _offset;
		}
	};
	
#pragma mark -
		
	class Writer {
	protected:
		IWriteBuffer * _write_buffer;
		
	public:		
		Writer(IWriteBuffer * write_buffer);
		virtual ~Writer();
		
		BufferedOffset<Header> header();
		
		template <typename BlockT>
		BufferedOffset<BlockT> append(OffsetT capacity = 0) {
			BlockT block;
			clear(block, capacity);
			
			OffsetT offset = _write_buffer->append(block.size);
			
			void * start = _write_buffer->fetch(offset);
			memcpy(start, &block, sizeof(BlockT));
						
			return BufferedOffset<BlockT>(offset, _write_buffer);
		}
	};
}

#endif
