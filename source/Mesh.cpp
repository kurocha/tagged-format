//
//  Mesh.cpp
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Mesh.h"

namespace TaggedFormat {
	std::string Mesh::name_for_layout(Layout layout) {
		switch (layout) {
			case POINTS:
				return "points";

			case LINES:
				return "lines";

			case LINE_LOOP:
				return "line-loop";

			case LINE_STRIP:
				return "line-strip";

			case TRIANGLES:
				return "triangles";

			case TRIANGLE_STRIP:
				return "triangle-strip";

			case TRIANGLE_FAN:
				return "triangle-fan";

			default:
				return "unspecified/unknown";
		}
	}
}