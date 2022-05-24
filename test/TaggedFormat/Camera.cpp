//
//  Camera.cpp
//  File file is part of the "Unit Test" project and released under the MIT License.
//
//  Created by Samuel Williams on 13/4/2016.
//  Copyright, 2016, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>
#include <TaggedFormat/Camera.hpp>

#include <TaggedFormat/Parser.hpp>
#include <TaggedFormat/Reader.hpp>

#include <Buffers/DynamicBuffer.hpp>

#include <vector>

namespace TaggedFormat
{
	const char * CameraTestText =
		"Test-camera: camera\n"
		"	1920 1080 1 1\n"
		"	1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\n"
		"	1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\n"
		"top: $Test-camera\n";

	UnitTest::Suite CameraTestSuite {
		"TaggedFormat::Camera",
		
		{"it should parse view and projection matrices",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(CameraTestText);
				Buffers::DynamicBuffer buffer;

				Parser::serialize(input, buffer);
				
				examiner << "Data size is correct." << std::endl;
				examiner.expect(buffer.size()) == 180;
				
				Reader reader(buffer);
				
				auto camera = reader.block_at_offset<Camera>(reader.header()->top_offset);

				examiner.expect(camera->resolution[0]) == 1920;
				examiner.expect(camera->resolution[1]) == 1080;
				examiner.expect(camera->pixel_aspect_ratio[0]) == 1;
				examiner.expect(camera->pixel_aspect_ratio[1]) == 1;

				float32 test_pattern[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
				examiner.check(std::equal(camera->view_matrix, camera->view_matrix+16, test_pattern));
			}
		},
	};
}
