//
//  Block.h
//  This file is part of the "Tagged Model Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_BLOCK_H
#define _TAGGED_MODEL_FORMAT_BLOCK_H

#include "Buffer.h"

#include <iostream>
#include <string>

namespace TaggedModelFormat {
	
	/// A standard 32-bit floating point data-type.
	typedef float float32;
	
	/// A standard 64-bit floating point data-type.
	typedef double float64;
	
	template <typename BlockT>
	struct BlockTraits {
	};
	
	/// A generic block header.
	struct Block {
		/// The blog identifier.
		uint32_t tag;
		
		// Total size of block including this structure.
		OffsetT size;
		
		/// @returns a pointer to the end of the block in memory based on its size.
		void * end() {
			return (ByteT *)this + size;
		}
		
		/// @returns a pointer to a specific offset from the start of the block.
		void * end(std::size_t offset) {
			return (ByteT *)this + offset;
		}
	};
	
	/// The header typically used at the start of the binary file.
	struct Header : public Block {
		/// A version identifier, typically 42.
		uint32_t magic;
		
		/// The offset to the top block.
		OffsetT top_offset;
	};
	
	template<>
	struct BlockTraits<Header> {
		static const uint32_t TAG = 'tmv2';
	};
	
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
		Layout layout;
		
		/// The offset of the indices block.
		OffsetT indices_offset;
		
		/// The offset of the vertices block.
		OffsetT vertices_offset;
		
		/// The offset of the axes block.
		OffsetT axes_offset;
		
		/// @returns a human readable name for a given layout.
		static std::string name_for_layout(Layout layout);
	};
	
	template<>
	struct BlockTraits<Mesh> {
		static const uint32_t TAG = 'mesh';
	};
	
	/// Reference an external resource. Implementation defined.
	struct External : public Block {
		/// Null terminated URL referencing external resource.
		unsigned char url[0];
	};
	
	template<>
	struct BlockTraits<External> {
		static const uint32_t TAG = 'extr';
	};
	
	/// A list of indicies, typically an array of uint16_t or unit32_t.
	template <typename IndexT>
	struct Indices : public Block {
		typedef IndexT ElementT;
		
		IndexT indices[0];
	};
	
	template<>
	struct BlockTraits<Indices<uint16_t>> {
		static const uint32_t TAG = 'ind2';
	};
	
	template<>
	struct BlockTraits<Indices<uint32_t>> {
		static const uint32_t TAG = 'ind4';
	};
	
	template<>
	struct BlockTraits<Indices<uint64_t>> {
		static const uint32_t TAG = 'ind8';
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
		static const uint32_t TAG = '2000';
	};
	
	template<>
	struct BlockTraits<Vertices<BasicVertexP2C4>> {
		static const uint32_t TAG = '2400';
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
		static const uint32_t TAG = '3000';
	};
	
	template<>
	struct BlockTraits<Vertices<BasicVertexP3C4>> {
		static const uint32_t TAG = '3400';
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
		static const uint32_t TAG = '3320';
	};
	
	template<>
	struct BlockTraits<Vertices<BasicVertexP3N3M2C4>> {
		static const uint32_t TAG = '3324';
	};
	
	template<>
	struct BlockTraits<Vertices<BasicVertexP3N3M2C4W2>> {
		static const uint32_t TAG = '3326';
	};
	
	/// A named axis is used to align meshes together.
	struct Axis {
		unsigned char name[32];
		float32 translation[3];
		// Stored as a quaternion:
		float32 rotation[4];
	};
	
	/// A block containing a list of named axes.
	struct Axes : public Block {
		typedef Axis ElementT;
		
		Axis axes[0];
		
		/// Get an axes with a given name.
		/// @returns nullptr if no axes with the given name was found.
		Axis * axis_named(std::string name);
	};
	
	template<>
	struct BlockTraits<Axes> {
		static const uint32_t TAG = 'axes';
	};
	
	struct NamedOffset {
		unsigned char name[32];
		OffsetT offset;
	};
	
	/// A block containing a key-offset mapping.
	struct Dictionary : public Block {
		typedef NamedOffset ElementT;
		
		NamedOffset entries[0];
		
		/// Get an offset with a given name.
		/// @returns 0 if offset with the given name was found.
		OffsetT offset_named(std::string name);
	};
	
	template<>
	struct BlockTraits<Dictionary> {
		static const uint32_t TAG = 'dict';
	};
	
	/// Return a human readable name for the given tag.
	std::string tag_name(uint32_t tag);
	
	/// Reset a data block with the correct tag and size based on type.
	template <typename BlockT>
	void clear(BlockT & block, OffsetT capacity = 0) {
		block.tag = BlockTraits<BlockT>::TAG;
		block.size = sizeof(BlockT) + capacity;
	}
}

#endif
