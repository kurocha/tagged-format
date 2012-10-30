//
//  Axes.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_AXES_H
#define __TAGGED_FORMAT_AXES_H

#include "Table.h"

namespace TaggedFormat {
	/// A named axis is used to align meshes together.
	struct NamedAxis : public NamedElement<32> {
		/// The translation of the axis, if any.
		float32 translation[3];

		/// The initial rotation of the axis, if any. Stored as a quaternion.
		float32 rotation[4];
	};

	/// A block containing a list of named axes.
	struct Axes : public Table<NamedAxis> {
	};

	template<>
	struct BlockTraits<Axes> {
		static const TagT TAG = tag_from_identifier("axes");
	};
}

#endif
