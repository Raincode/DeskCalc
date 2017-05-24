#pragma once

#include <iostream>
#include <string>
//
struct Warning {
	Warning(const std::string& msg) 
	{ 
		std::cout << msg << '\n';
	}
};