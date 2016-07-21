//
//  Writer.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include "Block.hpp"

#include <Buffers/ResizableBuffer.hpp>

#include <cstring>

namespace TaggedFormat {
	using Buffers::MutableBuffer;
	using Buffers::ResizableBuffer;
	
	/// Used for writing data at a given offset, but can be implicitly cast to an offset.
	template <typename BlockT>
	class BufferedOffset {
	public:
		BufferedOffset()
		{
		}
		
		BufferedOffset(OffsetT offset, MutableBuffer * buffer) : _offset(offset), _buffer(buffer)
		{
		}
		
		BlockT * operator*() {
			return reinterpret_cast<BlockT *>(*_buffer + _offset);
		}
		
		BlockT * operator->() {
			return reinterpret_cast<BlockT *>(*_buffer + _offset);
		}
		
		/// Return the offset of the block.
		operator OffsetT() {
			return _offset;
		}
		
		operator bool() {
			return _buffer != nullptr;
		}
	protected:
		OffsetT _offset = 0;
		MutableBuffer * _buffer = nullptr;
	};
	
	class Writer {
	public:
		Writer(ResizableBuffer & buffer);
		virtual ~Writer();
		
		Writer(const Writer & other) = delete;
		Writer & operator=(const Writer & other) = delete;
		
		BufferedOffset<Header> header();
		
		template <typename BlockT>
		BufferedOffset<BlockT> append(OffsetT capacity = 0) {
			BlockT block;
			
			// Initialize the block with the required capacity:
			clear(block, capacity);
			
			auto offset = _buffer.size();
			
			//std::cerr << "Increasing size by " << block.size << std::endl;
			_buffer.expand(block.size);
			
			//std::cerr << "Writing block of size " << sizeof(BlockT) << " at " << offset << " buffer size is " << _buffer.size() << std::endl;
			_buffer.write(block, offset);
			
			return BufferedOffset<BlockT>(offset, &_buffer);
		}
		
	protected:
		ResizableBuffer & _buffer;
		BufferedOffset<Header> _header;
	};
}
