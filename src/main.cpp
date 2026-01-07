// Carp lang main

#include <fstream>
#include <iostream>
#include <print>
#include <sstream>

#include "headers/tokeniser.hpp"

int main( int argc, char* argv[] )
{
	if ( !( argc > 1 ) ) {
		std::cout << "Please provide an input file" << '\n';
		return -1;
	}

	std::ifstream fileIn( argv[ 1 ] );
	if ( !fileIn ) {
		std::cerr << "Failed to open file.\n";
		return -1;
	}

	std::stringstream buffer;
	buffer << fileIn.rdbuf();	// read file add put into buffer stream

	Tokeniser tokeniser( buffer.str() );  // conv to str and give to tokeniser
	auto tokens = tokeniser.tokenise();	  // get the returned tokens from the tokeniser

	// example output for debugging
	for ( const auto& t : tokens ) {
		std::cout << "TokenType order : " << static_cast<int>( t.type ) << " | Textual: '" << t.value
					 << "' " << "Pos: " << t.loc.line << ":" << t.loc.column << '\n';
	}

	return 0;
}
