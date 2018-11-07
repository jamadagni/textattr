// textattr demo
// =============
//
// Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
//
// Use, modification and distribution are permitted subject to the
// "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

#include "textattr.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> // only needed for tempFileOpen()
#include <cstdlib> // only needed for tempFileOpen()
using namespace std;

#define NEWL putchar('\n')

// repeatedly used escape codes
string taWhite, taWhiteOnBlue, taBrightCyan, taOff;

char tempFileName[20]; // name only valid till next invocation of tempFileOpen
ofstream tempFile;
void tempFileOpen()
{
    strcpy(tempFileName, "/tmp/XXXXXX");
    mkstemp(tempFileName);
    tempFile.open(tempFileName);
}

void use(const char * symbol)
{
    cout << taWhite << "Using " << taWhiteOnBlue << symbol << taOff << ":" << endl;
}

void helper()
{
    cout << "This is normal text.\n";

    // ta demo
    use("ta");
    cout << ta("^510") << "Terminal colors " << ta("w") << "made easy" << ta("+g") << " for all!!!" << ta("f") << '\n';

    // ta_cout demo
    #define TASTREAM_INPUT "@+c" << "Terminal colors " << "@+m" << "made easy" << "@y" << " for all!!!" << "@f" << "\n" \
                           "Now we are back to default text.\n" << \
                           "C-style strings and C++ " << string("@w i") << "std::string" << "@f" << "-s starting with " << \
                           "@w" << "@" << "@f" << " are treated as specs (but not a lone @).\n" \
                           "Other types for which a global " << "@w i" << "operator<<(ostream&,T)" << "@f" << " is defined are also supported: " << \
                           "@n" << 1 << "@c" << 'c' << "@w" << boolalpha << true << "@f" << "\n"
    use("predefined tastream ta_cout");
    ta_cout << TASTREAM_INPUT;

    // file tastream demo
    use("tastream with an ofstream");
    tempFileOpen();
    tastream(tempFile) << TASTREAM_INPUT;
    tempFile.close();
    cout << "Printed text to " << taWhite << tempFileName << taOff << "; please check it.\n";

    // string tastream demo
    use("tastream with an ostringstream");
    ostringstream os;
    tastream tas(os);
    tas << "@^200 /l" << "Terminal colors " << "@^020" << "made easy" << "@^002" << " for all!!!" << "@f" << '\n';
    cout << os.str();

    NEWL;
}

void printSeparator()
{
    cout << "~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~\n\n";
}

int main()
{
    taWhite = ta("w");
    taWhiteOnBlue = ta("w /b");
    taBrightCyan = ta("+c");
    taOff = ta("f");

    string taOfficialColors = string(ta("g")) + "text" + taWhite + "attr" + taOff;
    string errorMessageLiteral = string(ta("+r")) + "error message" + taOff;

    NEWL;
    cout << "taOfficialColors length: " << taWhite << taOfficialColors.length() << taOff << '\n';
    cout << "errorMessageLiteral length: " << taWhite << errorMessageLiteral.length() << taOff << '\n';
    NEWL;

    printSeparator();
    helper();

    // taDisabled demo

    use("taDisabled");

    cout << "Now disabling " << taOfficialColors << ": ";
    taDisabled = true;
    #define DONE "@y" << "DONE!" << "@f" << "\n\n";
    ta_cout << DONE;
    helper();

    cout << "Now re-enabling " << taOfficialColors << ": ";
    taDisabled = false;
    ta_cout << DONE;

    printSeparator();

    // TextAttrError demo

    use("TextAttrError");
    NEWL;

    cout << "If you input an illegal spec to " << taWhiteOnBlue << "ta" << taOff << ": " << taBrightCyan << "QXQX" << taOff << '\n';
    try
    {
        cout << ta("QXQX") << "Some text" << '\n';
    }
    catch (TextAttrError e)
    {
        cout << "... the calling statement is never executed" << '\n';
        cout << "and we caught a " << taWhiteOnBlue << "TextAttrError" << taOff << " with the " << errorMessageLiteral << ":" << '\n';
        cout << '{' << e.what() << '}' << '\n';
    }
    NEWL;

    cout << "But in the case of a " << taWhiteOnBlue << "tastream" << taOff << ":\n";
    try
    {
        ta_cout << "normal text preceding illegal spec " << taBrightCyan << "XQXQ" << taOff << "..." << "@XQXQ" << "text with spec XQXQ";
    }
    catch (TextAttrError e)
    {
        cout << "\n... will be output and we caught a " << taWhiteOnBlue << "TextAttrError" << taOff << " with the " << errorMessageLiteral << ":\n"
             << '{' << e.what() << '}' << '\n';
    }
    NEWL;

    printSeparator();

    // effect of taDisabled on exceptions

    ta_cout << "Note that if " << taOfficialColors << " is disabled via " << taWhiteOnBlue << "taDisabled" << taOff << ",\n";
    cout << "illegal specs will not cause exceptions." << '\n';
    NEWL;
    taDisabled = true;

    ta_cout << "For example, when spec " << taBrightCyan << "QQQ" << taOff << " is input to " << taWhiteOnBlue << "ta" << taOff << " for:\n";
    cout << ta("QQQ") << "this text" << '\n';
    cout << "... nothing happens." << '\n';
    NEWL;

    ta_cout << "Likewise, when spec " << taBrightCyan << "XXX" << taOff << " is input to a " << taWhiteOnBlue << "tastream" << taOff << " for:\n";
    ta_cout << "@XXX" << "this text\n";
    cout << "... no exception is thrown!" << '\n';
    NEWL;

    printSeparator();

    // end

    cout << "That concludes this demo!" << '\n';
    cout << "Don't forget to read the help which you can see by running " << taWhite << "ta-help" << taOff << "." << '\n';
    NEWL;
}
