
#include <UnitTest/UnitTest.h>
#include <array>

#include <TaggedFormat/Skeleton.h>
#include <TaggedFormat/Parser.h>
#include <TaggedFormat/MemoryBuffer.h>
#include <TaggedFormat/Reader.h>

namespace TaggedFormat {
	const char * BasicSkeletonText =
		"Model-animation: array skeleton-animation-key-frame\n"
		"	1 linear 0.0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1\n"
		"	1 linear 30.0 1 0 0 0 0 1 0 0 0 0 1 0 0 10 0 1\n"
		"end\n"
		"Model-skeleton: skeleton\n"
		"	bones: array skeleton-bone\n"
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

		{"Check Alignment",
			[](UnitTest::Examiner & examiner) {
				examiner.check_equal(member_offset(&SkeletonBone::name), 0);
				examiner.check_equal(member_offset(&SkeletonBone::parent), 32);
				examiner.check_equal(member_offset(&SkeletonBone::transform), 36);
			}
		},

		{"Test Parser",
			[](UnitTest::Examiner & examiner) {
				std::stringstream input(BasicSkeletonText);
				MemoryBuffer memory_buffer;

				Parser::serialize(input, memory_buffer);

				Reader reader(memory_buffer.buffer());

				auto skeleton = reader.block_at_offset<Skeleton>(reader.header()->top_offset);
				examiner.check(skeleton);

				auto bones = reader.array_at_offset<SkeletonBone>(skeleton->bones_offset);
				examiner.check(bones);

				examiner.check_equal(bones->count(), 2);
				examiner.check_equal(bones->at(0).parent, 0);
				examiner.check_equal(bones->at(1).parent, 0);

				examiner.check_equal(bones->at(0).name, "BoneA");
				examiner.check_equal(bones->at(1).name, "BoneB");

				for (std::size_t i = 0; i < 16; i += 1)
					examiner.check_equal(IdentityMatrix[i], bones->at(0).transform[i]);

				auto sequences = reader.block_at_offset<OffsetTable>(skeleton->sequences_offset);
				examiner.check(sequences);

				OffsetT default_sequence = sequences->offset_named("default");
				auto animation = reader.block_at_offset<SkeletonAnimation>(default_sequence);
				examiner.check(animation);

				examiner.check_equal(animation->start_time, 15.0);
				examiner.check_equal(animation->end_time, 30.0);

				OffsetT bone_key_frames_offset = animation->key_frames_offset;
				auto bone_key_frames = reader.array_at_offset<SkeletonAnimationKeyFrame>(bone_key_frames_offset);
				examiner.check(bone_key_frames);

				examiner.check_equal(bone_key_frames->count(), 2);
			}
		},
	};
}
