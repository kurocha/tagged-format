//
//  Block.cpp
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Block.h"
#include <string.h>

namespace TaggedModelFormat {
	std::string Mesh::name_for_layout(Layout layout) {
		switch (layout) {
			case POINTS:
				return "points";
				
			case LINES:
				return "lines";
				
			case LINE_LOOP:
				return "line-loop";
				
			case LINE_STRIP:
				return "line-strip";
				
			case TRIANGLES:
				return "triangles";
				
			case TRIANGLE_STRIP:
				return "triangle-strip";
				
			case TRIANGLE_FAN:
				return "triangle-fan";
				
			default:
				return "unspecified/unknown";
		}
	}
	
	bool compare_names(std::string a, unsigned char b[32]) {
		if (a.size() > 32) return false;
		
		return strncmp(a.c_str(), (const char *)b, 32) == 0;
	}
	
	template <typename BlockT, typename ElementT>
	bool within(BlockT * block, std::size_t i) {
		return sizeof(BlockT) + (sizeof(ElementT) * i) < block->size;
	}
	
	Axis * Axes::axis_named(std::string name) {
		for (std::size_t i = 0; within<Axes, Axis>(this, i); i += 1) {
			if (compare_names(name, axes[i].name)) {
				return &axes[i];
			}
		}
		
		return nullptr;
	}
	
	OffsetT Dictionary::offset_named(std::string name) {
		for (std::size_t i = 0; within<Dictionary, NamedOffset>(this, i); i += 1) {
			if (compare_names(name, entries[i].name)) {
				return entries[i].offset;
			}
		}
		
		return 0;
	}
}
