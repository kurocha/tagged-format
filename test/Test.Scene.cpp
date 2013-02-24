
#include <UnitTest/UnitTest.h>
#include <array>

#include <TaggedFormat/Scene.h>
#include <TaggedFormat/Parser.h>
#include <TaggedFormat/MemoryBuffer.h>
#include <TaggedFormat/Reader.h>

namespace TaggedFormat {
	const char * BasicSceneText =
		"Test-mesh: mesh triangles\n"
		"end\n"
		"Test-skeleton: skeleton\n"
		"end\n"
		"Scene: node\n"
		"	root 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n"
		"	geometry-instance\n"
		"		mesh: $Test-mesh\n"
		"		skeleton: $Test-skeleton\n"
		"	end\n"
		"end\n"
		"top: $Scene\n";

	UnitTest::Suite SceneTestSuite {
		"Test Skeleton Data Structures",
		
		{"Test Parser",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(BasicSceneText);
				MemoryBuffer memory_buffer;

				Parser::serialize(input, memory_buffer);

				Reader reader(memory_buffer.buffer());

				Node * node = reader.block_at_offset<Node>(reader.header()->top_offset);
				examiner.check(node);
				examiner.check_equal(node->name, "root");

				examiner.check_equal(node->count(), 1);
				OffsetT instance_offset = node->at(0);

				GeometryInstance * geometry_instance = reader.block_at_offset<GeometryInstance>(instance_offset);
				examiner.check_equal(24, geometry_instance->mesh_offset);
				examiner.check_equal(72, geometry_instance->skeleton_offset);
				examiner.check_equal(0, geometry_instance->material_offset);
			}
		},
	};
}
