//
//  Parser.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_FORMAT_PARSER_H
#define _TAGGED_FORMAT_PARSER_H

#include "Mesh.h"
#include "Skeleton.h"
#include "Axes.h"
#include "Writer.h"

#include <iosfwd>
#include <string>
#include <map>

namespace TaggedFormat {

	class MemoryBuffer;

	/// Supports text format parsing, used in particular for the tmf-tool.
	namespace Parser {

		// *** Structured Input ***
		std::istream & operator>>(std::istream & input, BasicVertexP3N3 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4 & vertex);
		std::istream & operator>>(std::istream & input, NamedAxis & axis);

		// Skeletons
		std::istream & operator>>(std::istream & input, Weights<2>::Vertex & vertex);
		std::istream & operator>>(std::istream & input, Weights<4>::Vertex & vertex);
		std::istream & operator>>(std::istream & input, Bones::Bone & bone);
		std::istream & operator>>(std::istream & input, BoneKeyFrames::Frame & frame);

		// *** Structured Output ***
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3 & vertex);
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3M2 & vertex);
		std::ostream & operator<<(std::ostream & output, const BasicVertexP3N3M2C4 & vertex);
		std::ostream & operator<<(std::ostream & output, const NamedAxis & axis);

		// Skeletons
		std::ostream & operator<<(std::istream & input, const Weights<2>::Vertex & vertex);
		std::ostream & operator<<(std::istream & input, const Weights<4>::Vertex & vertex);
		std::ostream & operator<<(std::istream & input, const Bones::Bone & bone);
		std::ostream & operator<<(std::istream & input, const BoneKeyFrames::Frame & frame);

		
		class Context {
		public:
			typedef std::map<std::string, OffsetT> NamesMapT;
			
		protected:
			Writer * _writer;
			std::istream & _input;
			NamesMapT _names;
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
			
			void parse();
		};

		void serialize(std::istream & input, MemoryBuffer & output);
		void serialize(std::istream & input, std::ostream & output);
	};
}

#endif
