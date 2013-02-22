//
//  Reader.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_FORMAT_READER_H
#define _TAGGED_FORMAT_READER_H

#include "Block.h"

namespace TaggedFormat {

	/// Provides support for reading blocks from a given data buffer.
	class Reader {
	protected:
		Buffer _buffer;
		Header * _header;
		
	public:		
		Reader(Buffer buffer);
		virtual ~Reader();
		
		Header * header();
		
		/// If this returns true, all data is flipped (e.g. reversed).
		/// This function is not implemented correctly and generally reverse byte order is not handled correctly yet.
		bool flipped();

		/// @returns a human readable tag name.
		std::string tag_name(TagT tag);

		/// @returns a pointer to the block at a given offset.
		Block * block_at_offset(OffsetT offset);

		/// Read a given block of a specific type from a given offset. Provides basic sanity checking.
		/// @returns nullptr in the event that the block is not the correct type.
		template <typename BlockT>
		BlockT * block_at_offset(OffsetT offset) {
			Block * block = block_at_offset(offset);
			
			// Basic error checking:
			if (block->tag != BlockT::TAG) {
				return nullptr;
			}
			
			return (BlockT *)block;
		}
	};

};

#endif
