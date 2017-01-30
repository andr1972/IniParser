//this file is public domain
#include "../IniParser.h"

int main()
{
	IniParser ini("sample.ini");
	printf("%d\n",ini.sectionExists("sect2"));
	printf("%d\n", ini.sectionExists("sect22"));
	printf("%d\n", ini.keyExists("sect22","abc"));
	printf("%d\n", ini.keyExists("sect2", "abc"));
	printf("%d\n", ini.keyExists("sect2", "key2"));
	vector<string> sectNames = ini.readSections();
	vector<string> keyNames = ini.readSection("sect2");
  	printf("%s\n", ini.readString("sect2", "key2", "def").c_str());
	printf("%lld\n", ini.readLong("sect2", "key2", 5));
	ini.writeString("sect2", "key2", "22");
	ini.writeString("sect2", "abc", "valabc");
	ini.writeString("sect22", "abc", "22valabc");
    return 0;
}

