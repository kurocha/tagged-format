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

#include <string>

namespace TaggedFormat {
	constexpr TagT tag_from_identifier(const char identifier[4]) {
		return (identifier[3] << 24) | (identifier[2] << 16) | (identifier[1] << 8) | identifier[0];
	}

	inline std::string identifier_from_tag(const TagT & tag) {
		return std::string((char*)&tag, (char*)&tag + 4);
	}
	
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

		/// Return a human readable name for the given tag.
		std::string tag_name() {
			return identifier_from_tag(tag);
		}
	};
	
	/// The header typically used at the start of the binary file.
	struct Header : public Block {
		static const TagT TAG = tag_from_identifier("HDR3");
		
		/// A version identifier, typically 42.
		MagicT magic;
		
		/// The offset to the top block.
		OffsetT top_offset;
	};
	
	/// Reset a data block with the correct tag and size based on type.
	template <typename BlockT>
	void clear(BlockT & block, OffsetT capacity = 0) {
		block.tag = BlockT::TAG;
		block.size = sizeof(BlockT) + capacity;
	}

	struct Reference {
		static const TagT TAG = tag_from_identifier("#REF");

		OffsetT offset;

		/// Implicit cast to OffsetT for convenience.
		operator OffsetT () const {
			return offset;
		}
	};

	template <typename _ElementT, typename DerivedT = Block>
	struct Array : public Block {
		typedef _ElementT ElementT;
		static const TagT TAG = ElementT::TAG;

		/// A helper method to calculate the size required for allocation
		static std::size_t array_size(std::size_t count) {
			return sizeof(ElementT) * count;
		}

		/// Number of elements in the array:
		std::size_t count () {
			return (size - sizeof(DerivedT)) / sizeof(ElementT);
		}

		/// Return the end of the structure, where the array elements are stored.
		ElementT * begin() {
			return (ElementT *)Block::end(sizeof(DerivedT));
		}

		using Block::end;

		// Make sure we have a suitable type signature for iteration.
		ElementT * end() {
			return (ElementT *)Block::end();
		}

		ElementT & at(std::size_t index) {
			if (index < count()) {
				return *(begin() + index);
			} else {
				throw std::out_of_range("Invalid index for element array!");
			}
		}
	};
}

#endif
