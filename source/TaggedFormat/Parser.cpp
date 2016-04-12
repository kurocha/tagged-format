//
//  Parser.cpp
//  This file is part of the "TaggedFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Parser.hpp"
#include "MemoryBuffer.hpp"

#include "Table.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cstring>

namespace TaggedFormat
{
	namespace Parser
	{
		InvalidSequenceError::InvalidSequenceError(const std::string & message) : std::runtime_error(message)
		{
		}

		namespace IO {
			// Integral input helper
			template <typename IntegralT = uint8_t>
			struct Integer {
				IntegralT * value;
			};

			template <typename IntegralT>
			static std::istream & operator>>(std::istream & input, Integer<IntegralT> & integer) {
				input >> integer.value;

				return input;
			}

			template <typename IntegralT>
			static std::ostream & operator<<(std::ostream & output, const Integer<IntegralT> & integer) {
				output << integer.value;

				return output;
			}

			static std::istream & operator>>(std::istream & input, const Integer<uint8_t> & integer) {
				uint16_t value = 0;
				input >> value;
				*integer.value = value;

				return input;
			}

			static std::ostream & operator<<(std::ostream & output, const Integer<uint8_t> & integer) {
				uint16_t value = *integer.value;
				output << value;

				return output;
			}

			template <typename IntegralT>
			Integer<IntegralT> integral(IntegralT & value) {
				return Integer<IntegralT>{&value};
			}

			// *** Matrix I/O ***
			
			std::istream & operator>>(std::istream & input, float32 (&matrix)[16]) {
				for (std::size_t i = 0; i < 16 && input.good(); i += 1) {
					input >> matrix[i];
				}

				return input;
			}

			std::ostream & operator<<(std::ostream & output, const float32 (&matrix)[16]) {
				for (std::size_t i = 0; i < 16 && output.good(); i += 1) {
					if (i > 0) output << " ";

					output << matrix[i];
				}

				return output;
			}

			// *** Vertex I/O ***

			std::istream & operator>>(std::istream & input, Index16 & index) {
				input >> index.value;
				
				return input;
			}

			std::istream & operator>>(std::istream & input, Index32 & index) {
				input >> index.value;
				
				return input;
			}
			
			std::istream & operator>>(std::istream & input, VertexP3N3 & vertex) {
				input >> vertex.position[0] >> vertex.position[1] >> vertex.position[2];
				input >> vertex.normal[0] >> vertex.normal[1] >> vertex.normal[2];
				
				return input;
			}
			
			std::istream & operator>>(std::istream & input, VertexP3N3M2 & vertex) {
				input >> (VertexP3N3 &)vertex;
				input >> vertex.mapping[0] >> vertex.mapping[1];
				
				return input;
			}

			std::istream & operator>>(std::istream & input, VertexP3N3M2B4 & vertex) {
				input >> (VertexP3N3M2 &)vertex;

				input >> integral(vertex.bones[0]) >> integral(vertex.bones[1]) >> integral(vertex.bones[2]) >> integral(vertex.bones[3]);
				input >> vertex.weights[0] >> vertex.weights[1] >> vertex.weights[2] >> vertex.weights[3];
				
				return input;
			}

			std::istream & operator>>(std::istream & input, VertexP3N3M2C4 & vertex) {
				input >> (VertexP3N3M2 &)vertex;
				input >> vertex.color[0] >> vertex.color[1] >> vertex.color[2] >> vertex.color[3];
				
				return input;
			}

			std::istream & operator>>(std::istream & input, NamedAxis & axis) {
				input >> axis.name;
				input >> axis.translation[0] >> axis.translation[1] >> axis.translation[2];
				input >> axis.rotation[0] >> axis.rotation[1] >> axis.rotation[2] >> axis.rotation[3];
				
				return input;
			}

			std::istream & operator>>(std::istream & input, SkeletonBone & bone) {
				input >> bone.name;
				input >> integral(bone.parent);
				input >> bone.transform;

				return input;
			}

			std::istream & operator>>(std::istream & input, SkeletonAnimationKeyFrame & frame) {
				input >> integral(frame.bone);

				{
					std::string interpolation_method;
					input >> interpolation_method;

					if (interpolation_method == "bezier")
						frame.interpolation = SkeletonAnimationKeyFrame::Interpolation::BEZIER;
					else
						frame.interpolation = SkeletonAnimationKeyFrame::Interpolation::LINEAR;
				}

				input >> frame.time;
				input >> frame.transform;

				return input;
			}

			std::ostream & operator<<(std::ostream & output, const Index16 & index) {
				output << index.value;

				return output;
			}

			std::ostream & operator<<(std::ostream & output, const Index32 & index) {
				output << index.value;

				return output;
			}

			std::ostream & operator<<(std::ostream & output, const VertexP3N3 & vertex) {
				output << "P=(" << vertex.position[0] << ", " << vertex.position[1] << ", " << vertex.position[2] << ")";
				output << " N=(" << vertex.normal[0] << ", " << vertex.normal[1] << ", " << vertex.normal[2] << ")";
				
				return output;
			}
			
			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2 & vertex) {
				output << (VertexP3N3 &)vertex;
				output << " M=(" << vertex.mapping[0] << ", " << vertex.mapping[1] << ")";
				
				return output;
			}
			
			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2C4 & vertex) {
				output << (VertexP3N3M2 &)vertex;
				output << " C=(" << vertex.color[0] << ", " << vertex.color[1] << ", " << vertex.color[2] << ", " << vertex.color[3] << ")";
				
				return output;
			}

			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2B4 & vertex) {
				output << (VertexP3N3M2 &)vertex;

				output << " B=(";

				for (std::size_t i = 0; i < 4; i += 1) {
					if (i > 0) output << ", ";

					output << (uint16_t)vertex.bones[i] << "*" << vertex.weights[i];
				}

				output << ")";
				
				return output;
			}

			std::ostream & operator<<(std::ostream & output, const NamedAxis & axis) {
				output << axis.name;
				output << " T=(" << axis.translation[0] << ", " << axis.translation[1] << ", " << axis.translation[2] << ")";
				output << " R=(" << axis.rotation[0] << ", " << axis.rotation[1] << ", " << axis.rotation[2] << ", " << axis.rotation[3] << ")";
				
				return output;
			}

			std::ostream & operator<<(std::ostream & output, const SkeletonBone & bone) {
				output << " Bone=(id: " << (uint16_t)bone.parent << " transform: " << bone.transform << ")";

				return output;
			}

			std::ostream & operator<<(std::ostream & output, const SkeletonAnimationKeyFrame & frame) {
				output << " Frame=(bone-id: " << integral(frame.bone) << " @ " << frame.time << "s " << SkeletonAnimationKeyFrame::name_for_interpolation(frame.interpolation) << " -> " << frame.transform << ")";

				return output;
			}
		}

//MARK: -

		using namespace IO;
	
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
				mesh_block->layout = Mesh::Layout::TRIANGLES;
			else if (layout == "triangle-strip")
				mesh_block->layout = Mesh::Layout::TRIANGLE_STRIP;
			else
				throw InvalidSequenceError(std::string("Unknown mesh layout ") + layout);
			
			Context child(this);
			child.parse();
			
			mesh_block->indices_offset = child.lookup("indices");
			mesh_block->vertices_offset = child.lookup("vertices");
			mesh_block->axes_offset = child.lookup("axes");
			mesh_block->metadata_offset = child.lookup("metadata");
			
			return mesh_block;
		}

		OffsetT Context::parse_skeleton() {
			auto skeleton_block = _writer->append<Skeleton>();

			Context child(this);
			child.parse();

			skeleton_block->bones_offset = child.lookup("bones");
			skeleton_block->sequences_offset = child.lookup("sequences");

			return skeleton_block;
		}

		OffsetT Context::parse_animation() {
			auto animation_block = _writer->append<SkeletonAnimation>();

			_input >> animation_block->start_time >> animation_block->end_time;

			Context child(this);
			child.parse();

			animation_block->key_frames_offset = child.lookup("key-frames");

			return animation_block;
		}

		OffsetT Context::parse_geometry_instance()
		{
			auto geometry_instance_block = _writer->append<GeometryInstance>();

			Context child(this);
			child.parse();

			geometry_instance_block->mesh_offset = child.lookup("mesh");
			geometry_instance_block->skeleton_offset = child.lookup("skeleton");
			geometry_instance_block->material_offset = child.lookup("material");

			return geometry_instance_block;
		}

		OffsetT Context::parse_node() {
			std::string name;
			float32 transform[16];

			_input >> name >> transform;

			Context child(this);
			child.parse();

			std::size_t buffer_size = Node::array_size(child._offsets.size());
			auto node_block = _writer->append<Node>(buffer_size);

			node_block->name = name;
			std::copy(transform, transform+16, node_block->transform);

			std::memcpy(node_block->end(sizeof(Node)), child._offsets.data(), buffer_size);

			return node_block;
		}

		OffsetT Context::parse_camera() {
			auto camera_block = _writer->append<Camera>();
			
			_input >> camera_block->view_matrix >> camera_block->projection_matrix;

			return camera_block;
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
				memset(&value, 0, sizeof(value));

				input >> value;
				//std::cerr << "(parse items) <- " << value << std::endl;
				
				if (input.fail()) {
					std::stringstream buffer;

					buffer << "Could not parse item: " << typeid(ElementT).name();
					buffer << " -> " << value;

					throw InvalidSequenceError(buffer.str());
				}
				
				//std::cerr << "Appending value : " << value << std::endl;
				
				items.push_back(value);
			}
		}
		
		OffsetT Context::parse_offset_table() {
			Context child(this);
			child.parse();
			
			auto dictionary_block = _writer->append<OffsetTable>(OffsetTable::array_size(child._names.size()));
			
			std::size_t i = 0;
			auto entries = dictionary_block->begin();
			for (auto pair : child._names) {
				entries[i].name = pair.first;
				entries[i].offset = pair.second;
				
				//std::cerr << "Table " << entries[i].name << " = " << entries[i].offset << std::endl;
				
				i += 1;
			}
			
			return dictionary_block;
		}
		
		template <typename BlockT>
		OffsetT Context::parse_block() {
			std::vector<typename BlockT::ElementT> items;
			parse_items(_input, items);
			
			std::size_t buffer_size = sizeof(typename BlockT::ElementT) * items.size();
			auto block = _writer->append<BlockT>(buffer_size);
			std::memcpy(block->begin(), items.data(), buffer_size);
			
			return block;
		}
		
		OffsetT Context::parse_array() {
			std::string value_type, symbol;
			
			_input >> value_type;
			
			if (value_type == "index16") {
				return parse_block<Array<Index16>>();
			} else if (value_type == "index32") {
				return parse_block<Array<Index32>>();
			} else if (value_type == "vertex-p3n3") {
				return parse_block<Array<VertexP3N3>>();
			} else if (value_type == "vertex-p3n3m2") {
				return parse_block<Array<VertexP3N3M2>>();
			} else if (value_type == "vertex-p3n3m2b4") {
				return parse_block<Array<VertexP3N3M2B4>>();
			} else if (value_type == "vertex-p3n3m2c4") {
				return parse_block<Array<VertexP3N3M2C4>>();
			} else if (value_type == "axis") {
				return parse_block<Axes>();
			} else if (value_type == "skeleton-bone") {
				return parse_block<Array<SkeletonBone>>();
			} else if (value_type == "skeleton-animation-key-frame") {
				return parse_block<Array<SkeletonAnimationKeyFrame>>();
			}
			
			throw InvalidSequenceError(std::string("Could not parse input ") + value_type);
		}
		
		void Context::parse() {
			//std::cerr << "-> Entering parse" << std::endl;
			
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
				
				//std::cerr << "Parsing " << symbol << " named " << name << std::endl;

				OffsetT offset = 0;
				if (symbol.front() == '$') {
					// Lookup the offset - no parsing required:
					offset = lookup(std::string(symbol.begin() + 1, symbol.end()));
					
					//std::cerr << "Found offset " << offset << " for name " << symbol << std::endl;
				} else if (symbol == "mesh") {
					offset = parse_mesh();	
				} else if (symbol == "array") {
					offset = parse_array();
				} else if (symbol == "offset-table") {
					offset = parse_offset_table();
				} else if (symbol == "skeleton") {
					offset = parse_skeleton();
				} else if (symbol == "skeleton-animation") {
					offset = parse_animation();
				} else if (symbol == "node") {
					offset = parse_node();
				} else if (symbol == "geometry-instance") {
					offset = parse_geometry_instance();
				} else if (symbol == "camera") {
					offset = parse_camera();
				} else {
					throw InvalidSequenceError("Could not parse input");
				}

				if (named) {
					_names[name] = offset;
				}

				_offsets.push_back(offset);
			}
			
			//std::cerr << "<- Exiting parse; names defined = " << _names.size() << std::endl;
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
				
				//std::cerr << "Top block at offset " << header->top_offset << std::endl;
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
