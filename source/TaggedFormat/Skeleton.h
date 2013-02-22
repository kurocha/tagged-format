//
//  Skeleton.h
//  TaggedFormat
//
//  Created by Samuel Williams on 18/02/13.
//  Copyright (c) 2013 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_SKELETON_H
#define __TAGGED_FORMAT_SKELETON_H

#include "Block.h"
#include "Mesh.h"

namespace TaggedFormat {
	typedef Aligned<uint16_t, 16>::TypeT BoneID;

	/// A skeleton block contains weights, bones and animation sequences.
	struct Skeleton : public Block {
		static const TagT TAG = tag_from_identifier("SKEL");

		/// The offset of the per-vertex weights block.
		OffsetT weights_offset;

		/// The offset of the bone heirarchy.
		OffsetT bones_offset;

		/// A table for animation sequences.
		OffsetT sequences_offset;
	};

	struct VertexP3N3M2B4 : public BasicVertexP3N3M2 {
		static const TagT TAG = tag_from_identifier("3324");

		BoneID bones[4];
		float32 weights[4];
	};

	struct Bones : public Block {
		static const TagT TAG = tag_from_identifier("BONE");

		/// A list of indicies, typically an array of uint16_t or unit32_t.
		struct Bone {
			// An optional bone name.
			FixedString<> name;

			// The index of the parent node.
			BoneID parent;

			/// The bind pose transform from the parent to the child bone.
			float32 transform[16];
		};

		typedef Bone ElementT;

		Bone bones[0];
	};

	struct Animation : public Block {
		static const TagT TAG = tag_from_identifier("ANIM");

		float32 start_time;
		float32 end_time;

		OffsetT key_frames_offset;
	};

	struct SkeletonBoneKeyFrame : public Block {
		static const TagT TAG = tag_from_identifier("KEYF");
	
		enum class Interpolation : Aligned<uint16_t>::TypeT {
			LINEAR = 0,
			BEZIER = 1
		};

		static std::string name_for_interpolation(Interpolation interpolation);

		struct Frame {
			BoneID bone;
			Interpolation interpolation;

			float32 time;
			float32 transform[16];
		};

		typedef Frame ElementT;

		Frame frames[0];
	};
}

#endif
