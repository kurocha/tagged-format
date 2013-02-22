//
//  External.h
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef __TAGGED_FORMAT_EXTERNAL_H
#define __TAGGED_FORMAT_EXTERNAL_H

#include "Block.h"

namespace TaggedFormat {
	/// Reference an external resource. Implementation defined.
	struct External : public Block {
		static const TagT TAG = tag_from_identifier("EXRN");

		typedef unsigned char ElementT;

		/// Null terminated URL referencing external resource.
		unsigned char url[0];
	};
}

#endif
