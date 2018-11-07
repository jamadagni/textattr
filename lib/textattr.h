// textattr (ta)
// =============
//
// Makes adding color and attributes to beautify the terminal output of your
// program easier by translating human-readable specs into ANSI escape codes
//
// Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
//
// Use, modification and distribution are permitted subject to the
// "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

#ifndef TEXTATTR_H
#define TEXTATTR_H

#include <stdio.h>
#include <stdbool.h>

// functions

// next two lines needed because internal function cannot be named as ta_n
// due to clash with and use by C++ ta_n which throws
const char * _ta_n(const char * specString, int specStringLen);
#define ta_n _ta_n
#define ta(SPEC_STRING) _ta_n(SPEC_STRING, -1)

#define tawrite(...)        _tafwrite(stdout, __VA_ARGS__, NULL)
#define tafwrite(FILE, ...) _tafwrite(FILE,   __VA_ARGS__, NULL)
void _tafwrite(FILE * ofile, ...);

// variables

extern bool taDisabled;
extern const char * taErrorMsg;
extern FILE * taStderr;

#endif // TEXTATTR_H
