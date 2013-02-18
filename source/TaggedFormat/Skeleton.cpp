//
//  Skeleton.cpp
//  TaggedFormat
//
//  Created by Samuel Williams on 18/02/13.
//  Copyright (c) 2013 Orion Transfer Ltd. All rights reserved.
//

#include "Skeleton.h"

namespace TaggedFormat {
	std::string BoneKeyFrames::name_for_interpolation(Interpolation interpolation)
	{
		switch (interpolation) {
		case Interpolation::LINEAR:
			return "linear";
		case Interpolation::BEZIER:
			return "bezier";
		default:
			return "unknown";
		}
	}
}
