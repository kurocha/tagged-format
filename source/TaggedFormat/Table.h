//
//  Table.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_DICTIONARY_H
#define __TAGGED_FORMAT_DICTIONARY_H

#include "Block.h"
#include <cstring>

namespace TaggedFormat {
	template <std::size_t NAME_LENGTH = 32>
	struct NamedElement {
		unsigned char name[NAME_LENGTH];

		bool match(const std::string & pattern) {
			if (pattern.size() > NAME_LENGTH)
				return false;
			else
				return std::strncmp(pattern.c_str(), (const char *)name, NAME_LENGTH) == 0;
		}
	};

	struct NamedOffset : NamedElement<32> {
		OffsetT offset;
	};

	/// A block containing a key-offset mapping.
	template <typename _ElementT = NamedOffset>
	struct Table : public Block {
		typedef _ElementT ElementT;

		ElementT entries[0];

		/// Get a named element within the table.
		/// @returns nullptr if an element with the given name was not found.
		ElementT * element_named(const std::string & name) {
			for (std::size_t i = 0; within<Table>(this, i); i += 1) {
				if (entries[i].match(name)) {
					return &entries[i];
				}
			}

			return nullptr;
		}
	};

	struct OffsetTable : public Table<> {
		/// Get an offset with a given name.
		/// @returns 0 if offset with the given name was found.
		OffsetT offset_named(std::string name) {
			ElementT * element = element_named(name);

			if (element)
				return element->offset;
			else
				return 0;
		}
	};

	template<>
	struct BlockTraits<OffsetTable> {
		static const TagT TAG = tag_from_identifier("oftb");
	};
}

#endif
