//
//  Buffer.h
//  This file is part of the "Tagged Model Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_BUFFER_H
#define _TAGGED_MODEL_FORMAT_BUFFER_H

#include "Alignment.h"

namespace TaggedModelFormat {

	/// A memory offset to a specific block.
	typedef Aligned<uint64_t>::TypeT OffsetT;

	/// An 8-bit byte data-type.
	typedef unsigned char ByteT;

	/// A simple memory buffer with a given size.
	struct Buffer {
		const void * begin;
		OffsetT size;
		
		void * at(OffsetT offset) {
			return (ByteT *)begin + offset;
		}
	};
	
}

#endif
