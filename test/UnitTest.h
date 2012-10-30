//
//  Core/CodeTest.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/06.
//  Copyright 2006 Samuel WilliamsWilliams. All rights reserved.
//
//

#ifndef _UNIT_TEST_H
#define _UNIT_TEST_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

namespace UnitTest {
	/// Keep track of statistics.
	class Statistics {
		int _failed;
		int _passed;

	public:
		Statistics ();
		void fail_test ();
		void pass_test ();
		void print_summary (std::string name, std::ostream & out) const;

		bool failed() const { return _failed > 0; }

		void operator+= (const Statistics & other);
	};

	/// Logs errors that occur when performing unit tests, if they fail.
	class ErrorLogger {
		bool _error;

	public:
		ErrorLogger (bool error);
		~ErrorLogger ();

		template <typename AnyT>
		ErrorLogger& operator<< (const AnyT & fragment)
		{
			if (_error)
				std::cerr << fragment;

			return *this;
		}
	};
	
	/// A examiner runs the tests and keeps track of the results.
	class Examiner {
		Statistics * _statistics;
		
	public:
		Examiner(Statistics * statistics);
		
		ErrorLogger check(bool condition);
		
		template <typename LeftT, typename RightT>
		void check_equal(LeftT left, RightT right) {
			check(left == right) << left << " == " << right;
		}
	};
	
	// The test function type:
	typedef std::function<void (Examiner *)> TestFunctionT;

	/// A test that must be completed successfully.
	class Test {
		std::string _name;
		TestFunctionT _function;
		
	public:
		Test(std::string name, TestFunctionT function);
		
		std::string name() const { return _name; }
		
		void invoke(Examiner * examiner);
	};
	
	std::ostream & operator<< (std::ostream & out, const std::type_info & rhs);
	
	class Suite {
	public:
		struct Entry {
			const char * name;
			TestFunctionT test;
			
			Entry(const char * name_) : name(name_) {
			}
			
			Entry(const char * name_, TestFunctionT test_) : name(name_), test(test_) {
			}
		};
	
	protected:
		std::string _name;
		std::vector<Test *> _tests;

	public:
		Suite(std::initializer_list<Entry> entries);

		Statistics run(std::ostream & out);
	};
	
	/// A class that collects all defined unit tests.
	class Registry {
		std::vector<Suite *> _suites;
		
	public:
		void add(Suite * suite);
		Statistics perform_tests();
	};
	
	/// The global registry object.
	Registry * shared_registry();
}

#endif
