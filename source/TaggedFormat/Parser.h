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
#include "Scene.h"

#include "Writer.h"

#include <iosfwd>
#include <string>

#include <map>
#include <vector>

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
		std::istream & operator>>(std::istream & input, Bones::Bone & bone);
		std::istream & operator>>(std::istream & input, SkeletonBoneKeyFrame::Frame & frame);

		class DataType {
		};

		using DataTypePtr = std::shared_ptr<DataType>;

		class BlockTemplate {
		protected:
			TagT _tag;
			std::string _name;
			std::vector<DataTypePtr> _structure;

		public:
			BlockTemplate(TagT tag, std::string name, std::vector<DataTypePtr> structure);
			virtual ~BlockTemplate();

			TagT tag() const { return _tag; }
			const std::string & name() const { return _name; }
			const std::vector<DataTypePtr> & structure() const { return _structure; }

			static std::shared_ptr<BlockTemplate> parse(std::istream & input);
		};

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

			void parse();
		};

		void serialize(std::istream & input, MemoryBuffer & output);
		void serialize(std::istream & input, std::ostream & output);
	};
}

#endif
