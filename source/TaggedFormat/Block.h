//
//  Block.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_FORMAT_BLOCK_H
#define _TAGGED_FORMAT_BLOCK_H

#include "Buffer.h"

#include <iosfwd>
#include <string>

namespace TaggedFormat {
	
	typedef Aligned<uint32_t>::TypeT TagT;
	typedef Aligned<uint32_t>::TypeT MagicT;
	
	/// A standard 32-bit floating point data-type.
	typedef Aligned<float>::TypeT float32;
	
	/// A standard 64-bit floating point data-type.
	typedef Aligned<double>::TypeT float64;
	
	/// For blocks that end with a list of elements, this returns the number of elements in the block.
	template <typename BlockT>
	std::size_t element_count (BlockT * block) {
		return (block->size - sizeof(BlockT)) / sizeof(typename BlockT::ElementT);
	}

	template <typename BlockT>
	bool within(BlockT * block, std::size_t i) {
		return sizeof(BlockT) + (sizeof(typename BlockT::ElementT) * i) < block->size;
	}
	
	constexpr TagT tag_from_identifier(const char identifier[4]) {
		return (identifier[0] << 24) | (identifier[1] << 16) | (identifier[2] << 8) | identifier[3];
	}
	
	template <typename BlockT>
	struct BlockTraits {
	};
	
	/// A generic block header.
	struct Block {
		/// The blog identifier.
		TagT tag;
		
		// Total size of block including this structure.
		OffsetT size;
		
		/// @returns a pointer to the end of the block in memory based on its size.
		void * end() {
			return (ByteT *)this + size;
		}
		
		/// @returns a pointer to a specific offset from the start of the block.
		void * end(std::size_t offset) {
			return (ByteT *)this + offset;
		}
	};
	
	/// The header typically used at the start of the binary file.
	struct Header : public Block {
		/// A version identifier, typically 42.
		MagicT magic;
		
		/// The offset to the top block.
		OffsetT top_offset;
	};
	
	template<>
	struct BlockTraits<Header> {
		static const TagT TAG = tag_from_identifier("tmv2");
	};
	
	/// Return a human readable name for the given tag.
	std::string tag_name(TagT tag);
	
	/// Reset a data block with the correct tag and size based on type.
	template <typename BlockT>
	void clear(BlockT & block, OffsetT capacity = 0) {
		block.tag = BlockTraits<BlockT>::TAG;
		block.size = sizeof(BlockT) + capacity;
	}
}

#endif
