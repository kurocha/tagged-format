//
//  main.cpp
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>

#include "../TaggedModelFormat/Parser.h"
#include "../TaggedModelFormat/Reader.h"
#include "../TaggedModelFormat/MemoryBuffer.h"

namespace {
	using namespace TaggedModelFormat;
	
	template <typename ElementT>
	std::ostream & operator<<(std::ostream & output, std::vector<ElementT> & items) {
		output << "[";
		
		for (auto item : items) {
			output << item;
			
			if (item != items.back())
				output << ", ";
		}
		
		output << "]";
		
		return output;
	}

	template <typename ElementT>
	void dump_indices(Block * block, std::ostream & output) {
		Indices<ElementT> * indices_block = (Indices<ElementT> *)block;
		
		std::vector<ElementT> items(indices_block->indices, (ElementT *)indices_block->end());
		
		output << "Indices: " << items << std::endl;
	}
	
	template <typename ElementT>
	void dump_vertices(Block * block, std::ostream & output) {
		using TaggedModelFormat::Parser::operator<<;
		
		Vertices<ElementT> * vertices_block = (Vertices<ElementT> *)block;
		
		std::vector<ElementT> items(vertices_block->vertices, (ElementT *)vertices_block->end());
		
		for (std::size_t i = 0; i < items.size(); i += 1) {
			output << i << std::endl << items[i] << std::endl;
		}
	}

	void dump_mesh(Mesh * mesh, Reader & reader, std::ostream & output) {
		if (mesh->indices_offset) {
			Block * block = reader.block_at_offset(mesh->indices_offset);
			
			if (block->tag == BlockTraits<Indices<uint16_t>>::TAG) {
				dump_indices<uint16_t>(block, output);
			} else if (block->tag == BlockTraits<Indices<uint32_t>>::TAG) {
				dump_indices<uint32_t>(block, output);
			}
		}
		
		if (mesh->vertices_offset) {
			Block * block = reader.block_at_offset(mesh->vertices_offset);
			
			if (block->tag == BlockTraits<Vertices<BasicVertexP3N3M2>>::TAG) {
				dump_vertices<BasicVertexP3N3M2>(block, output);
			} else if (block->tag == BlockTraits<Vertices<BasicVertexP3N3M2C4>>::TAG) {
				dump_vertices<BasicVertexP3N3M2C4>(block, output);
			} else if (block->tag == BlockTraits<Vertices<BasicVertexP3N3M2C4W2>>::TAG) {
				dump_vertices<BasicVertexP3N3M2C4W2>(block, output);
			} 
		}
	}

	void dump(Buffer buffer, std::ostream & output) {
		Reader reader(buffer);
		
		Header * header = reader.header();
		
		output << "Header: " << reader.tag_name(header->tag) << ", " << header->magic << " (" << header->size << ")" << std::endl;
		
		Block * block = reader.block_at_offset(header->top_offset);
		
		std::cerr << "Top: " << reader.tag_name(block->tag) << "(@" << header->top_offset << ")" << std::endl;
		
		if (block->tag == BlockTraits<Mesh>::TAG) {
			dump_mesh((Mesh *)block, reader, output);
		}
	}
}

int main(int argc, const char * argv[]) {
	using namespace TaggedModelFormat;
	
	for (std::size_t i = 0; i < argc; i += 1) {
		std::string argument = argv[i];
		
		if (argument == "--help") {
			std::cerr << argv[0] << " Copyright 2012 Samuel Williams. No warranty." << std::endl;
			std::cerr << "\t--text-to-binary [input-text-path] [output-binary-path]" << std::endl;
			std::cerr << "\t--dump-binary [input-binary-path]" << std::endl;
		}
		
		else if (argument == "--text-to-binary") {
			assert(i + 2 < argc);
			
			std::ifstream input(argv[i+1]);
			std::ofstream output(argv[i+2]);
			
			Parser::serialize(input, output);
			
			i += 2;
		}
		
		else if (argument == "--dump-binary") {
			assert(i + 1 < argc);
			
			MemoryBuffer memory_buffer;
			memory_buffer.read_from_file(argv[i+1]);
			
			dump(memory_buffer.buffer(), std::cout);
			
			i += 1;
		}
	}
	
	if (argc == 1) {
		std::ifstream input("/Users/samuel/Documents/Programming/Graphics/TaggedModelFormat/TMF Tool/cube.tagged-model-text");
		std::ofstream output("dump.tagged-model-binary");
		
		Parser::serialize(input, output);
	}
}
