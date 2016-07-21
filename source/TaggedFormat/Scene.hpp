//
//  Scene.h
//  TaggedFormat
//
//  Created by Samuel Williams on 20/02/13.
//  Copyright (c) 2013 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include "Block.hpp"

namespace TaggedFormat {
	/*
		Scene: node
			[name] [transform]

			controller-instance
				mesh: $Cylinder-mesh
				skeleton: $Cylinder-skeleton
			end
		end
	*/

	struct Node : public Array<Reference, Node> {
		static const TagT TAG = tag_from_identifier("NODE");

		/// The node name, if any.
		FixedString<> name;

		/// Node transformation.
		float32 transform[16];
	};

	struct GeometryInstance : public Block {
		static const TagT TAG = tag_from_identifier("#GEO");

		OffsetT mesh_offset;
		OffsetT skeleton_offset;
		OffsetT material_offset;
	};
}
