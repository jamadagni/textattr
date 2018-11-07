// textattr demo
// =============
//
// Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
//
// Use, modification and distribution are permitted subject to the
// "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

import textattr;
import std.stdio;
import std.meta;

// repeatedly used escape codes
string taWhite, taWhiteOnBlue, taBrightCyan, taOff;

char[20] tempFileName; // only valid till next invocation of tempFileOpen
File tempFile;
void tempFileOpen()
{
    tempFileName = "/tmp/XXXXXX";
    import core.sys.posix.stdlib: mkstemp;
    tempFile.fdopen(mkstemp(tempFileName.ptr), "w");
}

void use(string symbol)
{
    writeln(taWhite, "Using ", taWhiteOnBlue, symbol, taOff, ":");
}

void helper()
{
    writeln("This is normal text.");

    // ta demo
    use("ta");
    writeln(ta("^510"), "Terminal colors ", ta("w"), "made easy", ta("+g"), " for all!!!", ta("f"));

    // tawrite demo
    alias tawriteInput = AliasSeq!("@+c", "Terminal colors ", "@+m", "made easy", "@y", " for all!!!", "@f", '\n',
                                   "Now we are back to default text.\n",
                                   "@w i", "string", "@f", "-s starting with ", "@w", "@", "@f", " are treated as specs (but not a lone @).\n",
                                   "Other types for which ", "@w i", "toString()", "@f", " is defined are also supported: ", "@n", 1, "@c", 'c', "@w", true, "@f", '\n');
    use("tawrite");
    tawrite(tawriteInput);

    // tawrite file overload demo
    use("tawrite overload to print to file");
    tempFileOpen();
    tempFile.tawrite(tawriteInput);
    tempFile.close();
    writeln("Printed text to ", taWhite, tempFileName, taOff, "; please check it.");

    writeln();
}

void printSeparator()
{
    write("~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~\n\n");
}

void main()
{
    taWhite = ta("w");
    taWhiteOnBlue = ta("w /b");
    taBrightCyan = ta("+c");
    taOff = ta("f");

    string taOfficialColors = ta("g") ~ "text" ~ taWhite ~ "attr" ~ taOff;
    string errorMessageLiteral = ta("+r") ~ "error message" ~ taOff;

    writeln();
    writeln("taOfficialColors length: ", taWhite, taOfficialColors.length, taOff);
    writeln("errorMessageLiteral length: ", taWhite, errorMessageLiteral.length, taOff);
    writeln();

    printSeparator();
    helper();

    // taDisabled demo

    use("taDisabled");

    write("Now disabling ", taOfficialColors, ": ");
    taDisabled = true;
    alias done = AliasSeq!("@y", "DONE!", "@f", "\n\n");
    tawrite(done);
    helper();

    write("Now re-enabling ", taOfficialColors, ": ");
    taDisabled = false;
    tawrite(done);

    printSeparator();

    // TextAttrException demo

    use("TextAttrException");
    writeln();

    tawrite("If you input an illegal spec to ", taWhiteOnBlue, "ta", taOff, ": ", taBrightCyan, "QXQX", taOff, '\n');
    try
        writeln(ta("QXQX"), "Some text");
    catch (TextAttrException e)
    {
        writeln("... the calling statement is never executed");
        writeln("and we caught a ", taWhiteOnBlue, "TextAttrException", taOff, " with the ", errorMessageLiteral, ":");
        writeln('{', e.msg, '}');
    }
    writeln();

    tawrite("But in the case of ", taWhiteOnBlue, "tawrite", taOff, ":\n");
    try
        tawrite("normal text preceding illegal spec ", taBrightCyan, "XQXQ", taOff, "...", "@XQXQ", "text with spec XQXQ");
    catch (TextAttrException e)
    {
        writeln("\n... will be output and we caught a ", taWhiteOnBlue, "TextAttrException", taOff, " with the ", errorMessageLiteral, ":");
        writeln('{', e.msg, '}');
    }
    writeln();

    printSeparator();

    // effect of taDisabled on exceptions

    tawrite("Note that if ", taOfficialColors, " is disabled via ", taWhiteOnBlue, "taDisabled", taOff, ",\n");
    writeln("illegal specs will not cause exceptions.");
    writeln();
    taDisabled = true;

    tawrite("For example, when spec ", taBrightCyan, "QQQ", taOff, " is input to ", taWhiteOnBlue, "ta", taOff, " for:\n");
    writeln(ta("QQQ"), "this text");
    writeln("... nothing happens.");
    writeln();

    tawrite("Likewise, when spec ", taBrightCyan, "XXX", taOff, " is input to ", taWhiteOnBlue, "tawrite", taOff, " for:\n");
    tawrite("@XXX", "this text\n");
    writeln("... no exception is thrown!");
    writeln();

    printSeparator();

    // end

    writeln("That concludes this demo!");
    writeln("Don't forget to read the help which you can see by running ", taWhite, "ta-help", taOff, ".");
    writeln();
}
