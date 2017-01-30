/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#pragma once
#include <iostream>

using namespace std;

class StrTools
{
public:
	static string trim(const string& str);
	static string trimLeft(const string& str);
	static string trimRight(const string& str);
};

