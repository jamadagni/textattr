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

#ifndef TEXTATTR_HPP
#define TEXTATTR_HPP

#include <stdexcept>
#include <iostream>

// classes

class TextAttrError : public std::invalid_argument
{
public:
    TextAttrError(const char * msg) : std::invalid_argument(msg) {}
};

class tastream
{
public:
    tastream(std::ostream & os) : _os(os) {}
    template<typename T> tastream & operator << (T val) { _os << val; return *this; }
    tastream & operator<<(std::ostream & (*manipulator)(std::ostream &)) { manipulator(_os); return *this; }
private:
    std::ostream & _os;
};

template<> tastream & tastream::operator<<(const char * s);
template<> tastream & tastream::operator<<(std::string s);

extern tastream ta_cout, ta_cerr;

// variable

extern bool taDisabled;
//extern const char * taErrorMsg;

// functions

const char * _ta_n_cpp(const char * specString, int specStringLen);
#define ta_n _ta_n_cpp
#define ta(SPEC_STRING) _ta_n_cpp(SPEC_STRING, -1)

#endif // TEXTATTR_HPP
