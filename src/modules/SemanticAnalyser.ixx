module;

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module SemanticAnalyser;

import tokeniser;
import parser;

// enum class TokenType;

// to store info about a var
export struct Symbol {
	TokenType tType;	// currently we only have its type
};

// everything within a {} - multiple can exist in one file

export struct Scope {
	std::unordered_map<std::string, Symbol> symbols;
	/* This map says:
	"x"   → Symbol{ int }
	"msg" → Symbol{ string }
	*/
};

export  class SemanticAnalyser {
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
//
	void enterScope();
	void exitScope();

	void visitStmt( const Stmt* stmt );
	TokenType visitExpr( const Expr* expr );

	void declare( const std::string& name, TokenType type );
	Symbol* lookup( const std::string& name );
};

SemanticAnalyser::SemanticAnalyser() = default;

void SemanticAnalyser::enterScope()
{
	scopeStack.push_back( {} );  // add new empty scope object in scope vector
}

void SemanticAnalyser::exitScope()
{
	scopeStack.pop_back();	// remove the last element [meaning exit]
}

// returns a pointer to the Symbol of an identifier, if that id exists in any active scope.
Symbol* SemanticAnalyser::lookup( const std::string& name )
{
	// Search for this variable starting from the closest scope, outward
	for ( auto rIter = scopeStack.rbegin(); rIter != scopeStack.rend(); ++rIter ) {
		// we're using reverse iteration to start from innermost scope to outermost scope
		// from child to parent, eventually to global scope
		auto found = rIter->symbols.find( name );
		// ↑ try to find id via name among symbols of that scope and return an iterator
		if ( found != rIter->symbols.end() ) {
			return &found->second;	// pointer to the Symbol stored in the map
		}
	}
	return nullptr;

	// rbegin() = Give me an iterator that starts at the LAST element
	// rend() = starts before first
}

// to keep track of declarations
void SemanticAnalyser::declare( const std::string& name, const TokenType type )
{
	auto& curent = scopeStack.back().symbols;	 // get the latest scope

	if ( curent.contains( name ) ) {
		throw std::runtime_error( "Variable redeclared: " + name );
	}
	curent[ name ] = Symbol{ type };	 // if not redeclared, mk key and assign it the type

	/*
		curent[name]
			If key exists → returns reference to value
			If key doesn’t exist → creates it
		Symbol{ type }
			Aggregate initialisation
			Equivalent to:
				Symbol sym;
				sym.tType = type;
	*/
}

// for getting expr
TokenType SemanticAnalyser::visitExpr( const Expr* expr )
{
	if ( dynamic_cast<const NumberExpr*>( expr ) ) {
		return TokenType::T_int;
	}
	/*	What dynamic_cast does
		Checks runtime type
			Returns:
				- pointer → if cast succeeds
				- nullptr → if it fails
		Why it works
			Because:
				Expr has a virtual destructor:
					- That gives it a vtable
					- RTTI is enabled
				This line does three things:
					- Tries to cast expr
					- Stores result in num
					- Tests num != nullptr
	*/
	if ( const auto id = dynamic_cast<const IdentExpr*>( expr ) ) {  // if it has id
		const Symbol* sym =
			 lookup( id->name );	 // get id [Pointer lets you express absence (nullptr)]
		if ( !sym ) {
			throw std::runtime_error( "Use of undeclared variable: " + id->name );
		}
		return sym->tType;
	}

	if ( dynamic_cast<const StringExpr*>( expr ) ) {
		return TokenType::T_string;
	}

	throw std::runtime_error( "Unknown expression type" );
}

void SemanticAnalyser::visitStmt( const Stmt* stmt )
{
	// declaration
	// here it tries to cast, if it passes then we know it is varDecl. if not then ship to next 'if'
	if ( const auto v = dynamic_cast<const VarDeclStmt*>( stmt ) ) {
		// ReSharper disable once CppTooWideScopeInitStatement
		const TokenType exprType = visitExpr( v->expr.get() );  // visitExpr returns a TokenType
		// get the expr type (like intLit/strLit etc) and compare
		if ( exprType != v->type ) {	// here v->type is the type decl like int,string,float
			throw std::runtime_error( "Type mismatch in declaration of " + v->name );
		}

		declare( v->name, v->type );
		return;
	}
	// assignment
	if ( const auto a = dynamic_cast<const AssignStmt*>( stmt ) ) {
		const Symbol* sym = lookup( a->name );	 // check if id exists or not
		if ( !sym ) {
			throw std::runtime_error( "Assignment to undeclared variable: " + a->name );
		}
		// ReSharper disable once CppTooWideScopeInitStatement
		const TokenType valueType = visitExpr( a->value.get() );	 // get the expr
		if ( valueType != sym->tType ) {
			throw std::runtime_error( "Type mismatch in assignment to " + a->name );
		}
		return;
	}
	// blocks
	if ( const auto b = dynamic_cast<const BlockStmt*>( stmt ) ) {
		enterScope();
		for ( auto& st : b->statements ) {
			visitStmt( st.get() );
			// What .get() does
			// Returns the raw pointer inside unique_ptr.
			// Ownership does not change.
		}
		exitScope();
		return;
	}
	// if
	if ( const auto i = dynamic_cast<const IfStmt*>( stmt ) ) {
		visitExpr( i->condition.get() );
		visitStmt( i->thenBranch.get() );
		if ( i->elseBranch ) {
			visitStmt( i->elseBranch.get() );
		}
		return;
	}
	// while
	if ( const auto w = dynamic_cast<const WhileStmt*>( stmt ) ) {
		visitExpr( w->condition.get() );
		visitStmt( w->loopBody.get() );
		return;
	}
	throw std::runtime_error( "Unknown Statement type" );
}

void SemanticAnalyser::analyse( const std::vector<std::unique_ptr<Stmt>>& program )
{
	enterScope();	// global scope
	for ( auto& stmt : program ) {
		visitStmt( stmt.get() );
	}
	exitScope();
}
