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
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2 & vertex) {
			input >> vertex.position[0] >> vertex.position[1] >> vertex.position[2];
			input >> vertex.normal[0] >> vertex.normal[1] >> vertex.normal[2];
			input >> vertex.mapping[0] >> vertex.mapping[1];
			
			return input;
		}
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4 & vertex) {
			input >> (BasicVertexP3N3M2 &)vertex;
			input >> vertex.color[0] >> vertex.color[1] >> vertex.color[2] >> vertex.color[3];
			
			return input;
		}
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4W2 & vertex) {
			input >> (BasicVertexP3N3M2C4 &)vertex;
			input >> vertex.weights[0] >> vertex.weights[1];
			
			return input;
		}
		
		std::istream & operator>>(std::istream & input, NamedAxis & axis) {
			input >> axis.name;
			input >> axis.translation[0] >> axis.translation[1] >> axis.translation[2];
			input >> axis.rotation[0] >> axis.rotation[1] >> axis.rotation[2] >> axis.rotation[3];
			
			return input;
		}
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2 & vertex) {
			output << "P=(" << vertex.position[0] << ", " << vertex.position[1] << ", " << vertex.position[2] << ")";
			output << " N=(" << vertex.normal[0] << ", " << vertex.normal[1] << ", " << vertex.normal[2] << ")";
			output << " M=(" << vertex.mapping[0] << ", " << vertex.mapping[1] << ")";
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4 & vertex) {
			output << (BasicVertexP3N3M2 &)vertex;
			output << " C=(" << vertex.color[0] << ", " << vertex.color[1] << ", " << vertex.color[2] << ", " << vertex.color[3] << ")";
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4W2 & vertex) {
			output << (BasicVertexP3N3M2C4 &)vertex;
			output << " W=(" << vertex.weights[0] << ", " << vertex.weights[1] << ")";
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, NamedAxis & axis) {
			output << axis.name;
			output << " T=(" << axis.translation[0] << ", " << axis.translation[1] << ", " << axis.translation[2] << ")";
			output << " R=(" << axis.rotation[0] << ", " << axis.rotation[1] << ", " << axis.rotation[2] << ", " << axis.rotation[3] << ")";
			
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
			
			// Parse expressions
			child.parse();
			
			mesh_block->indices_offset = child.lookup("indices");
			mesh_block->vertices_offset = child.lookup("vertices");
			mesh_block->axes_offset = child.lookup("axes");
			
			std::cerr << "Mesh: " << mesh_block->indices_offset << ", " << mesh_block->vertices_offset << ", " << mesh_block->axes_offset << std::endl;
			
			return mesh_block;
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
			
			if (value_type == "2u") {
				return parse_block<Indices<uint16_t>>();
			} else if (value_type == "4u") {
				return parse_block<Indices<uint32_t>>();
			} else if (value_type == "3p3n2m") {
				return parse_block<Vertices<BasicVertexP3N3M2>>();
			} else if (value_type == "axis") {
				return parse_block<Axes>();
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
				} else if (symbol == "dictionary") {
					offset = parse_offset_table();
				}
				
				if (named) {
					_names[name] = offset;
				}
			}
			
			std::cerr << "<- Exiting parse; names defined = " << _names.size() << std::endl;
		}
		
		void serialize(std::istream & input, std::ostream & output) {
			using namespace TaggedFormat;
			
			// Initialize memory buffer:
			MemoryBuffer memory_buffer(1024);
			Writer writer(&memory_buffer);
			auto header = writer.header();
			
			{
				// Parse input:
				Context context(&writer, input);
				context.parse();
				
				header->top_offset = context.lookup("top");
				
				std::cerr << "Top block at offset " << header->top_offset << std::endl;
			}
			
			{
				// Write buffer to output:
				Buffer buffer = memory_buffer.buffer();
				output.write((char *)buffer.begin, buffer.size);
			}
		}

	}		
}
