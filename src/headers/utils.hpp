#pragma once

#include <string>

#include "tokeniser.hpp"

// define colour codes for easy printing
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string MAGENTA = "\033[35m";
const std::string CoRESET = "\033[0m";	 // Reset to default color

inline std::string tokenTypeToString( TokenType t )
{
	switch ( t ) {
	case TokenType::T_int:
		return "int";
	case TokenType::T_float:
		return "float";
	case TokenType::T_string:
		return "string";

	case TokenType::T_identifier:
		return "identifier";
	case TokenType::T_numLit:
		return "number";
	case TokenType::T_strLit:
		return "string literal";

	case TokenType::T_eq:
		return "=";
	case TokenType::T_eqEq:
		return "==";
	case TokenType::T_NotE:
		return "!=";
	case TokenType::T_LeT:
		return "<";
	case TokenType::T_LeTEq:
		return "<=";
	case TokenType::T_GrT:
		return ">";
	case TokenType::T_GrTEq:
		return ">=";

	case TokenType::T_plus:
		return "+";
	case TokenType::T_minus:
		return "-";
	case TokenType::T_star:
		return "*";
	case TokenType::T_slash:
		return "/";

	case TokenType::T_semi:
		return ";";
	case TokenType::T_comma:
		return ",";

	case TokenType::T_LBrack:
		return "(";
	case TokenType::T_RBrack:
		return ")";
	case TokenType::T_LBrace:
		return "{";
	case TokenType::T_RBrace:
		return "}";
	case TokenType::T_LSquare:
		return "[";
	case TokenType::T_RSquare:
		return "]";

	case TokenType::T_EOF:
		return "<EOF>";

	default:
		return "<unknown token>";
	}
}

// prints space character until correct indentation is met
inline void indent( int n )
{
	for ( int i = 0; i < n; ++i ) {
		std::cout << " ";
	}
}
