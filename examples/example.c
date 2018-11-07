// Hello! This is a simple example file for textattr.
// All you need to do to use textattr is this line at the beginning:
#include "textattr.h"
// ... and include -lta (or textattr.c directly) in your compiler command.

// The following #includes aren't vital for textattr:
#include <stdio.h>
#include <string.h>

int main()
{
    // You can get the escape code for a set of attributes using ta() and save it:
    char taYellowOnBlue[15];
    strcpy(taYellowOnBlue, ta("y /b"));

    // You can then input it to printf() and such or simply inline calls to ta():
    printf("%sHello!%s\n", taYellowOnBlue, ta("f"));
    // WARNING: You can have up to 40 inline calls to ta() in a single printf()
    // after which the internal code buffers will be reused.

    // It may be easier to simply use tawrite which serially prints a variable number of input strings
    // but treats input beginning with @ as "at"tributes:
    tawrite("@g i u", "text", "@w", "attr", "@f", " tries to be helpful by providing different error messages:\n");

    // Note that you can use pre-computed codes like taYellowOnBlue above with tawrite as well.
    // They will just be output as-is (as they do not begin with @) and give the same effect.

    // An error message will be written to taErrorMsg if input attribute specs are faulty:
    const char * badSpecs [] = {"", "c /r m /g y /b k /l c /r m /g y /b k /l", "longerThanAnyValidSpec", "q", "^610", "%ghijkl", "a50"};
    for (int i = 0; i < 7; ++i)
    {
        ta(badSpecs[i]);
        tawrite(taErrorMsg, "\n");
        // Note above that tawrite() can be useful to serially output multiple strings
        // even when no textattr functionality is needed.
    }
}

// That concludes this simple example!
// As you can see, tawrite() should suffice for most use-cases.
// For an exhaustive exploration of more textattr nuances please see demo.c.
