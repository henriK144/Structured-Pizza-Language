#pragma once
#include <string>
#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>
#include "session.hpp"
#include "statement.hpp"
#include "parsetypes.hpp"

// Defines the ProgramState struct that statements are executed into

enum class State { READ, INTERPRET, EXECUTE };

struct ProgramState
{
	State state;
	std::optional<OrderSession> session;

	RawText sourcecode;
	Program bytecode;

	int programcounter;
	bool running;

	std::string PROMPT = "> ";
	bool norepl = false;

	ProgramState() : state{State::READ}, programcounter{0}, running{true} 
	{ ; }

	void readSource(std::istream& source)
	{
		std::ostringstream source_buffer; // set up an ostringstream
		for (char ch = '\0'; source.get(ch);) { // extract characters from source into said ostringstream
			if (ch == '\f') break; 
			if (ch == '\n') std::cout << PROMPT;
			source_buffer.put(ch);
		}
		source.ignore(); // get rid of the superfluous newline
		sourcecode = source_buffer.str(); // assign the result as the program's source code
	}

	void readSource(const char* sourcefile)
	{
		std::ifstream source_stream(sourcefile); // set up an ifstream and plug it into sourcefile
		if (!source_stream) { // quit if file doesn't exist
			std::cout << "File \"" << sourcefile << "\" does not exist.\n"; 
			return; 
		} 
		std::ostringstream source_buffer;
		for (char ch = '\0'; source_stream.get(ch);) {
			source_buffer.put(ch);
		}
		sourcecode = source_buffer.str(); // assign the result as the program's source code
	}
	
	void clearData() // frees the memory that represents the currently loaded program
	{
		bytecode.clear();
		sourcecode.clear();
	}
};
