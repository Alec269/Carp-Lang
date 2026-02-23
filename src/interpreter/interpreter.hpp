// src/interpreter/interpreter.hpp
#pragma once

#include <memory>
#include <vector>

#include "../headers/parser.hpp"

class Interpreter {
 public:
	void execute( const std::vector<std::unique_ptr<Stmt>>& statements );

 private:
	Environment env;

	void executeStmt( const Stmt* stmt );
	Value evaluateExpr( const Expr* expr );
};
