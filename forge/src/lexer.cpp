#include "lexer.h"

#include <format>
#include <cctype>

namespace forge {
	
	lexer::lexer()
	{
		init();
	}
	
	lexer::~lexer() { }

	void lexer::init()
	{
		populate_keywords();
		m_position.cursor = 0;
		m_position.line = 1;
		m_line_start = 0;
	}

	void lexer::set_error(lexer_output* out, lexer_error error, const std::string& error_desc, position error_pos)
	{
		out->error = error;
		out->error_desc = error_desc;
		out->error_pos = error_pos;
	}

	void lexer::set_input(const std::string& input)
	{
		m_input = input;
	}

	lexer_output lexer::process_input()
	{
		lexer_output result;
		result.error = lexer_error::None;
		result.error_desc = "";
		result.error_pos.cursor = 0;
		result.error_pos.line = 0;
		result.token_count = 0;

		while (!is_at_end()) {
			char c = current();

			if (c == '\0') {
				break;
			}
			else if (is_space(c)) {
				trim();
				if (c == '\n') {
					next_line();
				}
				continue;
			}

			if (is_character(c))
				ident();
			else if (is_number(c))
				number();
			else if (c == '"')
				string();
			else if (c == '=') {
				advance();
				if (current() == '=') {
					advance();
					make_token(cursor() - 2, token_type::Eq);
				}
				else {
					make_token(cursor() - 1, token_type::Assign);
				}
			}
			else if (c == '!') {
				advance();
				if (current() == '=') {
					advance();
					make_token(cursor() - 2, token_type::Ne);
				}	
			}
			else if (c == '<') {
				advance();
				if (current() == '=') {
					advance();
					make_token(cursor() - 2, token_type::Lte);
				}
				else {
					make_token(cursor() - 1, token_type::Lt);
				}
			}
			else if (c == '>') {
				advance();
				if (current() == '=') {
					advance();
					make_token(cursor() - 2, token_type::Gte);
				}
				else {
					make_token(cursor() - 1, token_type::Gt);
				}
			}
			else if (c == '+') {
				advance();
				make_token(cursor() - 1, token_type::Add);
			}
			else if (c == '-') {
				advance();
				if (current() == '>') {
					advance();
					make_token(cursor() - 2, token_type::Arrow);
				}
				else {
					make_token(cursor() - 1, token_type::Sub);
				}
			}
			else if (c == '*') {
				advance();
				make_token(cursor() - 1, token_type::Mul);
			}
			else if (c == '/') {
				advance();
				make_token(cursor() - 1, token_type::Div);
			}
			else if (c == ';') {
				advance();
				make_token(cursor() - 1, token_type::Semicolon);
			}
			else if (c == ':') {
				advance();
				make_token(cursor() - 1, token_type::Colon);
			}
			else if (c == '.') {
				advance();
				make_token(cursor() - 1, token_type::Period);
			}
			else if (c == ',') {
				advance();
				make_token(cursor() - 1, token_type::Comma);
			}
			else if (c == '{') {
				advance();
				make_token(cursor() - 1, token_type::LBrace);
			}
			else if (c == '}') {
				advance();
				make_token(cursor() - 1, token_type::RBrace);
			}
			else if (c == '(') {
				advance();
				make_token(cursor() - 1, token_type::LParen);
			}
			else if (c == ')') {
				advance();
				make_token(cursor() - 1, token_type::RParen);
			}
			else {
				set_error(
					&result,
					lexer_error::IllegalSymbol,
					std::format("Illegal Symbol: '{}'", c),
					m_position
				);

				// skip the illegal symbol
				advance();
				continue;
			}

			result.token_count++;
		}

		make_token(cursor(), token_type::Eof);
		return result;
	}

	token lexer::next_token()
	{
		if (m_tokens.empty()) {
			return {};
		}

		token result = m_tokens.front();
		m_tokens.pop_front();
		return result;
	}

	uint32_t lexer::cursor() const
	{
		return m_position.cursor;
	}

	uint32_t lexer::line() const
	{
		return m_position.line;
	}

	char lexer::current() const
	{
		return m_input[m_position.cursor];
	}

	void lexer::trim()
	{
		char c = current();

		while (!is_at_end() && is_space(c)) {
			c = advance();

			if (c == '\n') {
				next_line();
			}
		}
	}

	char lexer::advance()
	{
		if (is_at_end()) {
			return '\0';
		}

		m_position.cursor++;
		return current();
	}

	void lexer::next_line()
	{
		m_position.line++;
		m_line_start = cursor();
	}

	void lexer::ident()
	{
		uint32_t start = cursor();

		char c = advance();
		while (!is_at_end() && is_character(c)) {
			c = advance();
		}

		make_token(start, token_type::Ident);
	}

	void lexer::number()
	{
		uint32_t start = cursor();

		char c = advance();
		while (!is_at_end() && is_number(c)) {
			c = advance();
		}

		// we have a float
		if (c == '.') {
			c = advance();
			while (!is_at_end() && is_number(c)) {
				c = advance();
			}

			make_token(start, token_type::Float);
			return;
		}

		make_token(start, token_type::Number);
	}

	void lexer::string()
	{
		uint32_t start = cursor();

		char c = advance();
		while (!is_at_end() && c != '"') {
			c = advance();

			if (c == '\n') {
				next_line();
			}
		}

		// skip closing quote
		advance();

		make_token(start, token_type::String);
	}

	bool lexer::is_at_end() const
	{
		return cursor() >= m_input.size();
	}

	bool lexer::is_space(char c) const
	{
		return std::isspace(c);
	}

	bool lexer::is_character(char c) const
	{
		return std::isalpha(c) || c == '_';
	}

	bool lexer::is_number(char c) const
	{
		return std::isdigit(c);
	}

	token lexer::make_token(uint32_t start, token_type type)
	{
		token result;
		
		std::string lexeme = m_input.substr(start, (size_t)cursor() - start);
		result.lexeme = lexeme;

		position pos;
		pos.cursor = start - m_line_start + 1; // +1 for debug purposes
		pos.line = line();
		result.pos = pos;

		if (type == token_type::Ident) {
			if (s_keywords.contains(lexeme)) {
				type = s_keywords.at(lexeme);
			}
		}

		result.type = type;
		m_tokens.push_back(result);
		return result;
	}

	void lexer::populate_keywords()
	{
		s_keywords["proc"] = token_type::Proc;
		s_keywords["return"] = token_type::Return;
	}

}
