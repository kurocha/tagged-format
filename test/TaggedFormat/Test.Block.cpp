
#include <UnitTest/UnitTest.hpp>

#include <TaggedFormat/Block.hpp>
#include <TaggedFormat/Mesh.hpp>

namespace TaggedFormat {
	UnitTest::Suite BlockTestSuite {
		"Test Block",

		{"it is empty",
			[](UnitTest::Examiner & examiner) {
				Array<VertexP3> array;
				clear(array);
				
				examiner.expect(array.count()) == 0;
			}
		},

		{"it has the correct size and alignment",
			[](UnitTest::Examiner & examiner) {
				examiner.expect(member_offset(&Block::tag)) == 0;
				examiner.expect(member_offset(&Block::size)) == 4;
				
				examiner.expect(sizeof(float32)) == 4;
				examiner.expect(sizeof(float64)) == 8;
				
				examiner.expect(sizeof(Block)) == 12;
			}
		},
		
		{"it can clear block header",
			[](UnitTest::Examiner & examiner) {
				Header header;
				clear(header);
				
				examiner.expect(header.size) == sizeof(Header);
			}
		}
	};
}
