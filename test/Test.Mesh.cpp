
#include <UnitTest/UnitTest.h>

#include <TaggedFormat/Mesh.h>
#include <TaggedFormat/Parser.h>
#include <TaggedFormat/MemoryBuffer.h>
#include <TaggedFormat/Reader.h>

namespace TaggedFormat {
	const char * BasicMeshText =
		"Test-mesh: mesh triangles\n"
		"	indices: array index16\n"
		"		0 1 2 3 4 5 6 7 8 9 10 11"
		"	end\n"
		"	vertices: array vertex-p3n3m2\n"
		"		0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0\n"
		"		0.0 0.0 0.0 0.0 0.0 1.0 0.0 0.0\n"
		"		1.0 0.0 0.0 0.0 0.0 1.0 1.0 0.0\n"
		"		1.0 1.0 0.0 0.0 0.0 1.0 1.0 1.0\n"
		"		0.0 1.0 0.0 0.0 0.0 1.0 0.0 1.0\n"
		"	end\n"
		"	axes: array axis\n"
		"		Test 1.0 2.0 3.0 4.0 5.0 6.0 7.0"
		"	end\n"
		"end\n"
		"top: $Test-mesh\n";
		
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

		{"Test Parser",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(BasicMeshText);
				MemoryBuffer memory_buffer;

				Parser::serialize(input, memory_buffer);

				Reader reader(memory_buffer.buffer());

				auto mesh = reader.block_at_offset<Mesh>(reader.header()->top_offset);
				examiner.check(mesh);

				auto indices = reader.array_at_offset<Index16>(mesh->indices_offset);
				examiner.check(indices);

				examiner.check_equal(indices->count(), 12);

				for (std::size_t i = 0; i != indices->count(); i += 1) {
					examiner.check_equal(indices->at(i).value, i);
				}

				auto vertices = reader.array_at_offset<VertexP3N3M2>(mesh->vertices_offset);
				examiner.check(vertices);

				examiner.check_equal(vertices->count(), 5);

				examiner.check_equal(vertices->at(0).position[0], 0.0);
				examiner.check_equal(vertices->at(0).position[1], 1.0);
				examiner.check_equal(vertices->at(0).position[2], 2.0);
				examiner.check_equal(vertices->at(0).normal[0], 3.0);
				examiner.check_equal(vertices->at(0).normal[1], 4.0);
				examiner.check_equal(vertices->at(0).normal[2], 5.0);
				examiner.check_equal(vertices->at(0).mapping[0], 6.0);
				examiner.check_equal(vertices->at(0).mapping[1], 7.0);

				auto axes = reader.block_at_offset<Axes>(mesh->axes_offset);
				examiner.check(axes);
				examiner.check_equal(axes->count(), 1);

				examiner.check_equal(axes->at(0).translation[0], 1.0);
				examiner.check_equal(axes->at(0).translation[1], 2.0);
				examiner.check_equal(axes->at(0).translation[2], 3.0);

				examiner.check_equal(axes->at(0).rotation[0], 4.0);
				examiner.check_equal(axes->at(0).rotation[1], 5.0);
				examiner.check_equal(axes->at(0).rotation[2], 6.0);
				examiner.check_equal(axes->at(0).rotation[3], 7.0);
			}
		},
	};
}
