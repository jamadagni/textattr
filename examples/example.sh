#! /bin/sh

# Hello! This is a simple example file for textattr.
# All you need to do to use textattr is to have the ta/tawrite executables in your path.

# You can get the escape code for a set of attributes using $(ta ) and save it
# to avoid multiple process forks:
TA_YELLOW_ON_BLUE=$(ta y /b)

# You can then input it to echo and such, or simply inline calls to $(ta ):
echo $TA_YELLOW_ON_BLUE"Hello!"$(ta off)

# There is also the tawrite command which serially prints its arguments
# but treats arguments beginning with @ as "at"tributes:
tawrite "@g i u" text @w attr @f " tries to be helpful by providing different error messages:" "\n"

# Unlike echo, tawrite does not output a trailing newline or spaces between arguments. So:
# 1. A quoted "\n" is to be provided as a separate argument for a newline wherever required.
# 2. Spaces should be included as part of a quoted string if they are needed in the output.

# Note also that you can use pre-computed codes like $TA_YELLOW_ON_BLUE above with tawrite as well.
# They will just be output as-is (as they do not begin with @) and give the same effect.

# An error message will be written to stderr if input attribute specs are faulty:
for BAD_SPEC in "" "c /r m /g y /b k /l c /r m /g y /b k /l" "longerThanAnyValidSpec" "q" "^610" "%ghijkl" "a50"
do
    ta $BAD_SPEC
done

# That concludes this simple example!
# Due to tawrite's requirements of quoting and other idiosyncracies, using command substitution to get the codes
# either directly within an echo command or via variables is probably the easiest approach to using textattr.
# For an exhaustive exploration of more textattr nuances please see demo.sh.
