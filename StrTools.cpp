/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#include "StrTools.h"

using namespace std;

//http://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
string StrTools::trim(const string& str)
{
	const string whitespace = " \t";
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}


string StrTools::trimLeft(const string& str)
{
	const auto strBegin = str.find_first_not_of(" \t");
	if (strBegin == string::npos)
		return ""; // no content
	return str.substr(strBegin, str.length() - strBegin);
}


string StrTools::trimRight(const string& str)
{
	const auto strEnd = str.find_last_not_of(" \t");
	if (strEnd == string::npos)
		return ""; // no content
	return str.substr(0, strEnd + 1);
}
