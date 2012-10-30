//
//  Mesh.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_MODEL_FORMAT_MESH_H
#define __TAGGED_MODEL_FORMAT_MESH_H

#include "Block.h"

namespace TaggedFormat {
	/// A mesh block contains indices, vertices and axes.
	struct Mesh : public Block {
		// These definitions map directly to the equivalent OpenGL constants.
		enum Layout {
			POINTS = 0,
			LINES = 1,
			LINE_LOOP = 2,
			LINE_STRIP = 3,
			TRIANGLES = 4,
			TRIANGLE_STRIP = 5,
			TRIANGLE_FAN = 6
		};

		/// Provides a hint regarding the layout of the indices and vertices.
		Aligned<Layout>::TypeT layout;

		/// The offset of the indices block.
		OffsetT indices_offset;

		/// The offset of the vertices block.
		OffsetT vertices_offset;

		/// The offset of the axes block.
		OffsetT axes_offset;

		/// A table for additional metadata.
		OffsetT metadata_offset;

		/// @returns a human readable name for a given layout.
		static std::string name_for_layout(Layout layout);
	};

	template<>
	struct BlockTraits<Mesh> {
		static const TagT TAG = tag_from_identifier("mesh");
	};

	/// A list of indicies, typically an array of uint16_t or unit32_t.
	template <typename IndexT>
	struct Indices : public Block {
		typedef IndexT ElementT;

		IndexT indices[0];
	};

	template<>
	struct BlockTraits<Indices<uint16_t>> {
		static const TagT TAG = tag_from_identifier("ind2");
	};

	template<>
	struct BlockTraits<Indices<uint32_t>> {
		static const TagT TAG = tag_from_identifier("ind4");
	};

	template<>
	struct BlockTraits<Indices<uint64_t>> {
		static const TagT TAG = tag_from_identifier("ind8");
	};

	/// A list of vertices, typically BasicVertexP3N3M2.
	template <typename VertexT>
	struct Vertices : public Block {
		typedef VertexT ElementT;

		VertexT vertices[0];
	};

	// You are recommended to customize this data type to your exact requirements.

	/// For 2D lines and structures.
	struct BasicVertexP2 {
		float32 position[2];
	};

	/// For 2D lines and structures with per-vertex colour.
	struct BasicVertexP2C4 {
		float32 position[2];
		float32 color[4];
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP2>> {
		static const TagT TAG = tag_from_identifier("2000");
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP2C4>> {
		static const TagT TAG = tag_from_identifier("2400");
	};

	/// For 3D lines and structures.
	struct BasicVertexP3 {
		float32 position[3];
	};

	/// For 3D lines and structures with per-vertex colour.
	struct BasicVertexP3C4 {
		float32 position[3];
		float32 color[4];
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP3>> {
		static const TagT TAG = tag_from_identifier("3000");
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP3C4>> {
		static const TagT TAG = tag_from_identifier("3400");
	};

	/// For triangle mesh.
	struct BasicVertexP3N3M2 {
		float32 position[3];
		float32 normal[3];
		float32 mapping[2];
	};

	/// For triangle mesh with per-vertex colour.
	struct BasicVertexP3N3M2C4 : public BasicVertexP3N3M2 {
		float32 color[4];
	};

	/// For triangle mesh with per-vertex colour and weights.
	struct BasicVertexP3N3M2C4W2 : public BasicVertexP3N3M2C4 {
		uint32_t weights[2];
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP3N3M2>> {
		static const TagT TAG = tag_from_identifier("3320");
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP3N3M2C4>> {
		static const TagT TAG = tag_from_identifier("3324");
	};

	template<>
	struct BlockTraits<Vertices<BasicVertexP3N3M2C4W2>> {
		static const TagT TAG = tag_from_identifier("3326");
	};
}

#endif
