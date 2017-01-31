/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#include <fstream>
#include <string>
#include "StrTools.h"
#include "IniParser.h"

IniParser::IniParser(const string &filename, bool mustExist) : filename(filename)
{
	dirty = false;
	updateCnt = 0;
	reload(mustExist);
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

void IniParser::reload(bool mustExist)
{
	freeStructs();
	ifstream ifs(filename.c_str());
	vector<string> fileLines;
	if (ifs.is_open())
	{
		for (string line; getline(ifs, line);)
			fileLines.push_back(line);
		ifs.close();
	}
	else if (mustExist) throw IniException(filename + " not exists");
	Sect *psect = NULL;
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
			psect = new Sect();
			if (curLine.back() != ']') throw IniException("Invalid section: " + curLine + " in file "+filename);
			psect->sectHeader = curLine.substr(1, curLine.length() - 2);
			psect->commentsBefore = comments;
			comments.clear();

			unordered_map<string, size_t>::const_iterator it = sectMap.find(psect->sectHeader);
			if (it != sectMap.end())
				throw IniException("Duplicate section [" + psect->sectHeader + "] in file " + filename);
			sectCount++;
			sections.push_back(psect);
			sectMap[psect->sectHeader] = sectCount - 1;
		}
		else
		{
			if (psect==NULL) throw IniException("Key before first section in line: \"" + curLine + "\" in file " + filename);
			size_t eqPos = curLine.find('=');
			if (eqPos == string::npos) throw IniException("Invalid key without '=': " + curLine + " in file " + filename);
			Trio trio;
			trio.key = StrTools::trimRight(curLine.substr(0, eqPos));
			trio.val = StrTools::trimLeft(curLine.substr(eqPos + 1, curLine.length() - eqPos - 1));
			trio.comments = comments;
			comments.clear();

			unordered_map<string, size_t>::const_iterator it2 = psect->keysMap.find(trio.key);
            if (it2 != psect->keysMap.end())
                throw IniException("Duplicate key \"" + trio.key + "\"in section [" + psect->sectHeader+ "] in file " + filename);
			psect->keysMap[trio.key] = psect->keysval.size();
			psect->keysval.push_back(trio);
		}
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

IniParser::Sect* IniParser::getSectPtr(const string &sectStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) throw IniException("Section " + sectStr + " not found in file " + filename);
	size_t index = (*it).second;
	return sections[index];
}

vector<string> IniParser::readSectionKeys(const string &sectStr)
{
	Sect* sect = getSectPtr(sectStr);
	vector<string> keyNames;
	for (size_t i = 0; i < sect->keysval.size(); i++)
		keyNames.push_back(sect->keysval[i].key);
	return keyNames;
}

vector<pair<string,string>> IniParser::readSectionPairs(const string &sectStr)
{
	Sect* sect = getSectPtr(sectStr);
	vector<pair<string, string>> keyPairs;
	for (size_t i = 0; i < sect->keysval.size(); i++)
	{
		Trio t = sect->keysval[i];
		keyPairs.push_back(make_pair(t.key, t.val));
	}
	return keyPairs;
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

bool IniParser::tryReadString(string &result, const string &sectStr, const string &keyStr)
{
	unordered_map<string, size_t>::const_iterator it = sectMap.find(sectStr);
	if (it == sectMap.end()) return false;
	Sect* sect = sections[(*it).second];
	unordered_map<string, size_t>::const_iterator it2 = sect->keysMap.find(keyStr);
	if (it2 == sect->keysMap.end()) return false;
	result = sect->keysval[(*it2).second].val;
	return true;
}

string IniParser::readString(const string &sectStr, const string &keyStr)
{
	string result;
	if (tryReadString(result, sectStr, keyStr))
		return result;
	else
		throw IniException("Not found key "+keyStr+" in section "+sectStr);
}

string IniParser::readStringDef(const string &sectStr, const string &keyStr, const string &def)
{
	string result;
	if (tryReadString(result, sectStr, keyStr))
		return result;
	else
		return def;
}

long long IniParser::readInt(const string &sectStr, const string &keyStr)
{
	string str = readString(sectStr, keyStr);
	return atoll(str.c_str());
}

long long IniParser::readIntDef(const string &sectStr, const string &keyStr, const long long def)
{
	string str = readStringDef(sectStr, keyStr, to_string(def));
	return atoll(str.c_str());
}

double IniParser::readDouble(const string &sectStr, const string &keyStr)
{
	string str = readString(sectStr, keyStr);
	return atof(str.c_str());
}

double IniParser::readDoubleDef(const string &sectStr, const string &keyStr, const double def)
{
	string str = readStringDef(sectStr, keyStr, to_string(def));
	return atof(str.c_str());
}

bool IniParser::readBool(const string &sectStr, const string &keyStr)
{
	long long num = readInt(sectStr, keyStr);
	return num != 0;
}

bool IniParser::readBoolDef(const string &sectStr, const string &keyStr, const bool def)
{
	long long num = readIntDef(sectStr, keyStr, (long long)def);
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
