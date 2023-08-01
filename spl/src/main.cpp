// An implementation of the Structured Pizza Language, or SPL for short.

#include <iostream>
#include <limits>
#include <climits>
#include <string>
#include <vector>
#include <algorithm>

#include "pizza.hpp"
#include "order.hpp"
#include "session.hpp"

#include "parser.hpp"
#include "printer.hpp"
#include "program.hpp"
#include "statement.hpp"
#include "syntax.hpp"
#include "tokens.hpp"

//#define TESTING

int main(int argc, char** argv)
{

	ProgramState progstate;

	#ifdef TESTING

	// This is where you can do tests and stuff 

	#else

	std::cout << "Welcome to the Structured Pizza Language Interpeter v1.0!" << '\n';
	if (argc > 1) { // Process command line arguments

		std::vector<std::string> cmdArgs(&argv[1], &argv[1] + argc - 1);

		if (std::any_of(cmdArgs.begin(), cmdArgs.end(), [](const std::string& s) { return s.empty(); })) {
			std::cout << "Fatal error: Empty command line argument" << '\n';
			goto fatal_err;
		}

		auto scriptIt = std::stable_partition(
			cmdArgs.begin(), 
			cmdArgs.end(), 
			[](const std::string& s) { return s.front() == '-'; }
		);

		for (auto it = cmdArgs.begin(); it != scriptIt; ++it) { // process flags
			if (*it == "-detail") {
				printer::BDETAIL_FLAG = true;
			} else if (*it == "-slashlines") {
				printer::LINE_CHAR = '/';
			} else if (*it == "-tildelines") {
				printer::LINE_CHAR = '~';
			} else if (*it == "-doublearrow") {
				progstate.PROMPT = ">> ";
				printer::PROMPT = ">> ";
			} else if (*it == "-norepl") {
				progstate.norepl = true;
			} else {
				std::cout << "Fatal error: Unrecognized command line argument \"" << *it << "\"\n";
				goto fatal_err;
			}
		}

		for (auto it = scriptIt; it != cmdArgs.end(); ++it) { // process input scripts
			if (!progstate.running) break;

			progstate.readSource(it->c_str());

			try {
				progstate.bytecode = std::move(parser::interpret(progstate.sourcecode));
			} catch (BadInterp& bi) {
				printer::reportInterpError(bi, progstate);
				progstate.clearData();
				continue;
			}

			int retcode = execute(progstate.bytecode, progstate);
			if (retcode) {
				printer::reportError();
			}

			progstate.clearData();
		}

	}

	if (progstate.norepl) goto terminus;

	while (progstate.running) {
		switch (progstate.state) {

			case State::READ:

				if (std::cin.eof()) {
					progstate.running = false;
					continue;
				}

				printer::REPLineBreak(); // prompt for program input on stdin, then interpret
				progstate.readSource(std::cin);			
				progstate.state = State::INTERPRET;

				break;

			case State::INTERPRET:

				try { // interpret and load bytecode, then execute if valid or read again if invalid
					progstate.bytecode = std::move(parser::interpret(progstate.sourcecode));
					progstate.state = State::EXECUTE;
				} catch (BadInterp& bi) {
					printer::reportInterpError(bi, progstate);
					progstate.clearData();
					progstate.state = State::READ;
				}

				break;

			case State::EXECUTE:

				int retcode = execute(progstate.bytecode, progstate);
				if (retcode) {
					printer::reportError();
				}
				progstate.clearData();
				progstate.state = State::READ;

				break;

		}
	}
	#endif
	terminus:	
	return 0;
	fatal_err:
	return 1;
}
