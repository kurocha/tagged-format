//
//  Alignment.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_FORMAT_ALIGNMENT_H
#define _TAGGED_FORMAT_ALIGNMENT_H

#include <cstddef>
#include <cstdint>

namespace TaggedFormat {

	/// A helper for aligned data, defaulting to 32-bit boundaries:
	template <typename _TypeT, std::size_t _Alignment = 4>
	struct Aligned {
		enum {Alignment = _Alignment};
		typedef __attribute__((__aligned__((Alignment)))) _TypeT TypeT;
	};

}

#endif
