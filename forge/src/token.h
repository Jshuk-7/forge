#pragma once

#include <string>
#include <cstdint>

#include "assert.h"

namespace forge {

	enum class token_type
	{
		Ident,

		String,
		Number,
		Float,

		Proc,
		Return,

		Add,
		Sub,
		Mul,
		Div,

		Semicolon,
		Period,
		Comma,

		LBrace,
		RBrace,

		LParen,
		RParen,

		Eof,
	};

	struct position
	{
		uint32_t cursor;
		uint32_t line;
	};

	struct token
	{
		std::string lexeme;
		position pos;
		token_type type;
	};

	namespace utils {

		static std::string token_type_to_string(token_type type) {
			switch (type)
			{
				case token_type::Ident:     return "Ident";
				case token_type::String:    return "String";
				case token_type::Number:    return "Number";
				case token_type::Float:     return "Float";
				case token_type::Proc:      return "Proc";
				case token_type::Return:    return "Return";
				case token_type::Add:       return "Add";
				case token_type::Sub:       return "Sub";
				case token_type::Mul:       return "Mul";
				case token_type::Div:       return "Div";
				case token_type::Semicolon: return "Semicolon";
				case token_type::Period:    return "Period";
				case token_type::Comma:     return "Comma";
				case token_type::LBrace:    return "LBrace";
				case token_type::RBrace:    return "RBrace";
				case token_type::LParen:    return "LParen";
				case token_type::RParen:    return "RParen";
				case token_type::Eof:       return "Eof";
			}

			FG_ASSERT(false, "Unknown token type!");
			return "None";
		}

		static void print_token(token token_, const std::string& filename) {
			std::string lexeme = token_.lexeme;
			position pos = token_.pos;
			std::string type = token_type_to_string(token_.type);
			printf("['%s':'%s'; <%s:%u:%u>]\n",
				lexeme.c_str(),
				type.c_str(),
				filename.c_str(),
				pos.line,
				pos.cursor
			);
		}

	}

}
