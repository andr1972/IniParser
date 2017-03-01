/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "IniException.h"

using namespace std;

class IniParser
{
private:
	struct Trio
	{
		vector<string> comments;
		string key;
		string val;
	};

	struct Sect
	{
		vector<string> commentsBefore;
		string sectHeader;
		vector<Trio> keysval;
		unordered_map<string, size_t> keysMap;
	};

	string filename;
	vector<Sect*> sections;
	unordered_map<string, size_t> sectMap;
	bool dirty;
	void flush();
	void changed();
	void freeStructs();
	size_t updateCnt;
	IniParser::Sect* getSectPtr(const string &sectStr);
public:
	IniParser(const string &filename, bool mustExist);
	~IniParser();
	void beginUpdate();
	void endUpdate();
	void reload(bool mustExist);
	void clearAll();
	bool sectionExists(const string &sectStr);
	bool keyExists(const string &sectStr, const string &keyStr);
	vector<string> readSections();
	vector<string> readSectionKeys(const string &sectStr);
	vector<pair<string, string>> readSectionPairs(const string &sectStr);
	unordered_map<string, string> readSectionMap(const string &sectStr);
	static string mapValue(unordered_map<string, string> map, const string &key);
	static string mapValueDef(unordered_map<string, string> map, const string &key, const string &def);
	void eraseSection(const string &sectStr);
	void deleteSection(const string &sectStr);
	void deleteKey(const string &sectStr, const string &keyStr);
	bool tryReadString(string &result, const string &sectStr, const string &keyStr);
	string readString(const string &sectStr, const string &keyStr);
	string readStringDef(const string &sectStr, const string &keyStr, const string &def);
	int readInt32(const string &sectStr, const string &keyStr);
	int readInt32Def(const string &sectStr, const string &keyStr, const long long def);
	long long readInt64(const string &sectStr, const string &keyStr);
	long long readInt64Def(const string &sectStr, const string &keyStr, const long long def);
	double readDouble(const string &sectStr, const string &keyStr);
	double readDoubleDef(const string &sectStr, const string &keyStr, const double def);
	bool readBool(const string &sectStr, const string &keyStr);
	bool readBoolDef(const string &sectStr, const string &keyStr, const bool def);
	void writeString(const string &sectStr, const string &keyStr, const string &valueStr);
	void writeLong(const string &sectStr, const string &keyStr, const long long value);
	void writeDouble(const string &sectStr, const string &keyStr, const double value);
	void writeBool(const string &sectStr, const string &keyStr, const bool value);
};

