
#include <UnitTest/UnitTest.h>
#include <array>

#include <TaggedFormat/Skeleton.h>
#include <TaggedFormat/Parser.h>
#include <TaggedFormat/MemoryBuffer.h>
#include <TaggedFormat/Reader.h>

namespace TaggedFormat {
	const char * BasicSkeletonText =
		"Model-animation: array bone-key-frame\n"
		"	1 linear 0.0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n"
		"	1 linear 30.0 1 0 0 0 0 1 0 0 0 0 1 0 0 10 0 1\n"
		"end\n"
		"Model-skeleton: skeleton\n"
		"	weights: array b2w2\n"
		"		0 1 0.25 0.75\n"
		"	end\n"
		"	bones: array bone\n"
		"		BoneA 0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n"
		"		BoneB 0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 10 1\n"
		"	end\n"
		"	sequences: offset-table\n"
		"		default: animation 15.0 30.0\n"
		"			key-frames: $Model-animation\n"
		"		end\n"
		"	end\n"
		"end\n"
		"top: $Model-skeleton\n";

	std::array<float32, 16> IdentityMatrix = {
		{1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1}
	};

	UnitTest::Suite SkeletonTestSuite {
		"Test Skeleton Data Structures",
		
		{"Test Parser",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(BasicSkeletonText);
				MemoryBuffer memory_buffer;

				Parser::serialize(input, memory_buffer);

				Reader reader(memory_buffer.buffer());

				Skeleton * skeleton = reader.block_at_offset<Skeleton>(reader.header()->top_offset);
				examiner.check(skeleton);

				Weights<2> * weights = reader.block_at_offset<Weights<2>>(skeleton->weights_offset);
				examiner.check(weights);

				examiner.check_equal(element_count(weights), 1);
				examiner.check_equal(weights->vertices[0].bones[0], 0);
				examiner.check_equal(weights->vertices[0].bones[1], 1);
				examiner.check_equal(weights->vertices[0].weights[0], 0.25);
				examiner.check_equal(weights->vertices[0].weights[1], 0.75);

				Bones * bones = reader.block_at_offset<Bones>(skeleton->bones_offset);
				examiner.check(bones);

				examiner.check_equal(element_count(bones), 2);
				examiner.check_equal(bones->bones[0].parent, 0);
				examiner.check_equal(bones->bones[1].parent, 0);

				examiner.check_equal(bones->bones[0].name, "BoneA");
				examiner.check_equal(bones->bones[1].name, "BoneB");

				for (std::size_t i = 0; i < 16; i += 1)
					examiner.check_equal(IdentityMatrix[i], bones->bones[0].transform[i]);

				OffsetTable * sequences = reader.block_at_offset<OffsetTable>(skeleton->sequences_offset);
				examiner.check(sequences);

				OffsetT default_sequence = sequences->element_named("default")->offset;
				Animation * animation = reader.block_at_offset<Animation>(default_sequence);
				examiner.check(animation);

				examiner.check_equal(animation->start_time, 15.0);
				examiner.check_equal(animation->end_time, 30.0);

				OffsetT bone_key_frames_offset = animation->key_frames_offset;
				BoneKeyFrames * bone_key_frames = reader.block_at_offset<BoneKeyFrames>(bone_key_frames_offset);
				examiner.check(bone_key_frames);

				examiner.check_equal(element_count(bone_key_frames), 2);
			}
		},
	};
}
