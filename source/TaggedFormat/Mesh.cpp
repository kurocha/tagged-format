//
//  Mesh.cpp
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Mesh.hpp"

namespace TaggedFormat {
	std::string Mesh::name_for_layout(Layout layout) {
		switch (layout) {
			case Layout::POINTS:
				return "points";

			case Layout::LINES:
				return "lines";

			case Layout::LINE_LOOP:
				return "line-loop";

			case Layout::LINE_STRIP:
				return "line-strip";

			case Layout::TRIANGLES:
				return "triangles";

			case Layout::TRIANGLE_STRIP:
				return "triangle-strip";

			case Layout::TRIANGLE_FAN:
				return "triangle-fan";

			default:
				return "unspecified/unknown";
		}
	}
}