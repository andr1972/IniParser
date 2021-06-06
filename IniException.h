/* This file is part of https://github.com/borneq/IniParser
The MIT License (MIT), see file LICENSE */
#pragma once
#include <stdlib.h>
#include <string.h>

class IniException
{
private:
    std::string message;
    int n;
public:
	IniException(const std::string message, int n = 0) throw() : message(message), n(n) {}
    virtual ~IniException() throw() {}
    // Returns a C-style character string describing the general cause
    //  of the current error.
    virtual const char* what() const throw() { return message.c_str(); }
};
