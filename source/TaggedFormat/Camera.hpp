//
//  Camera.hpp
//  File file is part of the "Unit Test" project and released under the MIT License.
//
//  Created by Samuel Williams on 13/4/2016.
//  Copyright, 2016, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Block.hpp"

namespace TaggedFormat
{
	struct Camera : Block {
		static const TagT TAG = tag_from_identifier("CAM5");
		
		/// Native viewport size.
		float32 resolution[2], pixel_aspect_ratio[2];
		
		float32 view_matrix[16];
		float32 projection_matrix[16];
	};
}
