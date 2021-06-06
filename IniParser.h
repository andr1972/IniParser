/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "IniException.h"

class IniParser
{
private:
	struct Trio
	{
        std::vector<std::string> comments;
        std::string key;
        std::string val;
	};

	struct Sect
	{
        std::vector<std::string> commentsBefore;
        std::string sectHeader;
        std::vector<Trio> keysval;
        std::unordered_map<std::string, size_t> keysMap;
	};

    std::string filename;
    std::vector<Sect*> sections;
    std::unordered_map<std::string, size_t> sectMap;
	bool dirty;
	void flush();
	void changed();
	void freeStructs();
	size_t updateCnt;
	IniParser::Sect* getSectPtr(const std::string &sectStr);
public:
	IniParser(const std::string &filename, bool mustExist);
	~IniParser();
	void beginUpdate();
	void endUpdate();
	void reload(bool mustExist);
	void clearAll();
	bool sectionExists(const std::string &sectStr);
	bool keyExists(const std::string &sectStr, const std::string &keyStr);
    std::vector<std::string> readSections();
    std::vector<std::string> readSectionKeys(const std::string &sectStr);
    std::vector<std::pair<std::string, std::string>> readSectionPairs(const std::string &sectStr);
    std::unordered_map<std::string, std::string> readSectionMap(const std::string &sectStr);
	static std::string mapValue(std::unordered_map<std::string, std::string> map, const std::string &key);
	static std::string mapValueDef(std::unordered_map<std::string, std::string> map, const std::string &key,
                                   const std::string &def);
	void eraseSection(const std::string &sectStr);
	void deleteSection(const std::string &sectStr);
	void deleteKey(const std::string &sectStr, const std::string &keyStr);
	bool tryReadString(std::string &result, const std::string &sectStr, const std::string &keyStr);
    std::string readString(const std::string &sectStr, const std::string &keyStr);
    std::string readStringDef(const std::string &sectStr, const std::string &keyStr, const std::string &def);
	int readInt32(const std::string &sectStr, const std::string &keyStr);
	int readInt32Def(const std::string &sectStr, const std::string &keyStr, const long long def);
	long long readInt64(const std::string &sectStr, const std::string &keyStr);
	long long readInt64Def(const std::string &sectStr, const std::string &keyStr, const long long def);
	double readDouble(const std::string &sectStr, const std::string &keyStr);
	double readDoubleDef(const std::string &sectStr, const std::string &keyStr, const double def);
	bool readBool(const std::string &sectStr, const std::string &keyStr);
	bool readBoolDef(const std::string &sectStr, const std::string &keyStr, const bool def);
	void writeString(const std::string &sectStr, const std::string &keyStr, const std::string &valueStr);
	void writeLong(const std::string &sectStr, const std::string &keyStr, const long long value);
	void writeDouble(const std::string &sectStr, const std::string &keyStr, const double value);
	void writeBool(const std::string &sectStr, const std::string &keyStr, const bool value);
};

