/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#include <fstream>
#include <string>
#include "StrTools.h"
#include "IniParser.h"

IniParser::IniParser(const string &filename) : filename(filename)
{
	dirty = false;
	updateCnt = 0;
	reload();
}

void IniParser::changed()
{
	dirty = true;
	if (updateCnt == 0) flush();
}

void IniParser::beginUpdate()
{
	updateCnt++;
}

void IniParser::endUpdate()
{
	if (updateCnt == 0) throw IniException("endUpdate without beginUpdate");
	updateCnt--;
	if (dirty || updateCnt == 0)
		flush();
}

void IniParser::reload()
{
	sections.clear();
	ifstream ifs(filename.c_str());
	vector<string> fileLines;
	if (ifs.is_open())
	{
		for (string line; getline(ifs, line);)
			fileLines.push_back(line);
		ifs.close();
	}
	Sect *psect = new Sect();
	size_t sectCount = 0;
	vector<string> comments;
	for (size_t i = 0; i < fileLines.size(); i++)
	{
		string curLine = StrTools::trim(fileLines[i]);
		if (curLine=="") continue;
		if (curLine[0] == ';')
		{
			comments.push_back(curLine.substr(1, curLine.length()-1));
			continue;
		}
		if (curLine[0] == '[')
		{
			if (sectCount > 0)
			{
				sections.push_back(psect);
				sectMap[psect->sectHeader] = sectCount-1;
				psect = new Sect();
			}
			if (curLine.back() != ']') throw IniException("Invalid section: " + curLine + " in file "+filename);
			psect->sectHeader = curLine.substr(1, curLine.length() - 2);
			psect->commentsBefore = comments;
			comments.clear();
			sectCount++;
		}
		else
		{
			size_t eqPos = curLine.find('=');
			if (eqPos == string::npos) throw IniException("Invalid key without '=': " + curLine + " in file " + filename);
			Trio trio;
			trio.key = StrTools::trimRight(curLine.substr(0, eqPos));
			trio.val = StrTools::trimLeft(curLine.substr(eqPos + 1, curLine.length() - eqPos - 1));
			trio.comments = comments;
			comments.clear();
			psect->keysMap[trio.key] = psect->keysval.size();
			psect->keysval.push_back(trio);
		}
	}
	if (sectCount > 0)
	{
		sections.push_back(psect);
		sectMap[psect->sectHeader] = sectCount-1;
	}
}

void IniParser::flush()
{
	ofstream ofs(filename.c_str());
	for (size_t i = 0; i < sections.size(); i++)
	{
		if (i > 0) ofs << endl;
		for (size_t j = 0; j < sections[i]->commentsBefore.size(); j++)
			ofs << ";" << sections[i]->commentsBefore[j] << endl;
		ofs << "[" << sections[i]->sectHeader << "]" << endl;
		for (size_t j = 0; j < sections[i]->keysval.size(); j++)
		{
			for (size_t k = 0; k < sections[i]->keysval[j].comments.size(); k++)
				ofs << ";" << sections[i]->keysval[j].comments[k] << endl;
			ofs << sections[i]->keysval[j].key << "=" << sections[i]->keysval[j].val << endl;
		}
	}
	ofs.close();
	dirty = false;
}

void IniParser::freeStructs()
{
	for (size_t i = 0; i < sections.size(); i++)
		delete sections[i];
	sections.clear();
}

void IniParser::clearAll()
{
	freeStructs();
	changed();
}

IniParser::~IniParser()
{
	if (dirty) flush();
	freeStructs();
}

bool IniParser::sectionExists(const string &sectStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	return it != sectMap.end();
}

bool IniParser::keyExists(const string &sectStr, const string &keyStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) return false;
	Sect* sect = sections[(*it).second];
	unordered_map<string, size_t>::const_iterator it2 = sect->keysMap.find(keyStr);
	return it2 != sect->keysMap.end();
}

vector<string> IniParser::readSections()
{
	vector<string> sectNames;
	for (size_t i = 0; i < sections.size(); i++)
		sectNames.push_back(sections[i]->sectHeader);
	return sectNames;
}

vector<string> IniParser::readSection(const string &sectStr)
{
	vector<string> keyNames;
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) throw IniException("Section " + sectStr + " not found in file " + filename);
	size_t index = (*it).second;
	Sect* sect = sections[index];
	for (size_t i = 0; i < sect->keysval.size(); i++)
		keyNames.push_back(sect->keysval[i].key);
	return keyNames;
}

void IniParser::eraseSection(const string &sectStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) throw IniException("Section " + sectStr + " not found in file " + filename);
	Sect* sect = sections[(*it).second];
	sect->keysMap.clear();
	sect->keysval.clear();
	changed();
}

void IniParser::deleteSection(const string &sectStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) throw IniException("Section " + sectStr + " not found in file " + filename);
	size_t index = (*it).second;
	Sect* sect = sections[index];
	delete sect;
	sections.erase(sections.begin() + index);
	sectMap.erase(it);
	changed();
}

void IniParser::deleteKey(const string &sectStr, const string &keyStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) throw IniException("Section " + sectStr + " not found in file " + filename);
	Sect* sect = sections[(*it).second];
	unordered_map<string, size_t>::const_iterator it2 = sect->keysMap.find(keyStr);
	if (it2 == sect->keysMap.end()) throw IniException("Key " + keyStr + " not found in file " + filename + " in section " + sectStr);
	sect->keysval.erase(sect->keysval.begin() + (*it2).second);
	sect->keysMap.erase(it2);
	changed();
}

string IniParser::readString(const string &sectStr, const string &keyStr, const string &default)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) return default;
	Sect* sect = sections[(*it).second];
	unordered_map<string, size_t>::const_iterator it2 = sect->keysMap.find(keyStr);
	if (it2 == sect->keysMap.end()) return default;
	return sect->keysval[(*it2).second].val;
}

long long IniParser::readLong(const string &sectStr, const string &keyStr, const long long default)
{
	string str = readString(sectStr, keyStr, to_string(default));
	return atoll(str.c_str());
}

double IniParser::readDouble(const string &sectStr, const string &keyStr, const double default)
{
	string str = readString(sectStr, keyStr, to_string(default));
	return atof(str.c_str());
}

bool IniParser::readBool(const string &sectStr, const string &keyStr, const bool default)
{
	long long num = readLong(sectStr, keyStr, (long long)default);
	return num != 0;
}

//three cases: new seciotn, new key and replace value
void IniParser::writeString(const string &sectStr, const string &keyStr, const string &valueStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end())
	{
		dirty = true;
		Sect *psect = new Sect();
		Trio trio;
		trio.key = keyStr;
		trio.val = valueStr;
		psect->keysval.push_back(trio);
		psect->keysMap[keyStr] = 0;
		psect->sectHeader = sectStr;
		sections.push_back(psect);
		return;
	}
	Sect* sect = sections[(*it).second];
	unordered_map<string, size_t>::const_iterator it2 = sect->keysMap.find(keyStr);
	if (it2 == sect->keysMap.end())
	{
		dirty = true;
		Trio trio;
		trio.key = keyStr;
		trio.val = valueStr;
		sect->keysMap[keyStr] = sect->keysval.size();
		sect->keysval.push_back(trio);
		return;
	}
	size_t index = (*it2).second;
	if (sect->keysval[index].val != valueStr)
	{
		dirty = true;
		sect->keysval[index].val = valueStr;
	}
}

void IniParser::writeLong(const string &sectStr, const string &keyStr, const long long value)
{
	writeString(sectStr, keyStr, to_string(value));
}

void IniParser::writeDouble(const string &sectStr, const string &keyStr, const double value)
{
	writeString(sectStr, keyStr, to_string(value));
}

void IniParser::writeBool(const string &sectStr, const string &keyStr, const bool value)
{
	writeLong(sectStr, keyStr, value?1:0);
}