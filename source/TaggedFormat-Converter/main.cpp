//
//  tf-convert.cpp
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <assert.h>
#include <vector>

#include <TaggedFormat/Parser.h>
#include <TaggedFormat/Reader.h>
#include <TaggedFormat/Table.h>
#include <TaggedFormat/MemoryBuffer.h>

namespace {
	using namespace TaggedFormat;
	using namespace TaggedFormat::Parser::IO;

	void dump_offset(Reader * reader, OffsetT offset, std::ostream & output, std::size_t indentation = 0);

	void dump_block(Reader * reader, OffsetTable * block, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		for (auto item : *block) {
			output << indent << item.name << ":" << std::endl;
			
			dump_offset(reader, item.offset, output, indentation);
		}
	}

	void dump_block(Reader * reader, Mesh * mesh, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		output << indent << "layout = " << Mesh::name_for_layout(mesh->layout) << std::endl;
		
		output << indent << "indices:" << std::endl;
		dump_offset(reader, mesh->indices_offset, output, indentation);

		output << indent << "vertices:" << std::endl;
		dump_offset(reader, mesh->vertices_offset, output, indentation);

		output << indent << "axes:" << std::endl;
		dump_offset(reader, mesh->axes_offset, output, indentation);
	}

	void dump_block(Reader * reader, Skeleton * skeleton, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');

		output << indent << "bones:" << std::endl;
		dump_offset(reader, skeleton->bones_offset, output, indentation);

		output << indent << "sequences:" << std::endl;
		dump_offset(reader, skeleton->sequences_offset, output, indentation);
	}

	void dump_block(Reader * reader, SkeletonAnimation * skeleton_animation, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');

		output << indent << "Animation=(" << skeleton_animation->start_time << " -> " << skeleton_animation->end_time << ")" << std::endl;

		output << indent << "key-frames:" << std::endl;
		dump_offset(reader, skeleton_animation->key_frames_offset, output, indentation);
	}

	template <typename ArrayT>
	void dump_array(Reader * reader, ArrayT * array, std::ostream & output, std::size_t indentation, bool separate = true) {
		std::string indent(indentation, '\t');

		if (!separate) output << indent;
		
		for (auto element : *array) {
			if (separate) output << indent;
			
			output << element;
			
			if (separate) output << std::endl;
			else output << ' ';
		}
		
		if (!separate) output << std::endl;
	}
	
	void dump_header(Reader * reader, Header * block, std::ostream & output, std::size_t indentation) {
		std::string indent(indentation, '\t');
		
		output << indent << "<" << block->tag_name();
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
			output << indent << "[" << block->tag_name();
			output << "; " << block->size << " bytes";
			output << "; " << "offset = " << offset;
			output << "]" << std::endl;
		} else {
			output << indent << "[null: 0 bytes]" << std::endl;
			
			return;
		}

		switch (block->tag) {
			case Header::TAG:
				dump_header(reader, (Header *)block, output, indentation + 1);
				break;
				
			case Mesh::TAG:
				dump_block(reader, (Mesh *)block, output, indentation + 1);
				break;
			
			case OffsetTable::TAG:
				dump_block(reader, (OffsetTable *)block, output, indentation + 1);
				break;

			case Index16::TAG:
				dump_array(reader, (Array<Index16> *)block, output, indentation + 1, false);
				break;

			case Index32::TAG:
				dump_array(reader, (Array<Index32> *)block, output, indentation + 1, false);
				break;

			case VertexP3N3M2::TAG:
				dump_array(reader, (Array<VertexP3N3M2> *)block, output, indentation + 1);
				break;

			case VertexP3N3M2B4::TAG:
				dump_array(reader, (Array<VertexP3N3M2B4> *)block, output, indentation + 1);
				break;

			case VertexP3N3M2C4::TAG:
				dump_array(reader, (Array<VertexP3N3M2C4> *)block, output, indentation + 1);
				break;

			case Axes::TAG:
				dump_array(reader, (Axes *)block, output, indentation + 1);
				break;

			case Skeleton::TAG:
				dump_block(reader, (Skeleton *)block, output, indentation + 1);
				break;

			case SkeletonBone::TAG:
				dump_array(reader, (Array<SkeletonBone> *)block, output, indentation + 1);
				break;

			case SkeletonAnimation::TAG:
				dump_block(reader, (SkeletonAnimation *)block, output, indentation + 1);
				break;

			case SkeletonAnimationKeyFrame::TAG:
				dump_array(reader, (Array<SkeletonAnimationKeyFrame> *)block, output, indentation + 1);
				break;

			default:
				output << indent << "<unknown block tag>" << std::endl;
		}
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
			
			try {
				Parser::serialize(input, output);
			} catch (std::exception & exception) {
				std::cerr << exception.what() << std::endl;
			}
			
			i += 2;
		}

		else if (argument == "--dump-binary") {
			assert(i + 1 < arguments.size());
			
			MemoryBuffer memory_buffer;
			memory_buffer.read_from_file(arguments[i+1]);

			std::cout.precision(4);
			std::cout.setf(std::ios::fixed, std::ios::floatfield);
			dump(memory_buffer.buffer(), std::cout);
			
			i += 1;
		}
	}
}
