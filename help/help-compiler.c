#include "textattr.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool areEqualN(const char * a, const char * b, int n) { return strncmp(a, b, n) == 0; }

int print(const char * s)
{
    const char * p;
    for (p = s; *p; ++p) { if (fputc(*p, stdout) == EOF) break; }
    return p - s;
}

int nprint(const char * s, int n)
{
    const char * p;
    for (p = s; *p && (p - s) < n; ++p) { if (fputc(*p, stdout) == EOF) break; }
    return p - s;
}

int main(int argc, char * argv [])
{
    if (argc != 2)
        exit(EXIT_FAILURE);
    FILE * usageFile = fopen(argv[1], "r");

    taStderr = stderr;
    const char W[10] = "\033[97m", F[10] = "\033[0m";
    size_t lineBufLen = 1024;
    char * lineBuf = malloc(lineBufLen);
    ssize_t lineLen = 0;
    while ((lineLen = getline(&lineBuf, &lineBufLen, usageFile)) > 0)
    {
        const char * curTextPos = lineBuf;
        while (true)
        {
            const char * specStart = strchr(curTextPos, '$');
            if (!specStart) break; // no more possibility of spec strings
            nprint(curTextPos, specStart - curTextPos); // certainly non-spec text
            curTextPos = specStart;
            if      (areEqualN(curTextPos, "${W}" , 4)) { print(W); curTextPos += 4; }
            else if (areEqualN(curTextPos, "${F}" , 4)) { print(F); curTextPos += 4; }
            else if (areEqualN(curTextPos, "$(ta ", 5))
            {
                const char * specEnd = strchr(specStart + 5, ')');
                if (!specEnd) break; // legal end of spec string not found
                print(ta_n(specStart + 5, specEnd - specStart - 5)); // code for spec
                curTextPos = specEnd + 1;
            }
            else
                break; // no valid spec string
        }
        print(curTextPos); // remaining text in line
    }
    free(lineBuf);
}
