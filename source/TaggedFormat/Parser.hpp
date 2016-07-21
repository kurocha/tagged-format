//
//  Parser.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include "Mesh.hpp"
#include "Skeleton.hpp"
#include "Axes.hpp"
#include "Scene.hpp"
#include "Camera.hpp"

#include "Writer.hpp"

#include <iosfwd>
#include <string>

#include <map>
#include <vector>

#include <stdexcept>

namespace TaggedFormat
{
	class MemoryBuffer;

	/// Supports text format parsing, used in particular for the tmf-tool.
	namespace Parser
	{
		class InvalidSequenceError : public std::runtime_error {
		public:
			explicit InvalidSequenceError(const std::string & message);
		};

		namespace IO {
			// *** Matrix I/O ***
			
			template <std::size_t N>
			std::istream & operator>>(std::istream & input, float32 (&matrix)[N]) {
				for (std::size_t i = 0; i < N && input.good(); i += 1) {
					input >> matrix[i];
				}

				return input;
			}

			template <std::size_t N>
			std::ostream & operator<<(std::ostream & output, const float32 (&matrix)[N]) {
				for (std::size_t i = 0; i < N && output.good(); i += 1) {
					if (i > 0) output << " ";

					output << matrix[i];
				}

				return output;
			}

			std::istream & operator>>(std::istream & input, Index16 & index);
			std::istream & operator>>(std::istream & input, Index32 & index);
			std::istream & operator>>(std::istream & input, VertexP3N3 & vertex);
			std::istream & operator>>(std::istream & input, VertexP3N3M2 & vertex);
			std::istream & operator>>(std::istream & input, VertexP3N3M2B4 & vertex);
			std::istream & operator>>(std::istream & input, VertexP3N3M2C4 & vertex);
			std::istream & operator>>(std::istream & input, NamedAxis & axis);
			std::istream & operator>>(std::istream & input, SkeletonBone & bone);
			std::istream & operator>>(std::istream & input, SkeletonAnimationKeyFrame & frame);

			std::ostream & operator<<(std::ostream & output, const Index16 & index);
			std::ostream & operator<<(std::ostream & output, const Index32 & index);
			std::ostream & operator<<(std::ostream & output, const VertexP3N3 & vertex);
			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2 & vertex);
			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2C4 & vertex);
			std::ostream & operator<<(std::ostream & output, const VertexP3N3M2B4 & vertex);
			std::ostream & operator<<(std::ostream & output, const NamedAxis & axis);
			std::ostream & operator<<(std::ostream & output, const SkeletonBone & bone);
			std::ostream & operator<<(std::ostream & output, const SkeletonAnimationKeyFrame & frame);

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
		}

		class Context {
		public:
			typedef std::map<std::string, OffsetT> NamesMapT;
			typedef std::vector<OffsetT> OffsetsT;
			
		protected:
			Writer * _writer;
			std::istream & _input;

			NamesMapT _names;
			OffsetsT _offsets;

			Context * _parent;
			
			template <typename BlockT>
			OffsetT parse_block();
			
			OffsetT parse_offset_table();
			
		public:
			Context(Writer * writer, std::istream & input);
			Context(Context * parent);
			
			OffsetT lookup(std::string name);
			
			OffsetT parse_mesh();
			OffsetT parse_array();

			OffsetT parse_skeleton();
			OffsetT parse_animation();

			OffsetT parse_geometry_instance();
			OffsetT parse_node();
			OffsetT parse_camera();

			void parse();
		};

		void serialize(std::istream & input, ResizableBuffer & output);
		void serialize(std::istream & input, std::ostream & output);
	};
}
