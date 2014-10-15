//
//  Skeleton.h
//  TaggedFormat
//
//  Created by Samuel Williams on 18/02/13.
//  Copyright (c) 2013 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_SKELETON_H
#define __TAGGED_FORMAT_SKELETON_H

#include "Block.hpp"
#include "Mesh.hpp"

namespace TaggedFormat {
	typedef uint8_t BoneID;

	/// A skeleton block contains weights, bones and animation sequences.
	struct Skeleton : public Block {
		static const TagT TAG = tag_from_identifier("SKEL");

		/// The offset of the bone heirarchy.
		OffsetT bones_offset;

		/// A table for animation sequences.
		OffsetT sequences_offset;
	};

	/// A vertex with support for 4 bones:
	struct VertexP3N3M2B4 : public VertexP3N3M2 {
		static const TagT TAG = tag_from_identifier("3324");

		alignas(1) BoneID bones[4];
		alignas(4) float32 weights[4];
	};

	struct SkeletonBone {
		static const TagT TAG = tag_from_identifier("BONE");

		// An optional bone name.
		FixedString<> name;

		// The index of the parent node.
		alignas(4) BoneID parent;

		/// The bind pose transform from the parent to the child bone.
		float32 transform[16];
	};

	struct SkeletonAnimation : public Block {
		static const TagT TAG = tag_from_identifier("ANIM");

		float32 start_time;
		float32 end_time;

		OffsetT key_frames_offset;
	};

	struct SkeletonAnimationKeyFrame : public Block {
		static const TagT TAG = tag_from_identifier("KEYF");

		enum class Interpolation : Aligned<uint8_t>::TypeT {
			LINEAR = 0,
			BEZIER = 1
		};

		static std::string name_for_interpolation(Interpolation interpolation);


		BoneID bone;
		Interpolation interpolation;

		float32 time;
		float32 transform[16];
	};
}

#endif
