
#include <UnitTest/UnitTest.hpp>
#include <array>

#include <TaggedFormat/Scene.hpp>
#include <TaggedFormat/Parser.hpp>
#include <TaggedFormat/Reader.hpp>

#include <Buffers/DynamicBuffer.hpp>

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
		"	node\n"
		"		child 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n"
		"	end\n"
		"end\n"
		"top: $Scene\n";

	UnitTest::Suite SceneTestSuite {
		"Test Skeleton Data Structures",
		
		{"Test Parser",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(BasicSceneText);
				Buffers::DynamicBuffer buffer;

				Parser::serialize(input, buffer);

				Reader reader(buffer);

				auto node = reader.block_at_offset<Node>(reader.header()->top_offset);
				examiner.check(node);
				examiner.check_equal(node->name, "root");

				examiner.check_equal(node->count(), 2);
				OffsetT instance_offset = node->at(0);

				auto geometry_instance = reader.block_at_offset<GeometryInstance>(instance_offset);
				examiner.check_equal(24, geometry_instance->mesh_offset);
				examiner.check_equal(72, geometry_instance->skeleton_offset);
				examiner.check_equal(0, geometry_instance->material_offset);

				OffsetT child_node_offset = node->at(1);

				auto child_node = reader.block_at_offset<Node>(child_node_offset);
				examiner.check(child_node);
				examiner.check_equal(child_node->name, "child");
			}
		},
	};
}
