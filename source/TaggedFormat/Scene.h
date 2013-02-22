//
//  Scene.h
//  TaggedFormat
//
//  Created by Samuel Williams on 20/02/13.
//  Copyright (c) 2013 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_SCENE_H
#define __TAGGED_FORMAT_SCENE_H

#include "Block.h"

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

	struct Node : public Block {
		static const TagT TAG = tag_from_identifier("NODE");

		typedef OffsetT ElementT;

		/// The node name, if any.
		FixedString<> name;

		/// Node transformation.
		float32 transform[16];

		/// The object being instantiated in this part of the scene graph.
		OffsetT children[0];
	};

	struct GeometryInstance : public Block {
		static const TagT TAG = tag_from_identifier("#GEO");

		OffsetT mesh_offset;
		OffsetT skeleton_offset;
		OffsetT material_offset;
	};
}

#endif
