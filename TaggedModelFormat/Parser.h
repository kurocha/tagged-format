//
//  Parser.h
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_PARSER_H
#define _TAGGED_MODEL_FORMAT_PARSER_H

#include "Writer.h"

#include <iostream>
#include <string>
#include <map>

namespace TaggedModelFormat {
		
	namespace Parser {
		
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4 & vertex);
		std::istream & operator>>(std::istream & input, BasicVertexP3N3M2C4W2 & vertex);
		
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2 & vertex);
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4 & vertex);
		std::ostream & operator<<(std::ostream & output, BasicVertexP3N3M2C4W2 & vertex);
		
		class Context {
		public:
			typedef std::map<std::string, OffsetT> NamesMapT;
			
		protected:
			Writer * _writer;
			std::istream & _input;
			NamesMapT _names;
			Context * _parent;
			
			template <typename ElementT>
			OffsetT parse_indices();
			
			template <typename ElementT>
			OffsetT parse_vertices();			
			
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