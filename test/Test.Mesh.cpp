
#include <UnitTest/UnitTest.h>
#include <TaggedFormat/Mesh.h>

namespace TaggedFormat {
	UnitTest::Suite MeshTestSuite {
		"Test Mesh",

		{"Check Mesh Data Structure Alignment",
			[](UnitTest::Examiner & examiner) {
				examiner.check_equal(sizeof(Index16), 2);
				examiner.check_equal(sizeof(Index32), 4);

				examiner.check_equal(member_offset(&VertexP3N3M2C4::position), 0);
				examiner.check_equal(member_offset(&VertexP3N3M2C4::normal), 12);
				examiner.check_equal(member_offset(&VertexP3N3M2C4::mapping), 24);
				examiner.check_equal(member_offset(&VertexP3N3M2C4::color), 32);
			}
		},
	};
}
