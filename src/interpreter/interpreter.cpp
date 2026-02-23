// src/interpreter/interpreter.cpp
#include "interpreter.hpp"
#include "../headers/SemanticAnalyser.hpp"
#include "../headers/parser.hpp"
#include "../headers/tokeniser.hpp"

Value Interpreter::evaluateExpr( const Expr* expr )
{
	if ( const auto num = dynamic_cast<const NumberExpr*>( expr ) ) {
		return std::stoi( num->value );
	}
	if ( const auto str = dynamic_cast<const StringExpr*>( expr ) ) {
		return str->value;
	}
	if ( const auto ident = dynamic_cast<const IdentExpr*>( expr ) ) {
		return env.variables.at( ident->name );
	}
	if ( const auto bin = dynamic_cast<const BinaryExpr*>( expr ) ) {
		const Value left = evaluateExpr( bin->left.get() );
		const Value right = evaluateExpr( bin->right.get() );
		if ( bin->operatr == TokenType::T_plus ) {
			return std::get<int>( left ) + std::get<int>( right );
		}
		if ( bin->operatr == TokenType::T_minus ) {
			return std::get<int>( left ) - std::get<int>( right );
		}
	}
	throw std::runtime_error( "Unknown expression type" );
}

/* --------------------------------------------------------------------------------------------- */

void Interpreter::executeStmt( const Stmt* stmt )
{
	if ( const auto var = dynamic_cast<const VarDeclStmt*>( stmt ) ) {
		const Value val = evaluateExpr( var->expr.get() );
		env.variables[ var->name ] = val;
	}
	if ( const auto assign = dynamic_cast<const AssignStmt*>( stmt ) ) {
		const Value val = evaluateExpr( assign->value.get() );
		env.variables[ assign->name ] = val;
	}
	if ( const auto ifs = dynamic_cast<const IfStmt*>( stmt ) ) {
		const Value cond = evaluateExpr( ifs->condition.get() );
		if ( std::get<int>( cond ) != 0 ) {
			executeStmt( ifs->thenBranch.get() );
		} else if ( ifs->elseBranch ) {
			executeStmt( ifs->elseBranch.get() );
		}
	}
	if ( const auto w = dynamic_cast<const WhileStmt*>( stmt ) ) {
		while ( std::get<int>( evaluateExpr( w->condition.get() ) ) != 0 ) {
			executeStmt( w->loopBody.get() );
		}
	}
}

/* --------------------------------------------------------------------------------------------- */
void Interpreter::execute( const std::vector<std::unique_ptr<Stmt>>& statements )
{
	for ( const auto& stmt : statements ) {
		executeStmt( stmt.get() );
	}
}
