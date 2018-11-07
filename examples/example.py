#! /usr/bin/env python3

# Hello! This is a simple example file for textattr.
# All you need to do to use textattr is this line at the beginning:
from textattr import *

# You can get the escape code for a set of attributes using ta() and save it:
taYellowOnBlue = ta("y /b")

# You can then input it to print() and such or simply inline calls to ta():
print(taYellowOnBlue, "Hello!", ta("off"), sep = "")
# Note that you need to use stdout.write, or sep="" with print(), or {} with str.format
# to ensure no spaces separating the invisible code sequences and the text they apply to.

# It may be easier to simply use tawrite which is like stdout.write
# but treats input beginning with @ as "at"tributes:
tawrite("@g i u", "text", "@w", "attr", "@off", " tries to be helpful by providing different error messages:\n")
# For user convenience tawrite also runs str() on non-str input.

# Note that you can use pre-computed codes like taYellowOnBlue above with tawrite as well.
# They will just be output as-is (as they do not begin with @) and give the same effect.

# TextAttrError-s will be thrown if input attribute specs are faulty:
badSpecs = ("", "c /r m /g y /b k /l c /r m /g y /b k /l", "longerThanAnyValidSpec", "q", "^610", "%ghijkl", "a50")
for badSpec in badSpecs:
    try:
        ta(badSpec)
    except TextAttrError as e:
        print(e)

# That concludes this simple example!
# As you can see, tawrite() should suffice for most use-cases.
# For an exhaustive exploration of more textattr nuances please see demo.py.
