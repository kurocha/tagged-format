//
//  Parser.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Parser.h"
#include "MemoryBuffer.h"

#include "Table.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstring>

namespace TaggedFormat {
	namespace Parser {

		std::istream & operator>>(std::istream & input, float32 (&matrix)[16]) {
			for (std::size_t i = 0; i < 16 && input.good(); i += 1) {
				input >> matrix[i];
			}

			return input;
		}

		std::ostream & operator<<(std::ostream & output, const float32 (&matrix)[16]) {
			for (std::size_t i = 0; i < 16 && output.good(); i += 1) {
				output << matrix[i];
			}

			return output;
		}
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3 & vertex) {
			input >> vertex.position[0] >> vertex.position[1] >> vertex.position[2];
			input >> vertex.normal[0] >> vertex.normal[1] >> vertex.normal[2];
			
			return input;
		}
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2 & vertex) {
			input >> (BasicVertexP3N3 &)vertex;
			input >> vertex.mapping[0] >> vertex.mapping[1];
			
			return input;
		}
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4 & vertex) {
			input >> (BasicVertexP3N3M2 &)vertex;
			input >> vertex.color[0] >> vertex.color[1] >> vertex.color[2] >> vertex.color[3];
			
			return input;
		}

		std::istream & operator>>(std::istream & input, NamedAxis & axis) {
			input >> axis.name;
			input >> axis.translation[0] >> axis.translation[1] >> axis.translation[2];
			input >> axis.rotation[0] >> axis.rotation[1] >> axis.rotation[2] >> axis.rotation[3];
			
			return input;
		}

		std::istream & operator>>(std::istream & input, Weights<2>::Vertex & vertex) {
			input >> vertex.bones[0] >> vertex.bones[1];
			input >> vertex.weights[0] >> vertex.weights[1];
			
			return input;
		}

		std::istream & operator>>(std::istream & input, Weights<4>::Vertex & vertex) {
			input >> vertex.bones[0] >> vertex.bones[1] >> vertex.bones[2] >> vertex.bones[3];
			input >> vertex.weights[0] >> vertex.weights[1] >> vertex.weights[2] >> vertex.weights[3];
			
			return input;
		}

		std::istream & operator>>(std::istream & input, Bones::Bone & bone) {
			input >> bone.parent;
			input >> bone.transform;

			return input;
		}

		std::istream & operator>>(std::istream & input, BoneKeyFrames::Frame & frame) {
			input >> frame.bone;

			{
				std::string interpolation_method;
				input >> interpolation_method;

				if (interpolation_method == "bezier")
					frame.interpolation = BoneKeyFrames::Interpolation::BEZIER;
				else
					frame.interpolation = BoneKeyFrames::Interpolation::LINEAR;
			}

			input >> frame.time;

			for (std::size_t i = 0; i < 16 && input.good(); i += 1) {
				input >> frame.transform[i];
			}

			return input;
		}
		
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3 & vertex) {
			output << "P=(" << vertex.position[0] << ", " << vertex.position[1] << ", " << vertex.position[2] << ")";
			output << " N=(" << vertex.normal[0] << ", " << vertex.normal[1] << ", " << vertex.normal[2] << ")";
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3M2 & vertex) {
			output << (BasicVertexP3N3 &)vertex;
			output << " M=(" << vertex.mapping[0] << ", " << vertex.mapping[1] << ")";
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3M2C4 & vertex) {
			output << (BasicVertexP3N3M2 &)vertex;
			output << " C=(" << vertex.color[0] << ", " << vertex.color[1] << ", " << vertex.color[2] << ", " << vertex.color[3] << ")";
			
			return output;
		}

		std::ostream & operator<<(std::ostream & output, const NamedAxis & axis) {
			output << axis.name;
			output << " T=(" << axis.translation[0] << ", " << axis.translation[1] << ", " << axis.translation[2] << ")";
			output << " R=(" << axis.rotation[0] << ", " << axis.rotation[1] << ", " << axis.rotation[2] << ", " << axis.rotation[3] << ")";
			
			return output;
		}

		std::ostream & operator<<(std::ostream & output, const Weights<2>::Vertex & vertex) {
			output << " B=(" << vertex.bones[0] << ", " << vertex.bones[1] << ")";
			output << " W=(" << vertex.weights[0] << ", " << vertex.weights[1] << ")";
			
			return output;
		}

		std::ostream & operator<<(std::ostream & output, const Weights<4>::Vertex & vertex) {
			output << " B=(" << vertex.bones[0] << ", " << vertex.bones[1] << ", " << vertex.bones[2] << ", " << vertex.bones[3] << ")";
			output << " W=(" << vertex.weights[0] << ", " << vertex.weights[1] << ", " << vertex.weights[2] << ", " << vertex.weights[3] << ")";
			
			return output;
		}

		std::ostream & operator<<(std::ostream & output, const Bones::Bone & bone) {
			output << " Bone=(" << bone.parent << " -> " << bone.transform << ")";

			return output;
		}

		std::ostream & operator<<(std::ostream & output, BoneKeyFrames::Frame & frame) {
			output << " Frame=(" << frame.bone << " @ " << frame.time << "[" << BoneKeyFrames::name_for_interpolation(frame.interpolation) << "]" << " -> " << frame.transform << ")";

			return output;
		}		
		
//MARK: -
	
		Context::Context(Writer * writer, std::istream & input) : _writer(writer), _input(input), _parent(nullptr) {
		}
		
		Context::Context(Context * parent) : _writer(parent->_writer), _input(parent->_input), _parent(parent) {
		}
		
		OffsetT Context::lookup(std::string name) {
			NamesMapT::iterator offset = _names.find(name);
			
			if (offset != _names.end()) {
				return offset->second;
			} else if (_parent) {
				return _parent->lookup(name);
			}
			
			return 0;
		}
		
		OffsetT Context::parse_mesh() {			
			auto mesh_block = _writer->append<Mesh>();
			
			std::string layout; 
			_input >> layout;
			
			if (layout == "triangles")
				mesh_block->layout = Mesh::TRIANGLES;
			else if (layout == "triangle-strip")
				mesh_block->layout = Mesh::TRIANGLE_STRIP;
			
			Context child(this);
			child.parse();
			
			mesh_block->indices_offset = child.lookup("indices");
			mesh_block->vertices_offset = child.lookup("vertices");
			mesh_block->axes_offset = child.lookup("axes");
			
			std::cerr << "Mesh: " << mesh_block->indices_offset << ", " << mesh_block->vertices_offset << ", " << mesh_block->axes_offset << std::endl;
			
			return mesh_block;
		}

		OffsetT Context::parse_skeleton() {
			auto skeleton_block = _writer->append<Skeleton>();

			Context child(this);
			child.parse();

			skeleton_block->weights_offset = child.lookup("weights");
			skeleton_block->bones_offset = child.lookup("bones");
			skeleton_block->sequences_offset = child.lookup("sequences");

			std::cerr << "Skeleton: " << skeleton_block->weights_offset << ", " << skeleton_block->bones_offset << ", " << skeleton_block->sequences_offset << std::endl;

			return skeleton_block;
		}

		OffsetT Context::parse_animation() {
			auto animation_block = _writer->append<Animation>();

			_input >> animation_block->start_time >> animation_block->end_time;

			Context child(this);
			child.parse();

			animation_block->key_frames_offset = child.lookup("key-frames");

			std::cerr << "Animation: " << animation_block->start_time << " -> " << animation_block->end_time << std::endl;

			return animation_block;
		}
		
		template <typename ElementT>
		void parse_items(std::istream & input, std::vector<ElementT> & items) {
			std::string symbol;
			
			while (input.good()) {
				std::istream::pos_type current = input.tellg();
				
				input >> symbol;
				
				if (symbol == "end") {
					break;
				}
				
				input.seekg(current);
				ElementT value;
				
				input >> value;
				//std::cerr << "(parse items) <- " << value << std::endl;
				
				if (input.fail()) {
					throw std::runtime_error("Could not parse item");
				}
				
				//std::cerr << "Appending value : " << value << std::endl;
				
				items.push_back(value);
			}
		}
		
		OffsetT Context::parse_offset_table() {
			Context child(this);
			child.parse();
			
			auto dictionary_block = _writer->append<OffsetTable>(sizeof(OffsetTable::ElementT) * child._names.size());
			
			std::size_t i = 0;
			for (auto pair : child._names) {
				NamedOffset named_offset;
				std::strncpy((char *)named_offset.name, pair.first.c_str(), 32);
				named_offset.offset = pair.second;
				
				std::cerr << "Table " << named_offset.name << " = " << named_offset.offset << std::endl;
				
				dictionary_block->entries[i++] = named_offset;
			}
			
			return dictionary_block;
		}
		
		template <typename BlockT>
		OffsetT Context::parse_block() {
			std::vector<typename BlockT::ElementT> items;
			parse_items(_input, items);
			
			std::size_t buffer_size = sizeof(typename BlockT::ElementT) * items.size();
			auto block = _writer->append<BlockT>(buffer_size);
			std::memcpy(block->end(sizeof(BlockT)), items.data(), buffer_size);
			
			return block;
		}
		
		OffsetT Context::parse_array() {
			std::string value_type, symbol;
			
			assert(_input >> value_type);
			//std::cerr << "(parse array) <- " << value_type << std::endl;
			std::cerr << "Value type = " << value_type << std::endl;
			
			if (value_type == "u2") {
				return parse_block<Indices<uint16_t>>();
			} else if (value_type == "u4") {
				return parse_block<Indices<uint32_t>>();
			} else if (value_type == "p3n3m2") {
				return parse_block<Vertices<BasicVertexP3N3M2>>();
			} else if (value_type == "p3n3") {
				return parse_block<Vertices<BasicVertexP3N3>>();
			} else if (value_type == "axis") {
				return parse_block<Axes>();
			} else if (value_type == "b2w2") {
				return parse_block<Weights<2>>();
			} else if (value_type == "b4w4") {
				return parse_block<Weights<4>>();
			} else if (value_type == "bone") {
				return parse_block<Bones>();
			} else if (value_type == "bone-key-frame") {
				return parse_block<BoneKeyFrames>();
			}
			
			throw std::runtime_error("Could not parse input");
		}
		
		void Context::parse() {
			std::cerr << "-> Entering parse" << std::endl;
			
			while (_input.good()) {
				std::string symbol, name;
				
				bool named = false;
				_input >> symbol;
				//std::cerr << "(parse) <- " << symbol << std::endl;
				
				if (_input.eof() || symbol == "end") {
					break;
				}
				
				// Check to see if this node has a name:
				if (symbol.back() == ':') {
					symbol.pop_back();
					name = symbol;
					named = true;
					
					assert(_input >> symbol);
					//std::cerr << "(parse name) <- " << symbol << std::endl;
				}
				
				std::cerr << "Parsing " << symbol << " named " << name << std::endl;
				
				OffsetT offset = 0;
				if (symbol.front() == '$') {
					// Lookup the offset - no parsing required:
					offset = lookup(std::string(symbol.begin() + 1, symbol.end()));
					
					std::cerr << "Found offset " << offset << " for name " << symbol << std::endl;
				} else if (symbol == "mesh") {
					offset = parse_mesh();	
				} else if (symbol == "array") {
					offset = parse_array();
				} else if (symbol == "offset-table") {
					offset = parse_offset_table();
				} else if (symbol == "skeleton") {
					offset = parse_skeleton();
				} else if (symbol == "animation") {
					offset = parse_animation();
				}

				if (named) {
					_names[name] = offset;
				}
			}
			
			std::cerr << "<- Exiting parse; names defined = " << _names.size() << std::endl;
		}
		
		void serialize(std::istream & input, MemoryBuffer & output) {
			using namespace TaggedFormat;
			
			// Initialize memory buffer:
			Writer writer(&output);
			auto header = writer.header();
			
			{
				// Parse input:
				Context context(&writer, input);
				context.parse();
				
				header->top_offset = context.lookup("top");

				if (header->top_offset == 0) {
					std::cerr << "Warning: Could not find block for top offset!" << std::endl;
				}
				
				std::cerr << "Top block at offset " << header->top_offset << std::endl;
			}
		}

		void serialize(std::istream & input, std::ostream & output) {
			MemoryBuffer memory_buffer(1024);

			serialize(input, memory_buffer);

			{
				// Write buffer to output:
				Buffer buffer = memory_buffer.buffer();
				output.write((char *)buffer.begin, buffer.size);
			}
		}

	}		
}
