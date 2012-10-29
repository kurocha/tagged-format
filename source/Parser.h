//
//  Parser.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_PARSER_H
#define _TAGGED_MODEL_FORMAT_PARSER_H

#include "Mesh.h"
#include "Axes.h"
#include "Writer.h"

#include <iosfwd>
#include <string>
#include <map>

namespace TaggedFormat {

	/// Supports text format parsing, used in particular for the tmf-tool.
	namespace Parser {
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4W2 & vertex);
		std::istream & operator>>(std::istream & input, NamedAxis & axis);
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2 & vertex);
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4 & vertex);
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4W2 & vertex);
		std::ostream & operator<<(std::ostream & output, NamedAxis & axis);
		
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
			
			void parse();
		};
		
		void serialize(std::istream & input, std::ostream & output);
	};
}

#endif
