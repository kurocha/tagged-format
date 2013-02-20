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

	/// A fixed string typically used for holding names or references:
	template <std::size_t N = 32>
	struct FixedString {
		char value[N];

		void assign(const std::string & other) {
			std::strncpy(value, other.c_str(), N);
		}

		int compare(const std::string & other) const {
			if (other.size() > N)
				return -1;
			else
				return std::strncmp(other.c_str(), (const char *)value, N);
		}

		bool operator==(const std::string & other) const {
			return compare(other) == 0;
		}

		const std::string & operator=(const std::string & other) {
			assign(other);

			return other;
		}

		operator std::string () const {
			std::size_t length = 0;

			while (length < N && value[length]) {
				length++;
			}

			return std::string(value, value+length);
		}
	};

	template <std::size_t N>
	std::istream & operator>>(std::istream & input, FixedString<N> & fixed_string) {
		std::string other;
		input >> other;
		
		fixed_string.assign(other);

		return input;
	}

	template <std::size_t N>
	std::ostream & operator<<(std::ostream & output, const FixedString<N> & fixed_string) {
		output << (std::string)(fixed_string);

		return output;
	}
	
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
		return (identifier[3] << 24) | (identifier[2] << 16) | (identifier[1] << 8) | identifier[0];
	}

	inline std::string identifier_from_tag(const TagT & tag) {
		return std::string((char*)&tag, (char*)&tag + 4);
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

		std::string tag_name() {
			return identifier_from_tag(tag);
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
		static const TagT TAG = tag_from_identifier("hdr3");
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
