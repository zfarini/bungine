#include <iostream>
#include <fstream>
#include <ctype.h>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>

using namespace std;

string load_file(const string &filename)
{
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "PREPROCESSOR: failed to open file " << filename << " for reading" << endl;
		exit(1);
	}
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	string buffer(size, ' ');
	file.seekg(0);
	file.read(&buffer[0], size);
	file.close();
	return buffer;
}

enum TokenType
{
	TOKEN_UNKNOWN = 1,
	TOKEN_NUMBER = 256,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_STRUCT,
	TOKEN_ENUM,
};

struct Token
{
	int type;
	string str;
	uint64_t ivalue;
	int line;
	int col;
};

vector<Token> tokenize(const string &s)
{
	int i = 0;
	int line = 1;
	int column = 0;

	auto skip_char = [&]() {
		if (i >= s.size())
			return ;
		if (s[i] == '\n') {
			line++;
			column = 1;
		}
		else
			column++;
		i++;
	};

	vector<Token> tokens;

	while (i < s.size())
	{
		if (isspace(s[i])) {
			skip_char();
			continue ;
		}
		if ((s[i] == '/' && s[i + 1] == '/') || (s[i] == '#')) {
			while (i < s.size() && s[i] != '\n')
				skip_char();
			continue ;
		}
		if (s[i] == '/' && s[i + 1] == '*') {
			skip_char();
			skip_char();
			while (i < s.size() && (s[i] != '*' || s[i + 1] != '/'))
				skip_char();
			continue ;
		}

		Token token = {};
		token.line = line;
		token.col = column;
		if (isdigit(s[i])) {
			token.type = TOKEN_NUMBER;
			while (isdigit(s[i])) {
				token.ivalue = token.ivalue * 10 + (s[i] - '0');
				token.str += s[i];
				skip_char();
			}
		}
		else if (isalpha(s[i]) || s[i] == '_') {
			token.type = TOKEN_IDENTIFIER;
			while (isalnum(s[i]) || s[i] == '_') {
				token.str += s[i];
				skip_char();
			}
			if (token.str == "struct") token.type = TOKEN_STRUCT;
			else if (token.str == "enum") token.type = TOKEN_ENUM;
		}
		else if (s[i] == '"') {
			token.type = TOKEN_STRING;
			skip_char();
			while (i < s.size() && s[i] != '"') {
				token.str += s[i];
				skip_char();
			}
			skip_char();
		}
		// TODO: handle '\n'..
		else {
			if (strchr("+-*/%<>();{}[]!^~&|=,", s[i])) {
				token.type = (int)s[i];
				token.str += s[i];
			}
			skip_char();
		}
		tokens.push_back(token);
	}
	return tokens;
}

void write_to_file(const string &filename, const string &s)
{
	ofstream file(filename);

	if (!file.is_open()) {
		cerr << "PREPROCESSOR: failed to open file " << filename << " for writing" << endl;
		exit(1);
	}
	file << s;
	file.close();
}

string generate(const vector<Token> &tokens)
{
	string result;

	result += "#ifndef DISABLE_PREPROCESSOR\n";

	int i = 0;

	auto skip_token = [&](int type) {
		if (tokens[i].type == type) {
			i++;
			return true;
		}
		return false;
	};

	while (i < tokens.size()) {
		if (tokens[i].type == TOKEN_ENUM) {
			i++;
			string name = tokens[i].str;
			if (!skip_token(TOKEN_IDENTIFIER)) 
				continue ;
			if (!skip_token('{'))
				continue ;
			string s;
			s += "const char *get_enum_" + name + "_str(int value)\n{\n";
			s += "\tswitch(value){\n";
			while (i < tokens.size() && !skip_token('}')) {
				if (tokens[i].type != TOKEN_IDENTIFIER) {
					i++;
					continue ;
				}
				s += "\t\tcase " + tokens[i].str + ": return \"" + tokens[i].str + "\";\n";
				i++;
				skip_token(',');
			}
			s += "\t}\n}\n";
			result += s;
		}
		else
			i++;
	}
	result += "#endif\n";

	//cerr << result << endl;
	return result;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		cerr << "usage: " << argv[0] << " input_file output_file" << endl;
		return 0;
	}

	string input = load_file(argv[1]);

	vector<Token> tokens = tokenize(input);
	//for (int i = 0; i < tokens.size(); i++)
	//	cout << '"' << tokens[i].str << '"' << "\n";

	string s = generate(tokens);

	write_to_file(argv[2], s);
}
