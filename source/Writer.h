//
//  Writer.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_WRITER_H
#define _TAGGED_MODEL_FORMAT_WRITER_H

#include "Block.h"
#include <cstring>

namespace TaggedFormat {
	
	/// A simple output buffer for writing blocks of data.
	class IWriteBuffer {
	public:
		virtual ~IWriteBuffer();
		
		/// Append size bytes to the buffer, return the offset to the start.
		virtual OffsetT append(std::size_t size) = 0;
		
		/// Dereference an offset into the buffer:
		virtual void * fetch(OffsetT offset) = 0;
	};

	/// Used for writing data at a given offset, but can be implicitly cast to an offset.
	/// Rather than work directly with pointers, this wrapper assists with writing data to a particular offset in a buffer.
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

		/// Return the offset of the block.
		operator OffsetT() {
			return _offset;
		}
	};
	
//MARK: -
	
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
			std::memcpy(start, &block, sizeof(BlockT));
			
			return BufferedOffset<BlockT>(offset, _write_buffer);
		}
	};
}

#endif
