//
//  Parser.cpp
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Parser.h"
#include "MemoryBuffer.h"

#include <sstream>
#include <vector>
#include <assert.h>

namespace TaggedModelFormat {
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
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2 & vertex) {
			output << "P = " << vertex.position[0] << ", " << vertex.position[1] << ", " << vertex.position[2] << std::endl;
			output << "N = " << vertex.normal[0] << ", " << vertex.normal[1] << ", " << vertex.normal[2] << std::endl;
			output << "M = " << vertex.mapping[0] << ",  " << vertex.mapping[1] << std::endl;
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4 & vertex) {
			output << (BasicVertexP3N3M2 &)vertex;
			output << "C = " << vertex.color[0] << ", " << vertex.color[1] << ", " << vertex.color[2] << ", " << vertex.color[3] << std::endl;
			
			return output;
		}
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4W2 & vertex) {
			output << (BasicVertexP3N3M2C4 &)vertex;
			output << "W = " << vertex.weights[0] << ", " << vertex.weights[1] << std::endl;
			
			return output;
		}
		
#pragma mark -
	
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
			
			Context child(*this);
			
			// Parse expressions
			child.parse();
			
			mesh_block->indices_offset = child.lookup("indices");
			mesh_block->vertices_offset = child.lookup("vertices");
			
			std::cerr << "Mesh: " << mesh_block->indices_offset << " => " << mesh_block->vertices_offset << std::endl;
			
			return mesh_block;
		}
		
		template <typename ElementT>
		void parse_items(std::istream & input, std::vector<ElementT> & items) {
			std::string symbol;
			
			while (input.good()) {
				ElementT value;
				
				input >> value;
				//std::cerr << "(parse items) <- " << value << std::endl;
				
				if (input.fail()) {
					input.clear();
					
					assert(input >> symbol);
					assert(symbol == "end");
					
					break;
				}
				
				//std::cerr << "Appending value : " << value << std::endl;
				
				items.push_back(value);
			}
		}
		
		template <typename ElementT>
		OffsetT Context::parse_indices() {
			std::vector<ElementT> items;
			parse_items(_input, items);
			
			std::size_t buffer_size = sizeof(ElementT) * items.size();
			auto indices_block = _writer->append<Indices<ElementT>>(buffer_size);
			memcpy(indices_block->indices, items.data(), buffer_size);
			
			return indices_block;
		}
		
		template <typename ElementT>
		OffsetT Context::parse_vertices() {
			std::vector<ElementT> items;
			parse_items(_input, items);
			
			std::size_t buffer_size = sizeof(ElementT) * items.size();
			auto vertices_block = _writer->append<Vertices<ElementT>>(buffer_size);
			memcpy(vertices_block->vertices, items.data(), buffer_size);
			
			return vertices_block;
		}
		
		OffsetT Context::parse_array() {
			std::string value_type, symbol;
			
			assert(_input >> value_type);
			//std::cerr << "(parse array) <- " << value_type << std::endl;
			std::cerr << "Value type = " << value_type << std::endl;
			
			if (value_type == "2u") {
				return parse_indices<uint16_t>();
			} else if (value_type == "4u") {
				return parse_indices<uint32_t>();
			} else if (value_type == "3p3n2m") {
				return parse_vertices<BasicVertexP3N3M2>();
			}
			
			throw std::runtime_error("Could not parse input");
		}
		
		void Context::parse() {			
			while (_input.good()) {
				std::string symbol, name;
				
				bool named = false;
				_input >> symbol;
				//std::cerr << "(parse) <- " << symbol << std::endl;
				
				if (_input.eof() || symbol == "end") {
					return;
				}
				
				// Check to see if this node has a name:
				if (symbol.back() == ':') {
					symbol.pop_back();
					name = symbol;
					named = true;
					
					assert(_input >> symbol);
					//std::cerr << "(parse name) <- " << symbol << std::endl;
				}
				
				std::cerr << "Parsing " << symbol << " for " << name << std::endl;
				
				OffsetT offset = 0;
				if (symbol.front() == '$') {
					// Lookup the offset - no parsing required:
					offset = lookup(std::string(symbol.begin() + 1, symbol.end()));
				} else if (symbol == "mesh") {
					offset = parse_mesh();	
				} else if (symbol == "array") {
					offset = parse_array();
				}
				
				if (named) {
					_names[name] = offset;
				}
			}
		}
		
		void serialize(std::istream & input, std::ostream & output) {
			using namespace TaggedModelFormat;
			
			// Initialize memory buffer:
			MemoryBuffer memory_buffer(1024);
			Writer writer(&memory_buffer);
			auto header = writer.header();
			
			{
				// Parse input:
				Context context(&writer, input);
				context.parse();
				
				header->top_offset = context.lookup("top");
			}
			
			{
				// Write buffer to output:
				Buffer buffer = memory_buffer.buffer();
				output.write((char *)buffer.begin, buffer.size);
			}
		}

	}		
}
