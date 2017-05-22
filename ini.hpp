#pragma once

#include <cctype>
#include <fstream>
#include <map>
#include <string>

#include <mps/str_util.hpp>

namespace {
	class INI {
	public:
		explicit INI(const std::string& path);

		std::map<std::string, Any>& operator[](const std::string& key) {
			return data[key];
		}

	private:
		void parse(std::istream& input);
		bool entry(std::istream& input);
		bool tag(std::istream& input);
		bool key_value(std::istream& input);
		bool value(std::istream& input);

		class IniVal {
		public:

		private:
			union {
				std::string s;
				double d;
				int i;
				bool b;
			};
		};

		std::map<std::string, std::map<std::string, IniVal>> data;
		std::string currentTag, currentKey;
	};

	INI::INI(const std::string& path)
	{
		std::ifstream ifs{ path };
		if (!ifs)
			throw std::runtime_error{ "INI - Unable to open file " + path };
		parse(ifs);
	}

	void INI::parse(std::istream& is)
	{
		while (entry(is)) {}
	}

	bool INI::entry(std::istream& is)
	{
		if (tag(is)) {
			while (key_value(is)) {}
			return true;
		}
		return false;
	}

	bool INI::tag(std::istream& is)
	{
		char c;
		std::string s;
		if (is.get(c) && c == '[') {
			while (is.get(c) && c != '\n') {
				if (c == ']' && s.size()) {
					currentTag = s;
					return true;
				}
				else s += c;
			}
		}
		return false;
	}

	bool INI::key_value(std::istream& is)
	{
		std::string key;
		
		for (char c; is.get(c) ;) {
			if (c == '=') {
				currentKey = key;
				return value(is);
			}
			else key += c;
		}
		return false;
	}

	bool INI::value(std::istream& is)
	{
		if (std::isdigit(is.peek()) || is.peek() == '.') {
			double d;
			if (is >> d) {
				data[currentTag][currentKey] = d;
				return true;
			}
			return false;
		}
		std::string s;
		if (std::getline(is, s) && s.size()) {
			data[currentTag][currentKey] = s;
			return true;
		}
		return false;
	}
}

/**				INI grammar
 *
 *	start = {entry}
 *	entry = tag {key_value}
 *	tag = '[' string ']'
 *	key_value = string '=' value
 *	value = string | number | 'on' | 'off' | 'true' | 'false'
 *
 */	