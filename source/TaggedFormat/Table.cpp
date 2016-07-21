//
//  Table.cpp
//  Tagged Format
//
//  Created by Samuel Williams on 29/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Table.hpp"

namespace TaggedFormat {
	OffsetT OffsetTable::offset_named(const std::string & name) const {
		const NamedOffset * element = lookup(name);

		if (element)
			return element->offset;
		else
			return 0;
	}
}