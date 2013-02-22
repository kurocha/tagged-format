//
//  tf-convert.cpp
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>

#include <TaggedFormat/Parser.h>
#include <TaggedFormat/Reader.h>
#include <TaggedFormat/Table.h>
#include <TaggedFormat/MemoryBuffer.h>

namespace {
	using namespace TaggedFormat;
	
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
	
	void dump_offset(Reader * reader, OffsetT offset, std::ostream & output, std::size_t indentation = 0);
	
	template <typename BlockT, bool NEWLINE>
	void dump_block(Reader * reader, BlockT * block, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');

		using TaggedFormat::Parser::operator<<;
				
		std::vector<typename BlockT::ElementT> items((typename BlockT::ElementT *)block->end(sizeof(BlockT)), (typename BlockT::ElementT *)block->end());
		
		if (!NEWLINE) output << indent;
		
		for (auto item : items) {
			if (NEWLINE) output << indent;
			
			output << item;
			
			if (NEWLINE) output << std::endl;
			else output << ' ';
		}
		
		if (!NEWLINE) output << std::endl;
	}
	
	void dump_dictionary(Reader * reader, OffsetTable * block, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		std::vector<NamedOffset> items(block->entries, (NamedOffset *)block->end());
		
		for (auto item : items) {			
			output << indent << item.name << ":" << std::endl;
			
			dump_offset(reader, item.offset, output, indentation);
		}
	}

	void dump_mesh(Reader * reader, Mesh * mesh, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		output << indent << "layout = " << Mesh::name_for_layout(mesh->layout) << std::endl;
		
		output << indent << "indices:" << std::endl;
		dump_offset(reader, mesh->indices_offset, output, indentation);

		output << indent << "vertices:" << std::endl;
		dump_offset(reader, mesh->vertices_offset, output, indentation);

		output << indent << "axes:" << std::endl;
		dump_offset(reader, mesh->axes_offset, output, indentation);
	}
	
	void dump_header(Reader * reader, Header * block, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		output << indent << "<" << reader->tag_name(block->tag);
		output << "; " << block->size << " bytes";
		output << "; " << "magic = " << block->magic;
		output << ">" << std::endl;
		
		dump_offset(reader, block->top_offset, output, indentation);
	}

	void dump_offset(Reader * reader, OffsetT offset, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		Block * block = nullptr;
		
		// The header is always at offset 0.
		if (offset) {
			block = reader->block_at_offset(offset);
		}
		
		if (block) {
			output << indent << "[" << reader->tag_name(block->tag);
			output << "; " << block->size << " bytes";
			output << "; " << "offset = " << offset;
			output << "]" << std::endl;
		} else {
			output << indent << "[null: 0 bytes]" << std::endl;
			
			return;
		}
		
#define TMF_BLOCK_CASE(BlockT, NEWLINE) \
			case BlockT::TAG: \
				dump_block<BlockT, NEWLINE>(reader, (BlockT *)block, output, indentation + 1); \
				break;
		
		switch (block->tag) {
			case Header::TAG:
				dump_header(reader, (Header *)block, output, indentation + 1);
				break;
				
			case Mesh::TAG:
				dump_mesh(reader, (Mesh *)block, output, indentation + 1);
				break;
			
			case OffsetTable::TAG:
				dump_dictionary(reader, (OffsetTable *)block, output, indentation + 1);
				break;
				
			TMF_BLOCK_CASE(Axes, true)
			TMF_BLOCK_CASE(Vertices<BasicVertexP3N3M2>, true)
			TMF_BLOCK_CASE(Vertices<BasicVertexP3N3M2C4>, true)
			TMF_BLOCK_CASE(Vertices<BasicVertexP3N3M2C4W2>, true)
			TMF_BLOCK_CASE(Indices<uint16_t>, false)
			TMF_BLOCK_CASE(Indices<uint32_t>, false)
								
			default:
				output << indent << "<unknown block tag>" << std::endl;
		}
		
#undef TMF_BLOCK_CASE
	}
	
	void dump(Buffer buffer, std::ostream & output) {
		Reader reader(buffer);
		
		dump_header(&reader, reader.header(), std::cout, 0);
	}
}

int main(int argc, const char * argv[]) {
	using namespace TaggedFormat;
	
	std::vector<std::string> arguments(argv+1, argv+argc);
	
	for (std::size_t i = 0; i < arguments.size(); i += 1) {
		auto & argument = arguments[i];
		
		if (argument == "--help") {
			std::cerr << argv[0] << " Copyright 2012 Samuel Williams. No warranty." << std::endl;
			std::cerr << "\t--text-to-binary [input-text-path] [output-binary-path]" << std::endl;
			std::cerr << "\t--dump-binary [input-binary-path]" << std::endl;
		}
		
		else if (argument == "--text-to-binary") {
			assert(i + 2 < arguments.size());
			
			std::ifstream input(arguments[i+1]);
			std::ofstream output(arguments[i+2]);
			
			Parser::serialize(input, output);
			
			i += 2;
		}
		
		else if (argument == "--dump-binary") {
			assert(i + 1 < arguments.size());
			
			MemoryBuffer memory_buffer;
			memory_buffer.read_from_file(arguments[i+1]);
			
			dump(memory_buffer.buffer(), std::cout);
			
			i += 1;
		}
	}
}
