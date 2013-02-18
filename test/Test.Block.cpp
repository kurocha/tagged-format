
#include <UnitTest/UnitTest.h>
#include <TaggedFormat/Block.h>

namespace TaggedFormat {
	UnitTest::Suite BlocksTestSuite {
		"Test Blocks",
		
		{"Data type sizes and alignments",
			[](UnitTest::Examiner & examiner) {
				examiner.check_equal(sizeof(TaggedFormat::float32), 4);
				examiner.check_equal(sizeof(TaggedFormat::float64), 8);
			}
		},
		
		{"Block header initialisation",
			[](UnitTest::Examiner & examiner) {
				TaggedFormat::Header header;
				TaggedFormat::clear(header);
				
				examiner.check_equal(sizeof(TaggedFormat::Header), header.size);
			}
		}
	};
}
