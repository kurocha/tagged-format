//
//  Table.cpp
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Table.h"

namespace TaggedFormat {
	OffsetT OffsetTable::offset_named(const std::string & name) {
		NamedOffset * element = lookup(name);

		if (element)
			return element->offset;
		else
			return 0;
	}
}