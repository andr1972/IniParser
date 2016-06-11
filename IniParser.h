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
public:
	IniParser(const string &filename);
	~IniParser();
	void beginUpdate();
	void endUpdate();
	void reload();
	void clearAll();
	bool sectionExists(const string &sectStr);
	bool keyExists(const string &sectStr, const string &keyStr);
	vector<string> readSections();
	vector<string> readSection(const string &sectStr);
	void eraseSection(const string &sectStr);
	void deleteSection(const string &sectStr);
	void deleteKey(const string &sectStr, const string &keyStr);
	string readString(const string &sectStr, const string &keyStr, const string &default);
	long long readLong(const string &sectStr, const string &keyStr, const long long default);
	double readDouble(const string &sectStr, const string &keyStr, const double default);
	bool readBool(const string &sectStr, const string &keyStr, const bool default);
	void writeString(const string &sectStr, const string &keyStr, const string &valueStr);
	void writeLong(const string &sectStr, const string &keyStr, const long long value);
	void writeDouble(const string &sectStr, const string &keyStr, const double value);
	void writeBool(const string &sectStr, const string &keyStr, const bool value);
};

