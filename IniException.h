#pragma once
#include <stdlib.h>
#include <string.h>

using namespace std;

class IniException
{
private:
	string message;
    int n;
public:
	IniException(const string message, int n = 0) throw() : message(message), n(n) {}
    virtual ~IniException() throw() {}
    // Returns a C-style character string describing the general cause
    //  of the current error.
    virtual const char* what() const throw() { return message.c_str(); }
};
