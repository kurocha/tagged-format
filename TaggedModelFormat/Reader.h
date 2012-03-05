//
//  Reader.h
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_READER_H
#define _TAGGED_MODEL_FORMAT_READER_H

#include "Block.h"

namespace TaggedModelFormat {
	
	class Reader {
	protected:
		Buffer _buffer;
		Header * _header;
		
	public:		
		Reader(Buffer buffer);
		virtual ~Reader();
		
		Header * header();
		
		// If this returns true, all data is flipped (e.g. reversed).
		bool flipped();
		
		std::string tag_name(uint32_t tag);
		
		Block * block_at_offset(OffsetT offset);
		
		template <typename BlockT>
		BlockT * block_at_offset(OffsetT offset) {
			Block * block = block_at_offset(offset);
			
			// Basic error checking:
			if (block->tag != BlockTraits<BlockT>::TAG) {
				return nullptr;
			}
			
			return (BlockT *)block;
		}
	};

};

#endif
