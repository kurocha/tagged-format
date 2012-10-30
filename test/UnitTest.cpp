//
//  UnitTests.cpp
//  This file is released under the MIT license.
//
//  Created by Samuel Williams on 25/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//

#include "UnitTest.h"
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

namespace UnitTest {
	const char * VERSION = "1.0.0";
	
	namespace Options {
		std::string failure_command;
	}
	
	Statistics::Statistics () : _failed (0), _passed (0)
	{
	}

	void Statistics::fail_test ()
	{
		_failed += 1;
	}

	void Statistics::pass_test ()
	{
		_passed += 1;
	}

	void Statistics::operator+= (const Statistics & other)
	{
		_passed += other._passed;
		_failed += other._failed;
	}

	void Statistics::print_summary (std::string name, std::ostream & out) const
	{
		out << "[" << name << "] " << _passed << " passed";
		if (_failed > 0)
			out << " " << _failed << " failed";

		out << " out of " << _failed + _passed << " total" << std::endl;
	}
	
	ErrorLogger::ErrorLogger (bool error) : _error(error)
	{
	}

	ErrorLogger::~ErrorLogger ()
	{
		if (_error)
			std::cerr << std::endl;
	}
	
	Examiner::Examiner(Statistics * statistics) : _statistics(statistics) {
		
	}
	
	/// This function is typically used for parsing OpenGL extension strings.
	template <typename OutT>
	void split(const std::string & input, const char divider, OutT result) {
		std::size_t pos = 0, next = 0;

		do {
			next = input.find(divider, pos);

			std::string bit(&input[pos], (next == std::string::npos) ? (input.size() - pos) : (next - pos));
			*result++ = bit;

			pos = next + 1;
		} while (next != std::string::npos);
	}
	
	void run(const std::string & command) {
		pid_t child = fork();

		if (child) {
			waitpid(child, NULL, 0);
		} else {
			std::vector<std::string> arguments;
			split(command, ' ', std::back_inserter(arguments));

			std::vector<const char *> argv;
			for (auto & argument : arguments)
				argv.push_back(argument.c_str());

			argv.push_back(nullptr);

			dup2(2, 1);
			execv(argv[0], (char *const *)argv.data());
			abort();
		}
	}
	
	ErrorLogger Examiner::check(bool condition) {
		if (condition == false) {
			_statistics->fail_test();

			if (Options::failure_command.size() > 0) {
				run(Options::failure_command);
			}

			std::cerr << "Test Failed: ";
			return ErrorLogger(true);
		} else {
			_statistics->pass_test();
			return ErrorLogger(false);
		}
	}

	Test::Test(std::string name, TestFunctionT function) : _name(name), _function(function) {
	}

	void Test::invoke(Examiner * examiner) {
		_function(examiner);
	}
	
	Suite::Suite(std::initializer_list<Entry> entries) {
		for (auto & entry : entries) {
			if (entry.test)
				_tests.push_back(new Test(entry.name, entry.test));
			else
				_name = entry.name;
		}
		
		shared_registry()->add(this);
	}
	
	Statistics Suite::run(std::ostream & out) {
		Statistics total;
		
		out << "--- " << _name << " ---" << std::endl;
		
		for (auto test : _tests) {
			Statistics results;
			Examiner examiner(&results);
			
			test->invoke(&examiner);
			
			results.print_summary(test->name(), out);
			
			total += results;
		}
		
		return total;
	}
	
	void Registry::add(Suite * suite) {
		_suites.push_back(suite);
	}
	
	Statistics Registry::perform_tests() {
		Statistics overall;
		
		for (auto suite : _suites) {
			std::cerr << std::endl;
			overall += suite->run(std::cerr);
		}
		
		std::cerr << std::endl;
		overall.print_summary("Summary", std::cerr);
		
		return overall;
	}
	
	Registry * shared_registry() {
		static Registry * registry = nullptr;
		
		if (registry == nullptr)
			registry = new Registry;
		
		return registry;
	}
	
	static void segmentation_fault(int) {
		std::cerr << "*** Segmentation Fault ***" << std::endl;
		run(Options::failure_command);
		exit(-1);
	}
}

int main (int argc, char** argv)
{
	for (int i = 0; i < argc; i += 1) {
		auto arg = std::string(argv[i]);
		
		if (arg == "--on-failure" && (i+1) < argc) {
			UnitTest::Options::failure_command = std::string(argv[i+1]);
		}
		
		if (arg == "--copy") {
			std::cerr << "Unit Test Runner v" << UnitTest::VERSION << ". Copyright, 2012, by Samuel G. D. Williams." << std::endl;
			std::cerr << "This software is released under the MIT license and comes with ABSOLUTELY NO WARRANTY." << std::endl;
		}
		
		if (arg == "--help") {
			std::cerr << "Usage: " << argv[0] << " [options]" << std::endl;
			std::cerr << "This script can be used to run a set of compiled unit tests." << std::endl;
			std::cerr << std::endl;
			std::cerr << "	--on-failure [script-path]" << std::endl;
			std::cerr << "		Execute the given script when a unit test fails via fork-exec." << std::endl;
		}
	}
	
	if (UnitTest::Options::failure_command.size() > 0) {
		signal(SIGSEGV, UnitTest::segmentation_fault);
	}
	
	UnitTest::Statistics statistics = UnitTest::shared_registry()->perform_tests();
	
	if (statistics.failed()) {
		return 1;
	} else {
		return 0;
	}
}
