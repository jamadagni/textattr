// Hello! This is a simple example file for textattr.
// All you need to do to use textattr is this line at the beginning:
#include "textattr.hpp"
// ... and include -lta++ (or textattr.cpp directly) in your compiler command.

// The following #includes aren't vital for textattr:
#include <iostream>
#include <string>

int main()
{
    // You can get the escape code for a set of attributes using ta() and save it:
    std::string taYellowOnBlue = ta("y /b");

    // You can then input it to cout or simply inline calls to ta():
    std::cout << taYellowOnBlue << "Hello!" << ta("f") << std::endl;

    // It may be easier to simply use ta_cout which is like cout
    // but treats input beginning with @ as "at"tributes:
    ta_cout << "@g i u" << "text" << "@w" << "attr" << "@f" << " tries to be helpful by providing different error messages:\n";

    // Note that you can use pre-computed codes like taYellowOnBlue above with ta_cout as well.
    // They will just be output as-is (as they do not begin with @) and give the same effect.

    // TextAttrError-s will be thrown if input attribute specs are faulty:
    const char * badSpecs [] = {"", "c /r m /g y /b k /l c /r m /g y /b k /l", "longerThanAnyValidSpec", "q", "^610", "%ghijkl", "a50"};
    for (int i = 0; i < 7; ++i)
    {
        try { ta(badSpecs[i]); }
        catch (TextAttrError & e) { std::cout << e.what() << std::endl; }
    }
}

// That concludes this simple example!
// As you can see, ta_cout should suffice for most use-cases.
// For an exhaustive exploration of more textattr nuances please see demo.cpp.
