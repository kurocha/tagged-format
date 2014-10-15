//
//  Axes.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_AXES_H
#define __TAGGED_FORMAT_AXES_H

#include "Table.hpp"

namespace TaggedFormat {
	/// A named axis is used to align meshes together.
	struct NamedAxis {
		static const TagT TAG = tag_from_identifier("#AXE");

		/// The name associated with this axis.
		FixedString<> name;
		
		/// The translation of the axis, if any.
		float32 translation[3];

		/// The initial rotation of the axis, if any. Stored as a quaternion (x y z w).
		float32 rotation[4];

		bool match(const std::string & key) const {
			return name == key;
		}
	};

	/// A block containing a list of named axes.
	struct Axes : public Table<NamedAxis> {
	};
}

#endif
