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

#include "textattr.hpp"

tastream ta_cout(std::cout);
tastream ta_cerr(std::cerr);

template<> tastream & tastream::operator<<(const char * s)
{
    _os << ((s[0] == '@' && s[1] != '\0') ? ta(s + 1) : s);
    return *this;
}

template<> tastream & tastream::operator<<(std::string s)
{
    return *this << s.c_str(); // delegate to preceding function
}

#define TA_CPP
#include "textattr.c"

const char * _ta_n_cpp(const char * specString, int specStringLen)
{
    const char * codeSeq = _ta_n(specString, specStringLen);
    if (taErrorMsg && taErrorMsg[0])
        throw TextAttrError(taErrorMsg);
    return codeSeq;
}
