//
//  Buffer.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/03/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _TAGGED_FORMAT_BUFFER_H
#define _TAGGED_FORMAT_BUFFER_H

#include "Types.hpp"

namespace TaggedFormat
{
	/// A simple memory buffer with a given size.
	struct Buffer
	{
		const void * begin;
		OffsetT size;
		
		void * at(OffsetT offset) {
			return (Byte *)begin + offset;
		}
	};
}

#endif
