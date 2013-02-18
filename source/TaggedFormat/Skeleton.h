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

namespace TaggedFormat {
	typedef Aligned<uint16_t, 16>::TypeT BoneID;

	/// A skeleton block contains weights, bones and animation sequences.
	struct Skeleton : public Block {
		/// The offset of the per-vertex weights block.
		OffsetT weights_offset;

		/// The offset of the bone heirarchy.
		OffsetT bones_offset;

		/// A table for animation sequences.
		OffsetT sequences_offset;
	};

	template<>
	struct BlockTraits<Skeleton> {
		static const TagT TAG = tag_from_identifier("skel");
	};

	/// A list of indicies, typically an array of uint16_t or unit32_t.
	template <std::size_t COUNT = 4>
	struct Weights : public Block {
		/// Per-vertex weight information:
		struct Vertex {
			BoneID bones[COUNT];
			float32 weights[COUNT];
		};

		typedef Vertex ElementT;

		/// We have COUNT weights per vertex, organised in parallel.
		Vertex vertices[0];
	};

	template<>
	struct BlockTraits<Weights<2>> {
		static const TagT TAG = tag_from_identifier("b2w2");
	};

	template<>
	struct BlockTraits<Weights<4>> {
		static const TagT TAG = tag_from_identifier("b4w4");
	};

	struct Bones : public Block {
		/// A list of indicies, typically an array of uint16_t or unit32_t.
		struct Bone : public Block {
			// The index of the parent node.
			BoneID parent;

			/// The bind pose transform from the parent to the child bone.
			float32 transform[16];
		};

		typedef Bone ElementT;

		Bone bones[0];
	};

	template<>
	struct BlockTraits<Bones> {
		static const TagT TAG = tag_from_identifier("bnes");
	};

	struct Animation : public Block {
		float32 start_time;
		float32 end_time;

		OffsetT key_frames_offset;
	};

	template<>
	struct BlockTraits<Animation> {
		static const TagT TAG = tag_from_identifier("anim");
	};

	struct BoneKeyFrames : public Block {
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

	template<>
	struct BlockTraits<BoneKeyFrames> {
		static const TagT TAG = tag_from_identifier("bnkf");
	};
}

#endif
