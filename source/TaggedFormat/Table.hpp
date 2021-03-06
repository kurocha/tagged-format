//
//  Table.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include "Block.hpp"
#include <cstring>

namespace TaggedFormat {
	/// A block containing a key-offset mapping.
	template <typename ElementT>
	struct Table : public Array<ElementT> {
		/// Get a named element within the table.
		/// @returns nullptr if an element with the given name was not found.
		template <typename PatternT>
		const ElementT * lookup(const PatternT & name) const {
			const ElementT * entries = this->begin();

			for (std::size_t i = 0; i < this->count(); i += 1) {
				if (entries[i].match(name)) {
					return &entries[i];
				}
			}

			return nullptr;
		}
		
		template <typename PatternT>
		ElementT * lookup(const PatternT & name) {
			auto result = const_cast<const decltype(this)>(this)->lookup(name);
			
			return const_cast<ElementT *>(result);
		}
	};

	struct NamedOffset {
		static const TagT TAG = tag_from_identifier("#OFS");

		FixedString<> name;
		OffsetT offset;

		bool match(const std::string & key) const {
			return name == key;
		}
	};

	struct OffsetTable : public Table<NamedOffset> {
		/// Get an offset with a given name.
		/// @returns 0 if offset with the given name was found.
		OffsetT offset_named(const std::string & name) const;
	};
}
