//
//  Reader.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include "Block.hpp"

#include <Buffers/Buffer.hpp>

namespace TaggedFormat {
	using Buffers::Buffer;

	/// Provides support for reading blocks from a given data buffer.
	class Reader {
	public:
		Reader(const Buffer & buffer);
		virtual ~Reader();
		
		Reader(const Reader & other) = delete;
		Reader & operator=(const Reader & other) = delete;
		
		const Header * header();
		
		/// If this returns true, all data is flipped (e.g. reversed).
		/// This function is not implemented correctly and generally reverse byte order is not handled correctly yet.
		bool flipped();

		/// @returns a pointer to the block at a given offset.
		const Block * block_at_offset(OffsetT offset);

		/// Read a given block of a specific type from a given offset. Provides basic sanity checking.
		/// @returns nullptr in the event that the block is not the correct type.
		template <typename BlockT>
		const BlockT * block_at_offset(OffsetT offset) {
			const Block * block = block_at_offset(offset);
			
			// Basic error checking:
			if (block->tag != BlockT::TAG) {
				return nullptr;
			}
			
			return (const BlockT *)block;
		}

		/// Read a given array, similar to block_at_offset.
		template <typename ElementT>
		const Array<ElementT> * array_at_offset(OffsetT offset) {
			return block_at_offset<Array<ElementT>>(offset);
		}
		
	protected:
		const Buffer & _buffer;
		const Header * _header = nullptr;
	};

};
