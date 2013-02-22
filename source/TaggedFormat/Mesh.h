//
//  Mesh.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_MESH_H
#define __TAGGED_FORMAT_MESH_H

#include "Block.h"

namespace TaggedFormat {
	/// A mesh block contains indices, vertices and axes.
	struct Mesh : public Block {
		static const TagT TAG = tag_from_identifier("MESH");
		
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

	/// A list of indicies, typically an array of uint16_t or unit32_t.
	template <typename IndexT>
	struct Indices : public Block {
		typedef IndexT ElementT;

		IndexT indices[0];
	};

	struct Indices16 : public Indices<uint16_t> {
		static const TagT TAG = tag_from_identifier("IN16");
	};

	struct Indices32 : public Indices<uint32_t>  {
		static const TagT TAG = tag_from_identifier("IN32");
	};

	/// A list of vertices, typically BasicVertexP3N3M2.
	template <typename VertexT>
	struct Vertices : public Block {
		static const TagT TAG = VertexT::TAG;

		typedef VertexT ElementT;

		VertexT vertices[0];
	};

	// You are recommended to customize this data type to your exact requirements.

	/// For 2D lines and structures.
	struct BasicVertexP2 {
		static const TagT TAG = tag_from_identifier("2000");
		
		float32 position[2];
	};

	/// For 2D lines and structures with per-vertex colour.
	struct BasicVertexP2C4 {
		static const TagT TAG = tag_from_identifier("2400");

		float32 position[2];
		float32 color[4];
	};

	/// For 3D lines and structures.
	struct BasicVertexP3 {
		static const TagT TAG = tag_from_identifier("3000");

		float32 position[3];
	};

	struct BasicVertexP3N3 : public BasicVertexP3 {
		static const TagT TAG = tag_from_identifier("3300");

		float32 normal[3];
	};

	/// For triangle mesh.
	struct BasicVertexP3N3M2 : public BasicVertexP3N3 {
		static const TagT TAG = tag_from_identifier("3320");

		float32 mapping[2];
	};

	/// For triangle mesh with per-vertex colour.
	struct BasicVertexP3N3M2C4 : public BasicVertexP3N3M2 {
		static const TagT TAG = tag_from_identifier("3324");

		float32 color[4];
	};
}

#endif
