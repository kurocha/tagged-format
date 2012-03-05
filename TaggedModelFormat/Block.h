//
//  Block.h
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
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
	
	typedef float float32;
	typedef double float64;
	
	template <typename BlockT>
	struct BlockTraits {	
	};
		
	struct Block {
		uint32_t tag;
		
		// Total size of block including this structure.
		OffsetT size;
		
		void * end() {
			return (ByteT *)this + size;
		}
	};
	
	struct Header : public Block {
		uint32_t magic;
		OffsetT top_offset;
	};
	
	template<>
	struct BlockTraits<Header> {
		static const uint32_t TAG = 'tmv1';
	};
		
	struct Mesh : public Block {
		enum Layout {
			TRIANGLES = 1, 
			TRIANGLE_STRIP
		};
		
		uint64_t layout;
		OffsetT indices_offset;
		OffsetT vertices_offset;
	};
	
	template<>
	struct BlockTraits<Mesh> {
		static const uint32_t TAG = 'mesh';
	};
	
	struct External : public Block {
		// Null terminated URL referencing external resource.
		unsigned char url[0];
	};
	
	template<>
	struct BlockTraits<External> {
		static const uint32_t TAG = 'extr';
	};
	
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
	
	template <typename VertexT>
	struct Vertices : public Block {
		typedef VertexT ElementT;
		
		VertexT vertices[0];
	};
	
	// You are recommended to customize this data type to your exact requirements.
	
	// For 2D lines and structures
	struct BasicVertexP2 {
		float32 position[2];
	};
	
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
	
	// For 3D lines and structures
	struct BasicVertexP3 {
		float32 position[3];
	};
	
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
	
	// For triangle mesh
	struct BasicVertexP3N3M2 {
		float32 position[3];
		float32 normal[3];
		float32 mapping[2];
	};
	
	struct BasicVertexP3N3M2C4 : public BasicVertexP3N3M2 {
		float32 color[4];
	};
	
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
	
	template <typename BlockT>
	void clear(BlockT & block) {
		memset(&block, 0, sizeof(BlockT));
		
		block.tag = BlockTraits<BlockT>::TAG;
		block.size = sizeof(BlockT);
	}
	
	std::string tag_name(uint32_t tag);
	
}

#endif
