/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#pragma once
#include <iostream>

class StrTools
{
public:
	static std::string trim(const std::string& str);
	static std::string trimLeft(const std::string& str);
	static std::string trimRight(const std::string& str);
};

