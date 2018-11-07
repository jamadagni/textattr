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

import std.conv, std.exception, std.format, std.stdio;

private
{

alias constString = const(char)[];

enum negateAttr = true, bgColor = true, fixedColor = true;

// main spec to code functions

string getAttr(constString spec, bool negate = false)
{
    static immutable string [] attrAbbr = ["o",    "t",     "i",      "u",          "x",        "e",         "v",        "h",      "z"        ];
    static immutable string [] attrFull = ["bold", "faint", "italic", "underlined", "blinking", "overlined", "reversed", "hidden", "struckout"];
    static immutable ubyte  [] attrCode = [1,      2,       3,        4,            5,          6,           7,          8,        9          ];
    foreach (i, code; attrCode)
        if (spec == attrAbbr[i] || spec == attrFull[i])
            return text(negate ? (code + 20) : code);
    throw new TextAttrException(""); // message will be added at calling site
}

string getColorByName(constString spec, bool bkgd, bool fixed = false)
{
    static immutable string [] colorFull = ["black", "dark-gray", "light-gray", "white",   "red", "green", "blue", "cyan", "magenta",   "light-red", "light-green", "light-blue", "light-cyan", "light-magenta",   "brown", "yellow",  "default"];
    static immutable string [] colorAbbr = ["k",     "d",         "l",          "w",       "r",   "g",     "b",    "c",    "m",         "+r",        "+g",          "+b",         "+c",         "+m",              "n",     "y",       "_"      ];
    static immutable ubyte  [] colorCode = [30,      90,          37,           97,        31,    32,      34,     36,     35,          91,          92,            94,           96,           95,                33,      93,        39       ];
    static immutable string [] colorFRgb = ["000",   "111",       "333",        "555",     "300", "030",   "003",  "033",  "303",       "511",       "151",         "115",        "155",        "515",             "310",   "551",     null     ];
    foreach (i, code; colorCode)
        if (spec == colorAbbr[i] || spec == colorFull[i])
        {
            if (fixed && i != colorCode.length - 1)
                return getColorByRgbLimited(colorFRgb[i], bkgd);
            else
                return text(bkgd ? (code + 10) : code);
        }
    throw new TextAttrException(""); // message will be added at calling site
}

string getColorByRgbLimited(constString spec, bool bkgd)
{
    int code;
    if (spec.length != 3 || collectException(to!int(spec, 6), code))
        throw new TextAttrException("specifying a color as ‘^rgb’ should be done by three digits in the range 0 to 5; found: ‘%s’".format(spec));
    return (bkgd ? "48;5;%d" : "38;5;%d").format(16 + code);
    // NOTE: here, if v is a digit in the input ^rgb, then the actual component value on a scale of 0 to 255 is:
    //       0, if v is 0;  95 + 40 * (v - 1), otherwise
}

string getColorByRgbTrue(constString spec, bool bkgd)
{
    int rgb, r, g, b;
    if (spec.length != 6 || collectException(to!int(spec, 16), rgb))
        throw new TextAttrException("specifying a color as ‘%%rrggbb’ should be done by six hexadecimal digits; found: ‘%s’".format(spec));
    b = rgb % 256; rgb /= 256; // integer division
    g = rgb % 256; r = rgb / 256;
    return "%d;2;%d;%d;%d".format(bkgd ? 48 : 38, r, g, b);
}

string getColorByGray(constString spec, bool bkgd)
{
    int code;
    if (collectException(to!int(spec), code) || code < 1 || code > 24)
        throw new TextAttrException("specifying a grayscale color as ‘a#’ should be done by integers 1 to 24; found: ‘%s’".format(spec));
    return (bkgd ? "48;5;%d" : "38;5;%d").format(231 + code);
    // NOTE: here, if v is the input value, then the actual RGB component value on a scale of 0 to 255 is:
    //       8 + (v - 1) * 10
}

string getColor(constString spec, bool bkgd = false)
{
    if (spec[0] == '^')
        return getColorByRgbLimited(spec[1 .. $], bkgd);
    else if (spec[0] == '%')
        return getColorByRgbTrue(spec[1 .. $], bkgd);
    else if (spec[0] == 'a')
        return getColorByGray(spec[1 .. $], bkgd);
    else if (spec.length > 4 && spec[0 .. 4] == "gray")
        return getColorByGray(spec[4 .. $], bkgd);
    else if (spec[$ - 1] == '!')
        return getColorByName(spec[0 .. $ - 1], bkgd, fixedColor);
    else
        return getColorByName(spec, bkgd);
}

} // private

// publicly visible symbols

class TextAttrException: Exception
{
    this(string msg, string file = __FILE__, size_t line = __LINE__)
        { super(msg, file, line); }
}

bool taDisabled = false;

string ta(constString specString)
{
    if (!__ctfe && taDisabled)
        return "";  // NOTE: not checking for errors
    if (specString == "f")
        return "\033[0m";  // frequent use

    int specCount = 0;
    string codes = "", code;
    import std.algorithm: splitter;
    foreach (spec; splitter(specString, ' '))
    {
        if (specCount == 11)  // no more allowed
            throw new TextAttrException("more than 11 tokens found in spec string: ‘%s’".format(specString));

        if (spec.length == 0)
            continue;  // since splitter reads an empty token between two consecutive spaces
        if (spec.length > 17)
            throw new TextAttrException("spec of illegal length: ‘%s’".format(spec));

        if (spec == "f" || spec == "off")
            code = "0";
        else if (spec[0] == '/')
            code = getColor(spec[1 .. $], bgColor);
        else if (spec[0] == '-')
            code = getAttr(spec[1 .. $], negateAttr);
        else if (spec.length > 4 && spec[0 .. 4] == "not-")
            code = getAttr(spec[4 .. $], negateAttr);
        else
        {
            try code = getColor(spec);
            catch (TextAttrException e) // not color
            {
                if (e.msg != "") // message for specific color type
                    throw e;
                try code = getAttr(spec);
                catch (TextAttrException) // not attr
                    throw new TextAttrException("unrecognized color or attribute name: ‘%s’".format(spec));
            }
        }

        if (codes != "") codes ~= ";";
        codes ~= code;
        ++specCount;
    }
    if (specCount == 0)
        throw new TextAttrException("no specs were input");
    else
        return "\033[" ~ codes ~ "m";
}

void tawrite(T ...)(File ofile, T args)
{
    foreach (arg; args)
        static if (is(typeof(arg) == string))
        {
            if (arg.length > 1 && arg[0] == '@')
                ofile.write(ta(arg[1 .. $]));
            else
                ofile.write(arg);
        }
        else
            ofile.write(arg);
}

void tawrite(T ...)(T args) if (!is(T[0]: File))
{
    stdout.tawrite(args);
}
