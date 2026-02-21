// Carp lang main

#include <fstream>
#include <iostream>
#include <print>
#include <sstream>

#include "headers/SemanticAnalyser.hpp"
#include "headers/parser.hpp"
#include "headers/tokeniser.hpp"

int main( int argc, char* argv[] )
{
	if ( argc <= 1 ) {
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

	//@ Tokeniser
	Tokeniser tokeniser( buffer.str() );  // conv to str and give to tokeniser
	auto tokens = tokeniser.tokenise();	  // get the returned tokens from the tokeniser

	// # Token output for debugging
	for ( const auto& t : tokens ) {
		std::cout << "TokenType order : " << static_cast<int>( t.type ) << " | Textual: '" << MAGENTA
					 << t.value << CoRESET << "' " << "Pos: " << GREEN << t.loc.line << ":"
					 << t.loc.column << CoRESET << '\n';
	}

	// @ Parser
	std::vector<std::unique_ptr<Stmt>> nodes;
	try {

		Parser parser( tokens );  // pass tokens to the parser
		nodes = parser.parse();	  // start parsing and store in nodes

		for ( const auto& stmt : nodes ) {
			stmt->print();
		}

	} catch ( const std::exception& err ) {

		std::cerr << RED << "Parse Error: " << err.what() << CoRESET << "\n";
	}

	// @ Semantic analyser
	try {

		SemanticAnalyser semAnalyser;
		semAnalyser.analyse( nodes );

	} catch ( const std::exception& err ) {

		std::cerr << RED << "Semantic Error: " << err.what() << CoRESET << "\n";
	}

	return 0;
}
