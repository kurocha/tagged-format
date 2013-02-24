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

		/// The offset of the indices array.
		OffsetT indices_offset;

		/// The offset of the vertices array.
		OffsetT vertices_offset;

		/// The offset of the axes array.
		OffsetT axes_offset;

		/// A table for additional metadata.
		OffsetT metadata_offset;

		/// @returns a human readable name for a given layout.
		static std::string name_for_layout(Layout layout);
	};

	struct Index16 {
		static const TagT TAG = tag_from_identifier("IN16");

		uint16_t value;
	};

	struct Index32 {
		static const TagT TAG = tag_from_identifier("IN32");

		uint32_t value;
	};

	/// For 2D lines and structures.
	struct VertexP2 {
		static const TagT TAG = tag_from_identifier("2000");
		
		float32 position[2];
	};

	/// For 2D lines and structures with per-vertex colour.
	struct VertexP2C4 : public VertexP2 {
		static const TagT TAG = tag_from_identifier("2400");

		float32 color[4];
	};

	/// For 3D lines and structures.
	struct VertexP3 {
		static const TagT TAG = tag_from_identifier("3000");

		float32 position[3];
	};

	struct VertexP3N3 : public VertexP3 {
		static const TagT TAG = tag_from_identifier("3300");

		float32 normal[3];
	};

	/// For triangle mesh.
	struct VertexP3N3M2 : public VertexP3N3 {
		static const TagT TAG = tag_from_identifier("3320");

		float32 mapping[2];
	};

	/// For triangle mesh with per-vertex colour.
	struct VertexP3N3M2C4 : public VertexP3N3M2 {
		static const TagT TAG = tag_from_identifier("3324");

		float32 color[4];
	};
}

#endif
