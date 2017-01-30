//this file is public domain
#include "../IniParser.h"
#include "../IniException.h"

int main()
{
	try {
		IniParser ini("sample.ini", true);
		printf("%d\n", ini.sectionExists("sect2"));
		printf("%d\n", ini.sectionExists("sect22"));
		printf("%d\n", ini.keyExists("sect22", "abc"));
		printf("%d\n", ini.keyExists("sect2", "abc"));
		printf("%d\n", ini.keyExists("sect2", "key2"));
		vector<string> sectNames = ini.readSections();
		vector<string> keyNames = ini.readSection("sect2");
		printf("%s\n", ini.readString("sect2", "key2").c_str());
		printf("%lld\n", ini.readInt("sect2", "key2"));		
		ini.writeString("sect2", "key2", "22");
		ini.writeString("sect2", "abc", "valabc");
		ini.writeString("sect22", "abc", "22valabc");
		printf("%s\n", ini.readStringDef("sect2", "non existed", "def").c_str());
		printf("%s\n", ini.readString("sect2", "non existed").c_str());
	}
	catch (const IniException & e)
	{
		printf("Exception: %s\n", e.what());
		return -1;
	}
    return 0;
}

