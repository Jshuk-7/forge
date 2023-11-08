#include <string>
#include <format>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "token.h"
#include "lexer.h"

namespace fginternal {

	std::string get_platform() {
		return "x86_64";
	}

}

namespace forge {

#define FORGE_VERSION "v0.1.0"

	enum class forge_result
	{
		Success = 0,
		Failure,
	};

	struct forge_output
	{
		forge_result result;
		uint32_t error_count;
	};

	std::string read_file(const std::filesystem::path& filepath) {
		std::string contents;
		std::ifstream file(filepath);

		if (!file.is_open()) {
			std::string error = std::format("Error: failed to open file: /{}", filepath.string());
			printf("%s\n", error.c_str());
			return "";
		}

		file.seekg(0, std::ios::end);
		size_t file_size = file.tellg();
		contents.resize(file_size);
		file.seekg(0, std::ios::beg);
		file.read(contents.data(), file_size);

		file.close();
		return contents;
	}

	forge_output process(const std::string& script_name, const std::string& input) {
		forge_output out;
		out.result = forge_result::Success;
		out.error_count = 0;

		lexer lexer_ = lexer();
		lexer_.set_input(input);
		lexer_output output = lexer_.process_input();

		printf("Tokens Generated: %u\n", output.token_count);
		
		if (output.error != lexer_error::None) {
			out.result = forge_result::Failure;
			out.error_count++;
			printf("<%s:%u:%u> Error: %s\n",
				script_name.c_str(),
				output.error_pos.line,
				output.error_pos.cursor,
				output.error_desc.c_str()
			);
		}

		token token_ = lexer_.next_token();
		while (token_.type != token_type::Eof) {
			utils::print_token(token_, script_name);
			token_ = lexer_.next_token();
		}

		return out;
	}

	forge_output interpret(const std::string& script_name, const std::string& input) {
		forge_output output = process(script_name, input);
		if (output.result == forge_result::Failure) {
			printf("%u error%sgenerated\n",
				output.error_count,
				output.error_count > 1 ? "s " : " "
			);
		}
		return output;
	}

	void execute_script(const std::filesystem::path& script) {
		std::string contents = read_file(script);

		if (contents.empty()) {
			return;
		}

		interpret(script.string(), contents);
		getchar();
	}

	void start_repl() {
		std::string platform = ::fginternal::get_platform();
		printf("Forge %s on platform %s\n", FORGE_VERSION, platform.c_str());

		for (;;) {
			std::string input;
			printf(">> ");
			std::getline(std::cin, input);

			if (input == "exit") {
				return;
			}

			interpret("<stdin>", input);
		}
	}

}
