// textattr demo
// =============
//
// Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
//
// Use, modification and distribution are permitted subject to the
// "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

#include "textattr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for mkstemp

#define NEWL putchar('\n')

// repeatedly used escape codes
// NOTE: be sure to allocate enough space
char taWhite[10], taWhiteOnBlue[15], taBrightCyan[10], taOff[10];

char tempFileName[20]; // name only valid till next invocation of tempFileOpen
FILE * tempFile;
void tempFileOpen()
{
    strcpy(tempFileName, "/tmp/XXXXXX");
    tempFile = fdopen(mkstemp(tempFileName), "w");
}

void use(const char * symbol)
{
    tawrite(taWhite, "Using ", taWhiteOnBlue, symbol, taOff, ": ", "\n");
}

void helper()
{
    printf("This is normal text.\n");

    // ta demo
    use("ta");
    printf("%sTerminal colors %smade easy%s for all!!!%s\n", ta("^510"), ta("w"), ta("+g"), ta("f"));

    // ta_n demo
    const char * ta_n_input = "k /lBlack on light gray";
    use("ta_n");
    printf("%s%s%s\n", ta_n(ta_n_input, 4), ta_n_input + 4, ta("f"));

    // tawrite demo
    #define TAWRITE_INPUT "@+c", "Terminal colors ", "@+m", "made easy", "@y", " for all!!!", "@f", "\n" \
                          "Now we are back to default text.\n", \
                          "@y /r", "CAREFUL", "@f", ": Only null-terminated strings should be passed to ", "@w /b", "tawrite", "@f", "!\n" \
                          "Those starting with ", "@w", "@", "@f", " are treated as specs (but not a lone @).\n"
    use("tawrite");
    tawrite(TAWRITE_INPUT);

    // tafwrite demo
    use("tafwrite");
    tempFileOpen();
    tafwrite(tempFile, TAWRITE_INPUT);
    fclose(tempFile);
    tawrite("Printed text to ", taWhite, tempFileName, taOff, "; please check it.\n");

    NEWL;
}

void printSeparator()
{
    printf("~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~\n\n");
}

int main()
{
    strcpy(taWhite, ta("w"));
    strcpy(taWhiteOnBlue, ta("w /b"));
    strcpy(taBrightCyan, ta("+c"));
    strcpy(taOff, ta("f"));

    char taOfficialColors[25];
    char errorMessageLiteral[30];

    sprintf(taOfficialColors, "%stext%sattr%s", ta("g"), taWhite, taOff); // be sure to allocate enough space
    sprintf(errorMessageLiteral, "%serror message%s", ta("+r"), taOff); // be sure to allocate enough space

    NEWL;
    printf("taOfficialColors length: %s%lu%s\n", taWhite, strlen(taOfficialColors), taOff);
    printf("errorMessageLiteral length: %s%lu%s\n", taWhite, strlen(errorMessageLiteral), taOff);
    NEWL;

    printSeparator();
    helper();

    // taDisabled demo

    use("taDisabled");

    tawrite("Now disabling ", taOfficialColors, ": ");
    taDisabled = true;
    #define DONE "@y", "DONE!", "@f", "\n\n"
    tawrite(DONE);
    helper();

    tawrite("Now re-enabling ", taOfficialColors, ": ");
    taDisabled = false;
    tawrite(DONE);

    printSeparator();

    // taErrorMsg demo

    #define PRINT_ERR_MSG printf("{%s}\n", taErrorMsg)
    use("taErrorMsg");
    NEWL;

    tawrite("If you input an illegal spec via ", taWhiteOnBlue, "ta", taOff, ": ", taBrightCyan, "QXQX", taOff, "\n");
    printf("%sIt will not affect subsequent output...\n", ta("QXQX"));
    tawrite("but we got the ", errorMessageLiteral, ":\n");
    PRINT_ERR_MSG;
    NEWL;

    tawrite("If you input a legal spec string instead: ", taBrightCyan, "w underlined /g!", taOff, "\n");
    printf("%sApart from getting the desired output...%s\n", ta("w underlined /g!"), taOff);
    tawrite("any previous ", errorMessageLiteral, " will be cleared:\n");
    PRINT_ERR_MSG;
    tawrite("So don't call ", taOfficialColors, " again before you copy/print the ", errorMessageLiteral, " if needed.\n");
    NEWL;

    tawrite("But in the case of ", taWhiteOnBlue, "tawrite", taOff, ", in outputting a sequence such as:\n");
    tawrite("normal text, ", "@QXQ", "text with spec QXQ, ", "@g", "green text, ", taOff, "@XQX", "text with spec XQX\n");
    tawrite("... only the last ", errorMessageLiteral, " will be available:\n");
    PRINT_ERR_MSG;
    tawrite("... and that too only if not cleared by ", "@+m", "another call with a valid spec string", taOff, ":\n");
    PRINT_ERR_MSG;
    NEWL;

    printSeparator();

    // taStderr demo

    use("taStderr");
    NEWL;

    tawrite("taStderr is null by default. Now setting it to ", taWhite, "stderr", taOff, ".\n");
    taStderr = stderr;
    tawrite("Trying to input an illegal spec to ", taOfficialColors, ": ", taBrightCyan, "XQXQ", taOff, "\n");
    printf("%sIt will not affect subsequent output...\n", ta("XQXQ"));
    tawrite("... but the ", errorMessageLiteral, " should have been printed above\n");
    tawrite("and is also available via ", taWhiteOnBlue, "taErrorMsg", taOff, " as before:\n");
    PRINT_ERR_MSG;
    NEWL;

    tempFileOpen();
    printf("Now setting taStderr to %s%s%s.\n", taWhite, tempFileName, taOff);
    taStderr = tempFile;
    tawrite("Trying to input an illegal spec to ", taOfficialColors, ": ", taBrightCyan, "QXXQ", taOff, "\n");
    tawrite("@QXXQ", "This won't look any different...\n");
    tawrite("... but the ", errorMessageLiteral, " will be printed to that file\n");
    printf("but without colors since it is not a tty.\n");
    tawrite("It is of course also available via ", taWhiteOnBlue, "taErrorMsg", taOff, ":\n");
    PRINT_ERR_MSG;
    NEWL;
    fclose(tempFile);
    taStderr = NULL;

    printSeparator();

    // effect of taDisabled on error messages

    tawrite("Note that if ", taOfficialColors, " is disabled via ", taWhiteOnBlue, "taDisabled", taOff, ",\n");
    tawrite("you will not receive any ", errorMessageLiteral, " for illegal specs.\n");
    tawrite("But any earlier ", errorMessageLiteral, " is always cleared.\n.\n");
    NEWL;
    taDisabled = true;

    tawrite("For example, when spec ", taBrightCyan, "QQQ", taOff, " is input to ", taWhiteOnBlue, "ta", taOff, " for:\n");
    printf("%s", ta("QQQ"));
    tawrite("this text\n... ", taWhiteOnBlue, "taErrorMsg", taOff, " is empty: ");
    PRINT_ERR_MSG;
    tawrite("and does not contain the ", errorMessageLiteral, " received for ", taBrightCyan, "QXXQ", taOff, ".\n");
    NEWL;
    tawrite("Likewise, when spec ", taBrightCyan, "XXX", taOff, " is input to ", taWhiteOnBlue, "tawrite", taOff, " for:\n");
    tawrite("@XXX", "this text\n... no ", errorMessageLiteral, " is produced: ");
    PRINT_ERR_MSG;
    NEWL;

    printSeparator();

    // end

    printf("That concludes this demo!\n");
    printf("Don't forget to read the help which you can see by running %sta-help%s.\n", taWhite, taOff);
    NEWL;
}
