//
//  Buffer.h
//  This file is part of the "TaggedModelFormat" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_MODEL_FORMAT_BUFFER_H
#define _TAGGED_MODEL_FORMAT_BUFFER_H

#include <stdint.h>

namespace TaggedModelFormat {
	
	typedef uint64_t OffsetT;
	typedef unsigned char ByteT;
	
	struct Buffer {
		const void * begin;
		OffsetT size;
		
		void * at(OffsetT offset) {
			return (ByteT *)begin + offset;
		}
	};
	
}

#endif
