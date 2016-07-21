//
//  Types.h
//  This file is part of the "Tagged Format" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#pragma once

#include <cstddef>
#include <cstdint>

#include <string>

// std::strncpy, std::strncmp
#include <cstring>

namespace TaggedFormat
{
	typedef std::uint8_t Byte;
	
	/// A helper for aligned data, defaulting to 32-bit boundaries:
	template <typename _TypeT, std::size_t _Alignment = 4>
	struct Aligned
	{
		enum {Alignment = _Alignment};
		typedef __attribute__((__aligned__((Alignment)))) _TypeT TypeT;
	};

	typedef Aligned<uint32_t>::TypeT TagT;
	typedef Aligned<uint32_t>::TypeT MagicT;
	
	/// A standard 32-bit floating point data-type.
	typedef Aligned<float>::TypeT float32;
	
	/// A standard 64-bit floating point data-type.
	typedef Aligned<double>::TypeT float64;

	/// A memory offset to a specific block.
	typedef Aligned<std::size_t>::TypeT OffsetT;

	/// A fixed string typically used for holding names or references:
	template <std::size_t N = 32>
	struct FixedString {
		char value[N];

		void assign(const std::string & other) {
			std::strncpy(value, other.c_str(), N);
		}

		int compare(const std::string & other) const {
			if (other.size() > N)
				return -1;
			else
				return std::strncmp(other.c_str(), (const char *)value, N);
		}

		bool operator==(const std::string & other) const {
			return compare(other) == 0;
		}

		const std::string & operator=(const std::string & other) {
			assign(other);

			return other;
		}

		operator std::string () const {
			std::size_t length = 0;

			while (length < N && value[length]) {
				length++;
			}

			return std::string(value, value+length);
		}
	};

	template <std::size_t N>
	std::istream & operator>>(std::istream & input, FixedString<N> & fixed_string) {
		std::string other;
		input >> other;
		
		fixed_string.assign(other);

		return input;
	}

	template <std::size_t N>
	std::ostream & operator<<(std::ostream & output, const FixedString<N> & fixed_string) {
		output << (std::string)(fixed_string);

		return output;
	}

	/// This function returns the offset of a member of a class.
	///		e.g. member_offset(&VertexData::color)
	template <class T, typename U>
	inline constexpr std::size_t member_offset(U T::* member)
	{
		// https://gist.github.com/graphitemaster/494f21190bb2c63c5516
		// constexpr T t {};
		// return size_t(&(t.*member)) - size_t(&t);

		return reinterpret_cast<std::size_t>(&(reinterpret_cast<T const volatile*>(0)->*member));
	}
}
