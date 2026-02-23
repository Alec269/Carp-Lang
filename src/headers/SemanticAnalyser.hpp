// src\headers\SemanticAnalyser.hpp
#pragma once

#include <unordered_map>

#include "parser.hpp"

// to store info about a var
struct Symbol {
	TokenType tType;	// currently we only have its type
};

// everything within a {} - multiple can exist in one file
struct Scope {
	std::unordered_map<std::string, Symbol> symbols;

	/* This map says:
	"x"   → Symbol{ int }
	"msg" → Symbol{ string }
	*/
};

class SemanticAnalyser {
 public:
	SemanticAnalyser();
	void analyse( const std::vector<std::unique_ptr<Stmt>>& program );

 private:
	std::vector<Scope> scopeStack;  // to keep track of all the scopes in order

	/* example stack
	global scope
		└─ if scope
			└─ while scope
	*/

	void enterScope();
	void exitScope();

	void visitStmt( const Stmt* stmt );
	TokenType visitExpr( const Expr* expr );

	void declare( const std::string& name, TokenType type );
	Symbol* lookup( const std::string& name );
};
