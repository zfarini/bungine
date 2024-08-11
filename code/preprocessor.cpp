#include <iostream>
#include <fstream>
#include <ctype.h>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

template<typename T>
string to_string(const T& x)
{
	stringstream ss;

	ss << x;
	return ss.str();
}

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
	TOKEN_META,
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
			else if (token.str == "meta") token.type = TOKEN_META;
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
			if (strchr("+-*/%<>();{}[]!^~&|=,:", s[i])) {
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

struct MetaInfo {
	struct {
		bool generate;
		bool is_const;
		bool is_color;
	} ui;
	bool serialize;
};

struct StructMemberData {
	string name;
	string type_name;
	int is_array;
	string array_size;
	MetaInfo meta;
};

struct StructMetaData {
	string name;
	int member_count;
	StructMemberData members[64];
	bool valid;
};

struct EnumMetaData {
	string name;
	vector<pair<string, string>> elems;
};

vector<Token> tokens;
int ti;

int skip_token(int type) {
	if (tokens[ti].type == type) {
		ti++;
		return true;
	}
	return false;
}

StructMetaData get_struct_info()
{
	StructMetaData sdata = {};

	if (!skip_token(TOKEN_STRUCT))
		return sdata;

	sdata.name = tokens[ti].str;
	if (sdata.name == "Array")
		return sdata;

	if (!skip_token(TOKEN_IDENTIFIER))
		return sdata ;
	if (!skip_token('{'))
		return sdata;
	while (ti < tokens.size() && !skip_token('}')) {
		bool skip = false;

		StructMemberData &member = sdata.members[sdata.member_count];
		member = {};

		if (tokens[ti].type == TOKEN_META) {
			ti++;
			skip_token('(');
			while (ti < tokens.size() && !skip_token(')')) {
				if (tokens[ti].str == "ui") {
					member.meta.ui.generate = true;
					ti++;
					if (tokens[ti].str == ":") {
						ti++;
						if (tokens[ti].str == "const")
							member.meta.ui.is_const = true;
						else if (tokens[ti].str == "color")
							member.meta.ui.is_color = true;
					}
					skip_token(',');
				}
				else if (tokens[ti].str == "serialize") {
					member.meta.serialize = true;
					ti++;
					skip_token(',');
				}
				else
					ti++;
			}
		}
		if (tokens[ti].type != TOKEN_IDENTIFIER)
			skip = true;

		member.type_name = tokens[ti++].str;
		if (tokens[ti].type == ':' && tokens[ti + 1].type == ':') {
			ti += 2;
			member.type_name += "::" + tokens[ti++].str;
		}
		if (tokens[ti].type == '<') {
			while (ti < tokens.size() && tokens[ti].type != '>')
				member.type_name += tokens[ti++].str;
			member.type_name += ">";
			skip_token('>');
		}

		while (ti < tokens.size() && tokens[ti].type == '*')
			member.type_name += tokens[ti++].str;

		if (tokens[ti].type != TOKEN_IDENTIFIER)
			skip = true;

		member.name = tokens[ti++].str;

		if (tokens[ti].type == '[') {
			ti++;
			while (ti < tokens.size() && tokens[ti].type != ']')
				member.array_size += tokens[ti++].str;
			skip_token(']');
			member.is_array = true;
		}
		while (ti < tokens.size() && tokens[ti].type != ';')
			ti++;
		skip_token(';');
		if (!skip)
			sdata.member_count++;
		if (skip) {
			while (ti < tokens.size() && tokens[ti].type != '}')
				ti++;
		}
	}

	sdata.valid = true;
	return sdata;
}

string generate()
{
	ti = 0;

	string result;
	string header;

	header += "#ifndef DISABLE_PREPROCESSOR\n";

	header += "struct StructMemberData {";
	header += "const char *name;";
	header += "const char *type_name;";
	header += "int is_array;";
	header += "int array_size;";
	header += "};";

	header += "struct StructMetaData {";
	header += "const char *name;";
	header += "int member_count;";
	header += "StructMemberData members[64];";
	header += "};";

	unordered_set<string> base_types = {
		"int", "int32_t", "size_t", "bool", "v2", "v3", "quat", "mat4", "float", "double", "char"
	};

	for (auto type: base_types) {
		header += "void serialize_" + type + "(FILE *fd, bool w, " + type + "&x) {";
		if (type == "bool") {
			header += "int v = x;";
			header += "if (w) fwrite(&v, sizeof(int), 1, fd);";
			header += "else {";
			header += "(void)fread(&v, sizeof(int), 1, fd);";
			header += "x = v;";
			header += "}";
		}
		else {
			header += "if (w) fwrite(&x, sizeof(" + type + "), 1, fd);";
			header += "else {(void)fread(&x, sizeof(" + type + "), 1, fd);}";
		}
		header += "}";
	}

	unordered_map<string, StructMetaData> structs;
	unordered_map<string, string> typedefed_structs;
	unordered_map<string, EnumMetaData> enums;

	while (ti < tokens.size()) {
		if (tokens[ti].type == TOKEN_ENUM) {
			ti++;
			EnumMetaData data = {};
			data.name = tokens[ti].str;
			if (!skip_token(TOKEN_IDENTIFIER)) 
				continue ;
			if (!skip_token('{'))
				continue ;

			while (ti < tokens.size() && !skip_token('}')) {
				if (tokens[ti].type != TOKEN_IDENTIFIER) {
					ti++;
					continue ;
				}
				string name, value;

				name = tokens[ti++].str;
				while (ti < tokens.size() && tokens[ti].type != ',' && tokens[ti].type != '}')
					value += tokens[ti++].str;
				skip_token(',');
				data.elems.push_back({name, value});
			}
			enums[data.name] = data;
		}
		else if (tokens[ti].type == TOKEN_STRUCT) {
			StructMetaData sdata = get_struct_info();
			if (sdata.valid)
				structs[sdata.name] = sdata;
		}
		else if (tokens[ti].str == "typedef") {
			ti++;
			string type1;
			while (ti + 1 < tokens.size() && tokens[ti + 1].type != ';')
				type1 += tokens[ti++].str;
			string type2 = tokens[ti++].str;
			skip_token(';');
			
			if (!typedefed_structs.count(type1))
				typedefed_structs[type2] = type1;
			else
				typedefed_structs[type2] = typedefed_structs[type1];
		}
		else
			ti++;
	}

	for (auto &[name, data]: structs) {
		header += "static StructMetaData get_struct_" + name + "_info();";
		header += "static void imgui_edit_struct_" + name + "(" + name +  " &x, const char *name, bool collapsed);";
		header += "static void serialize_" + name + "(FILE *fd, bool w, " + name + " &x, Arena *arena);";
	}

	for (auto &[name, data]: enums) {
		string s;
		s += "static const char *get_enum_" + name + "_str(int value){";
		s += "switch(value){";
		for (auto [ename, value] : data.elems)
			s += "case " + ename + ": return \"" + ename + "\";";
		s += "} return \"Enum_" + name + "_Unknown\";}";
		result += s;
	}

	for (auto &[sname, sdata]: structs) {

		string s;

		s += "StructMetaData get_struct_" + sname + "_info(){";
		s += "StructMetaData data = {};";
		s += "data.name = \"" + sname + "\";";
		s += "data.member_count = " + to_string(sdata.member_count) + ";";

		for (int j = 0; j < sdata.member_count; j++) {
			string idx = "data.members[" + to_string(j) + "]";

			s += idx + ".name = \"" + sdata.members[j].name + "\";";
			s += idx + ".type_name = \"" + sdata.members[j].type_name + "\";";
			if (sdata.members[j].is_array) {
				s += idx + ".is_array = 1;";
				s += idx + ".array_size = " + sdata.members[j].array_size + ";";
			}
		}
		s += "return data;";
		s += "}";
		result += s;
	}

#define T(c, ...) string('', c) + "\"" + #__VA_ARGS__ + "\""

	auto gen_ui = [&](auto &&self, string struct_name, 
			string name, 
			bool is_child,
			string aname,
			StructMemberData m) -> string {
		string type_name = struct_name;
		if (typedefed_structs.count(struct_name))
			type_name = typedefed_structs[struct_name];

		string v = aname;

		string s;

		if (type_name == "v2") {
			s += "ImGui::InputFloat2(\"" + name + "\", " + v + ".e);";
		}
		else if (type_name == "v3") {
			if (m.meta.ui.is_color)
				s += "ImGui::ColorEdit3(\"" + name + "\", " + v + ".e);";
			else if (m.meta.ui.is_const)
				s += "ImGui::Text(\"V3(%f, %f, %f)\", " + v + ".e[0], " + v + ".e[1], " + v + ".e[2]);";
			else
				s += "ImGui::InputFloat3(\"" + name + "\", " + v + ".e);";
		}
		else if (type_name == "quat") {
			s += "ImGui::InputFloat4(\"" + name + "\", " + v + ".e);";
			// TODO: normalize quat?
		}
		else if (type_name == "bool" || struct_name == "b32") { 
			s += "{bool tmp = " + v + ";"
				+ "ImGui::Checkbox(\"" + name + "\", &tmp);"
				+ v + " = tmp; }";
		}
		else if (type_name == "int32_t" || type_name == "int") {
			s += "ImGui::InputInt(\"" + name + "\", &" + v + ");";
		}
		else if (type_name == "uint32_t") {

		}
		else if (type_name == "size_t" || type_name == "longunsignedint") {
			if (m.meta.ui.is_const)
				s += "ImGui::Text(\"" + name + " = %lu\"," + v + ");";
			else
				s += "ImGui::InputScalar(\"" + name + "\", ImGuiDataType_U64, &"  + v + ");"; 
		}
		else if (type_name == "float") {
			s += "ImGui::InputFloat(\"" + name + "\", &" + v + ");";
		}
		else if (struct_name == "String") {
			s += "ImGui::Text(\"" + name + ": \\\"" + "%.*s\\\"\", (int)" + v + ".count, " + v + ".data);";
		}
		else if (type_name.substr(0, 6) == "Array<") {
			assert(is_child);
			string type = type_name.substr(6);
			type.pop_back();

			string org_type = type;

			s += "if (ImGui::CollapsingHeader(\"" + name + "\")) {";

			s += "for (int i = 0; i < " + v + ".count; i++){";
			if (typedefed_structs.count(type))
				type = typedefed_structs[type];
			if (structs.count(type)) {
				
				s += "char elem_name[32];";
				s += "snprintf(elem_name, sizeof(elem_name), \"" + name + "[%d]\", i);";

				s += "imgui_edit_struct_" + type + "(" + aname + ".data[i], elem_name, true);";
			}
			else {
				s += self(self, type, "", true, aname + ".data[i]", m);
			}
			s += "}";
			s += "}";
		}
		else {
			if (enums.count(type_name)) {
				s += "imgui_edit_enum_" + type_name + "(" + aname + ", \"" + name + "\");";
				return s;
			}
			if (!structs.count(type_name))
				return "";
			if (is_child) {
				s += "imgui_edit_struct_" + type_name + "(" + aname + ", \"" + name + "\", true);";
			}
			else {
				s += "if (!collapsed || ImGui::CollapsingHeader(name)){";
				auto sdata = structs[type_name];

				for (int i = 0; i < sdata.member_count; i++) {
					if (sdata.members[i].meta.ui.generate) {
						string fullname = aname + "." + sdata.members[i].name;
						s += self(self, sdata.members[i].type_name, sdata.members[i].name,
								true, fullname, sdata.members[i]);
					}
				}
				s += "}";
			}
		}
		return s;
	};

	for (auto [name, data]: enums) {
		string s;

		s += "static void imgui_edit_enum_" + name + "(" + name + " &x, const char *name){";
		s += "const char *items[" + to_string(data.elems.size()) + "];";
		s += name + " items_type[" + to_string(data.elems.size()) + "];";
		s += "int curr = 0;";
		for (int i = 0; i < data.elems.size(); i++) {
			s += "items[" + to_string(i) + "] = \"" + data.elems[i].first + "\";";
			s += "items_type[" + to_string(i) + "] = " + data.elems[i].first + ";";
			s += "if (x == " + data.elems[i].first + ") curr = " + to_string(i) + ";";
		}
		s += "if (ImGui::CollapsingHeader(name))";
		s += "ImGui::ListBox(name, &curr, items, " + to_string(data.elems.size()) + ");";
		s += "x = items_type[curr];";
		s += "}";
		result += s;
	}
	for (auto [sname, sdata]: structs) {
		string s;

		bool generate_ui = false;
		for (int i = 0; !generate_ui && i < sdata.member_count; i++)
			if (sdata.members[i].meta.ui.generate)
				generate_ui = true;
		if (generate_ui) {
			s += "static void imgui_edit_struct_" + sdata.name + "(" + sdata.name +  " &x, const char *name, bool collapsed = true) {";
			StructMemberData member_data = {};
			s += gen_ui(gen_ui, sdata.name, "x", false, "x", member_data);
			s += "}";
		}

		{
			s += "static void serialize_" + sname +"(FILE *fd, bool w, " + sname + " &x, Arena *arena = 0){";
			for (int i = 0; i < sdata.member_count; i++) {
				const StructMemberData &m = sdata.members[i];
				if (m.meta.serialize) {
					string type_name = m.type_name;
					if (typedefed_structs.count(type_name))
						type_name = typedefed_structs[type_name];
					string v = "x." + m.name;

					if (type_name.substr(0, 6) == "Array<") {
						string inner_type = type_name.substr(6);
						inner_type.pop_back();

						s += "if (w) {";
						s += "serialize_size_t(fd, w, " + v + ".capacity);";
						s += "serialize_size_t(fd, w, " + v + ".count);";
						s += "for (int i = 0; i < " + v + ".count; i++)";
						s += "serialize_" + inner_type + "(fd, w, " + v + ".data[i], arena);";
						s += "}";
						s += "else {";
						s += "assert(arena);";
						s += "usize capacity, count;";
						s += "serialize_size_t(fd, w, capacity);";
						s += "serialize_size_t(fd, w, count);";
						s += v + " = make_array_max<" + inner_type + ">(arena, capacity);";
						s += "for (int i = 0; i < count; i++) {";
						s += inner_type + " y = {};";
						s += "serialize_" + inner_type + "(fd, w, y, arena);";
						s += v + ".push(y);";
						s += "}";
						s += "}";
					}
					else {
						if (base_types.count(type_name))
							s += "serialize_" + type_name + "(fd, w, " + v + ");";
						else if (structs.count(type_name))
							s += "serialize_" + type_name + "(fd, w, " + v + ", arena);";
						else if (enums.count(type_name)) {
							s += "{";
							s += "int tmp = " + v + ";";
							s += "serialize_int(fd, w, tmp);";
							s += "if (!w) " + v + " = (" + type_name + ")tmp;";
							s += "}";
						}
						else {
							cerr << "COULDN'T FIND SERIALIZATION FUNCTION FOR TYPE: " << type_name << endl;
							assert(0);
						}
					}
				}
			}

			s += "}";
		}

		result += s;
	}

	result = header + result;
	result += "\n#endif\n";
	return result;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		cerr << "usage: " << argv[0] << " input_file output_file" << endl;
		return 0;
	}

	string input = load_file(argv[1]);

	tokens = tokenize(input);
	//for (int i = 0; ti < tokens.size(); i++)
	//	cout << '"' << tokens[ti].str << '"' << "";

	string s = generate();

	write_to_file(argv[2], s);
}
