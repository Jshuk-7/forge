#pragma once

#include <string>
#include <deque>
#include <cstdint>
#include <unordered_map>

#include "token.h"

namespace forge {

	enum class lexer_error
	{
		None = 0,
		IllegalSymbol,
	};

	struct lexer_output
	{
		lexer_error error;
		std::string error_desc;
		position error_pos;
		uint32_t token_count;
	};

	class lexer
	{
	public:
		lexer();
		~lexer();

		void set_input(const std::string& input);
		lexer_output process_input();

		token next_token();

	private:
		void init();

		void set_error(lexer_output* result, lexer_error error, const std::string& error_desc, position error_pos);

	private:
		uint32_t cursor() const;
		uint32_t line() const;
		char current() const;
		void trim();
		char advance();
		void next_line();

		void ident();
		void number();
		void string();

		bool is_at_end() const;
		bool is_space(char c) const;
		bool is_character(char c) const;
		bool is_number(char c) const;

		token make_token(uint32_t start, token_type type);

		void populate_keywords();

	private:
		std::string m_input;
		position m_position;
		std::deque<token> m_tokens;

	private:
		inline static std::unordered_map<std::string, token_type> s_keywords;
	};

}
