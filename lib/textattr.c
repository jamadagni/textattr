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

#ifndef TA_CPP
#include "textattr.h"
#include <stdbool.h>
#endif

#include <stdio.h>
#include <stdlib.h>  // for strtol, getenv
#include <string.h>
#include <assert.h>
#include <ctype.h>   // for isspace
#include <unistd.h>  // for isatty

#ifndef TA_CPP
#include <stdarg.h>
#endif

// publicly visible variables

bool taDisabled = false;
const char * taErrorMsg = NULL;

#ifndef TA_CPP
FILE * taStderr = NULL;
#endif

// wrappers to standard C types/functions

/* NOTE: For cheap substring operations (without involving strcpy) such as to be
 * able to accept specs as parts of a null-terminated spec string (since such
 * substrings will not be null-terminated themselves), and since we need to
 * compute the length of the specs in most cases especially to avoid buffer
 * overflows, we define a length-aware string type for internal use only.
 */
typedef struct { const char * data; int len; } String;
static String string(const char * data, int len)
{
    String temp = {data, len};
    return temp;
}

static bool areEqual(String spec, const char * other)
{
    return strncmp(spec.data, other, spec.len) == 0 && other[spec.len] == '\0';
    /* NOTE: The == '\0' above ensures that `other` ends with spec.len. This
     * check is safe since due to short-circuit evaluation it will be done only
     * if spec.len number of non-null chars of `other` have been compared and
     * found identical to the same number of chars of `spec.data`, which is all
     * non-null, so `other`, being null-terminated, should have the next as '\0'
     */
}

static bool areEqualN(const char * a, const char * b, int n) { return strncmp(a, b, n) == 0; }

static bool checkedAtoi(String spec, int base, int * output)
{
    char * endOfValidSpecData;
    *output = strtol(spec.data, &endOfValidSpecData, base);
    return endOfValidSpecData == spec.data + spec.len; // successful conversion
}

// helpers for returning from main functions

static char errorMsg[128] = ""; // separate from taErrorMsg which doesn't allow write access
static String error = {errorMsg, 0};
static String writeErrorString(const char * fmt, String spec)
{
    assert(strstr(fmt, "%.*s")); // to correspond to usage below
    error.len = snprintf(errorMsg, 128, fmt, spec.len, spec.data);
        // error.data is const, hence using errorMsg above
        // also, error.len is never used; error is made a String just for return-type parity with the
        // functions that return a code; code.len is checked for buffer overflow in appendToCodeSeq
        // this is just defensive programming which may not really be necessary though...
    return error;
}

static char codeGlobalBuf[18] = "";
static String codeGlobal = {codeGlobalBuf, 0};

static String writeCodeString(const char * fmt, int code)
{
    codeGlobal.len = snprintf(codeGlobalBuf, 18, fmt, code); // output.data is const
    assert(codeGlobal.len < 18); // to check if it was null-terminated or not
    return codeGlobal;
}

static String writeTrueColorCodeString(int prefix, int r, int g, int b)
{
    codeGlobal.len = snprintf(codeGlobalBuf, 18, "%d;2;%d;%d;%d", prefix, r, g, b); // output.data is const
    assert(codeGlobal.len < 18); // to check if it was null-terminated or not
    return codeGlobal;
}

// main spec to code functions

static const bool NEGATE_ATTR = true, BG_COLOR = true, FIXED_COLOR = true;
static const bool NORMAL_ATTR = false, FG_COLOR = false, SCHEME_COLOR = false;

typedef unsigned char ubyte;

#define attrLen 9
static const char * attrAbbr[attrLen] = {"o",    "t",     "i",      "u",          "x",        "e",         "v",        "h",      "z"        };
static const char * attrFull[attrLen] = {"bold", "faint", "italic", "underlined", "blinking", "overlined", "reversed", "hidden", "struckout"};
static ubyte        attrCode[attrLen] = {1,      2,       3,        4,            5,          6,           7,          8,        9          };
static String getAttr(String spec, bool negate)
{
    for (int i = 0; i < attrLen; ++i)
        if (areEqual(spec, attrAbbr[i]) || areEqual(spec, attrFull[i]))
            return writeCodeString("%d", negate ? (attrCode[i] + 20) : attrCode[i]);
    return error;
        // unrecognized attribute
        // errorMsg would have been cleared; will be added at calling site
}

static String getColorByRgbLimited(String spec, bool bkgd); // decl

#define colorLen 17
static const char * colorFull[colorLen] = {"black", "dark-gray", "light-gray", "white",   "red", "green", "blue", "cyan", "magenta",   "light-red", "light-green", "light-blue", "light-cyan", "light-magenta",   "brown", "yellow",  "default"};
static const char * colorAbbr[colorLen] = {"k",     "d",         "l",          "w",       "r",   "g",     "b",    "c",    "m",         "+r",        "+g",          "+b",         "+c",         "+m",              "n",     "y",       "_"      };
static ubyte        colorCode[colorLen] = {30,      90,          37,           97,        31,    32,      34,     36,     35,          91,          92,            94,           96,           95,                33,      93,        39       };
static const char * colorRgbL[colorLen] = {"000",   "111",       "333",        "555",     "300", "030",   "003",  "033",  "303",       "511",       "151",         "115",        "155",        "515",             "310",   "551",     0        };
static String getColorByName(String spec, bool bkgd, bool fixed)
{
    for (int i = 0; i < colorLen; ++i)
        if (areEqual(spec, colorAbbr[i]) || areEqual(spec, colorFull[i]))
        {
            if (fixed && i != colorLen - 1)
                return getColorByRgbLimited(string(colorRgbL[i], 3), bkgd);
            else
                return writeCodeString("%d", bkgd ? (colorCode[i] + 10) : colorCode[i]);
        }
    return error;
        // unrecognized color
        // errorMsg would have been cleared; will be added at calling site
}

static String getColorByRgbLimited(String spec, bool bkgd)
{
    int code;
    if (spec.len != 3 || !checkedAtoi(spec, 6, &code))
        return writeErrorString("specifying a color as ‘^rgb’ should be done by three digits in the range 0 to 5; found: ‘%.*s’", spec);
    return writeCodeString(bkgd ? "48;5;%d" : "38;5;%d", 16 + code);
    // NOTE: here, if v is a digit in the input @rgb, then the actual component value on a scale of 0 to 255 is:
    //       0, if v is 0;  95 + 40 * (v - 1), otherwise
}

static String getColorByRgbTrue(String spec, bool bkgd)
{
    int rgb, r, g, b;
    if (spec.len != 6 || !checkedAtoi(spec, 16, &rgb))
        return writeErrorString("specifying a color as ‘%%rrggbb’ should be done by six hexadecimal digits; found: ‘%.*s’", spec);
    b = rgb % 256; rgb /= 256; // integer division
    g = rgb % 256; r = rgb / 256;
    return writeTrueColorCodeString(bkgd ? 48 : 38, r, g, b);
}

static String getColorByGray(String spec, bool bkgd)
{
    int code;
    if (!checkedAtoi(spec, 10, &code) || code < 1 || code > 24)
        return writeErrorString("specifying a grayscale color as ‘a#’ should be done by integers 1 to 24; found: ‘%.*s’", spec);
    return writeCodeString(bkgd ? "48;5;%d" : "38;5;%d", 231 + code);
    // NOTE: here, if v is the input value, then the actual RGB component value on a scale of 0 to 255 is:
    //       8 + (v - 1) * 10
}

static String getColor(String spec, bool bkgd)
{
    if (spec.data[0] == '^')
        return getColorByRgbLimited(string(spec.data + 1, spec.len - 1), bkgd);
    else if (spec.data[0] == '%')
        return getColorByRgbTrue(string(spec.data + 1, spec.len - 1), bkgd);
    else if (spec.data[0] == 'a')
        return getColorByGray(string(spec.data + 1, spec.len - 1), bkgd);
    else if (spec.len > 4 && areEqualN(spec.data, "gray", 4))
        return getColorByGray(string(spec.data + 4, spec.len - 4), bkgd);
    else if (spec.data[spec.len - 1] == '!')
        return getColorByName(string(spec.data, spec.len - 1), bkgd, FIXED_COLOR);
    else
        return getColorByName(spec, bkgd, SCHEME_COLOR);
}

// helpers to build code string

/*
Maximally long meaningful spec is all 9 attributes and 2 (fg/bg) colors. That
means 2 bytes for the \033[ prefix and 11 bytes for 11 suffixes. Maximum length
of an attribute is 2 bytes for cancelers. Same for a color is 16 bytes: 4 for
38;2 plus 3 * 4 = 12 for true color RGB. Thus we have a maximal meaningful
byte count of 2 + 11 + 9 * 2 + 2 * 16 = 63. With the final null we have 64 bytes
sufficient for a meaningful maximal code. We will support upto 40 consecutive
invocations of `ta` (such as from a single line of `printf`) after which the
earliest output codes will be overwritten.
*/
#define codeSeqCycBufCount 40
static char codeSeqCycBuf[codeSeqCycBufCount][64];
static char * codeSeqCurBuf;
static int codeSeqCurBufLen;
static int codeSeqCurIndex = 0;

static bool appendToCodeSeq(String code, char suffix)
{
    int newLen = codeSeqCurBufLen + code.len + 1; // 1 for suffix
    if (newLen > 47) return false; // won't overflow capacity; this is why the get* fns above return len-aware String
    strncpy(codeSeqCurBuf + codeSeqCurBufLen, code.data, code.len);
    codeSeqCurBuf[newLen - 1] = suffix;
    codeSeqCurBuf[newLen] = '\0'; // actually needed only if suffix == 'm' but still for safety...
    codeSeqCurBufLen = newLen;
    return true;
}

static const char * setAndPrintError(const char * msg)
{
    taErrorMsg = msg; // for reading from a calling program even if taStderr is null
#ifndef TA_CPP
    if (taStderr)
        fprintf(taStderr,
                isatty(fileno(taStderr)) ?
                    //  green             white             yellow               off
                    "\033[32m" "text" "\033[97m" "attr " "\033[93m" "error" "\033[0m" ": %s\n" :
                    "textattr error: %s\n",
                msg);
#endif
    return ""; // for convenience and brevity of calling code
}

static String specArray[11];
static int specCount;

static const char * getCodeSeq()
// uses globals above to avoid malloc, so no args
// will return the escape code to be sent to the terminal, so downgrading from String to const char *
{
    assert(0 < specCount && specCount < 12);

    codeSeqCurBuf = codeSeqCycBuf[codeSeqCurIndex];
    ++codeSeqCurIndex; // for next iteration
    codeSeqCurIndex %= codeSeqCycBufCount; // only so many buffers available
    strcpy(codeSeqCurBuf, "\033[");
    codeSeqCurBufLen = 2;

    String code;
    #define GOT_ERROR code.data == errorMsg

    // add individual spec codes to codeSeq
    for (int i = 0; i < specCount; ++i)
    {
        String spec = specArray[i];

        if (spec.len == 0)
            code.data = strcpy(errorMsg, "empty spec found"); // can happen (only) with malformed command-line input
        else if (spec.len > 17)
            code = writeErrorString("spec of illegal length: ‘%.*s’", spec);
        else if (areEqual(spec, "f") || areEqual(spec, "off"))
            code = string("0", 1);
        else if (spec.data[0] == '/')
            code = getColor(string(spec.data + 1, spec.len - 1), BG_COLOR);
        else if (spec.data[0] == '-')
            code = getAttr(string(spec.data + 1, spec.len - 1), NEGATE_ATTR);
        else if (spec.len > 4 && areEqualN(spec.data, "not-", 4))
            code = getAttr(string(spec.data + 4, spec.len - 4), NEGATE_ATTR);
        else
        {
            code = getColor(spec, FG_COLOR);
            if (GOT_ERROR) // not color
            {
                if (errorMsg[0] != '\0') // message for specific color type
                    break;
                code = getAttr(spec, NORMAL_ATTR);
                if (GOT_ERROR) // not attr
                    code = writeErrorString("unrecognized color or attribute name: ‘%.*s’", spec);
            }
        }
        if (GOT_ERROR) break;

        char suffix = (i != specCount - 1) ? ';' : 'm';
        bool appendSuccess = appendToCodeSeq(code, suffix);
        assert(appendSuccess);
    }

    return GOT_ERROR ? setAndPrintError(errorMsg) : codeSeqCurBuf;
}

// publicly visible functions

const char * _ta_n(const char * specString, int specStringLen)
{
    errorMsg[0] = '\0'; // clear error message always
    if (taDisabled)
        return ""; // NOTE: not checking for errors

    if (specString[0] == 'f' && (specStringLen == 1 || specString[1] == '\0'))
        return "\033[0m"; // frequent use

    // following is a hand-written version of strtok which doesn't need non-const char * and uses String;
    // only possible since we have a cap on the allowed number of tokens; else we would have to use strtok
    specCount = 0;
    String * curSpec = specArray; // first item
    bool insideSpec = false;
    for (const char * p = specString;
         // continue so long as we don't encounter null and are within specified length if any
         *p && ((specStringLen > 0) ?
                (p - specString) < specStringLen :
                true);
         ++p)
    {
        if (*p == ' ') // end of token
        {
            insideSpec = false;
            if (curSpec->len != 0)
                ++curSpec; // for next token
        }
        else
        {
            if (insideSpec)
                ++(curSpec->len);
            else // start of token
            {
                insideSpec = true;
                if (specCount == 11) // no more allowed
                {
                    #define MORE "more than 11 tokens found in spec string: "
                    if (specStringLen > 0)
                        writeErrorString(MORE "‘%.*s’", string(specString, specStringLen));
                    else
                        snprintf(errorMsg, 128, MORE "‘%s’", specString);
                    return setAndPrintError(errorMsg);
                }
                curSpec->data = p;
                curSpec->len = 1;
                ++specCount;
            }
        }
    }
    return (specCount == 0) ? setAndPrintError("no specs were input") : getCodeSeq();
}

#if !defined(TA_EXEC) && !defined(TA_CPP)
// when compiling the C library
void _tafwrite(FILE * ofile, ...)
{
    va_list args;
    va_start(args, ofile);
    const char * str; //, * codeSeq
    while (true)
    {
        str = va_arg(args, const char *);
        if (str == NULL) break;
        fputs((str[0] == '@' && str[1] != '\0') ? ta(str + 1) : str, ofile);
    }
    va_end(args);
}
#endif

#ifdef TA_EXEC
// when compiling the executable
int main(int argc, char * argv[])
{
    int progNameLen = strlen(argv[0]);
    bool codeRequired = progNameLen > 5 && areEqualN(argv[0] + progNameLen - 5, "-code", 5);

    taStderr = stderr;
    if (getenv("TA_DISABLED") && !codeRequired)
        taDisabled = true;

    if (progNameLen > 5 && areEqualN(argv[0] + progNameLen - 5, "write", 5))
    // invoked as tawrite
    {
        for (int i = 1; i < argc; ++i)
        {
            char * arg;
            arg = argv[i];
            if (areEqualN(arg, "\\n", 2)) // for convenience and parity with library functions
                putchar('\n');
            else
                fputs((arg[0] == '@' && arg[1] != '\0') ? ta(arg + 1) : arg, stdout);
        }
    }
    else
    // invoked as ta or ta-code
    if (!taDisabled)
    {
        if (argc < 2 || argc > 12) // one program name plus arguments
        {
            setAndPrintError("at least 1 argument and at most 11 arguments should be given");
            return EXIT_FAILURE;
        }

        // building spec String-s from arguments
        specCount = 0;
        String * curSpec = specArray; // first item
        for (int i = 1; i < argc; ++i)
        {
            curSpec->data = argv[i];
            curSpec->len = strlen(argv[i]);
            ++specCount;
            ++curSpec; // for next token
        }
        const char * codeSeq = getCodeSeq();
        if (codeSeq[0] == '\0') return EXIT_FAILURE; // error message would have been printed to stderr

        if (codeRequired)
            printf("\\033%s\n", codeSeq + 1); // except first i.e. esc char
        else
            fputs(codeSeq, stdout);
    }

    return EXIT_SUCCESS;
}
#endif // TA_EXEC
