// Hello! This is a simple example file for textattr.
// All you need to do to use textattr is this line at the beginning:
import textattr;
// ... and include textattr.d in your compiler command.

// The following import isn't vital for textattr:
import std.stdio;

void main()
{
    // You can get the escape code for a set of attributes using ta() and save it:
    string taYellowOnBlue = ta("y /b");

    // You can then input it to write() and family or simply inline calls to ta():
    writeln(taYellowOnBlue, "Hello!", ta("f"));

    // It may be easier to simply use tawrite which is like write
    // but treats input beginning with @ as "at"tributes:
    tawrite("@g i u", "text", "@w", "attr", "@f", " tries to be helpful by providing different error messages:\n");

    // Note that you can use pre-computed codes like taYellowOnBlue above with tawrite as well.
    // They will just be output as-is (as they do not begin with @) and give the same effect.

    // TextAttrExceptions-s will be thrown if input attribute specs are faulty:
    string [] badSpecs = ["", "c /r m /g y /b k /l c /r m /g y /b k /l", "longerThanAnyValidSpec", "q", "^610", "%ghijkl", "a50"];
    foreach (badSpec; badSpecs)
    {
        try { ta(badSpec); }
        catch (TextAttrException e) { writeln(e.msg); }
    }
}

// That concludes this simple example!
// As you can see, tawrite() should suffice for most use-cases.
// For an exhaustive exploration of more textattr nuances please see demo.d.
