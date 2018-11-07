#! /bin/bash

# textattr demo
# =============
#
# Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
#
# Use, modification and distribution are permitted subject to the
# "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

# repeatedly used escape codes
taWhite=$(ta w)
taWhiteOnBlue=$(ta w /b)
taBrightCyan=$(ta +c)
taOff=$(ta f)

function use
{
    echo "${taWhite}Using ${taWhiteOnBlue}$1${taOff}:"
}

function helper
{
    echo "This is normal text."

    # ta demo
    TA_OUTPUT="$(ta ^510)Terminal colors $(ta w)made easy$(ta +g) for all!!!$(ta f)"
    use "ta"
    echo $TA_OUTPUT

    # ta-code demo
    use "ta-code"
    echo "The escape code for $(ta y /m)yellow on magenta$(ta f) is ${taWhite}$(ta-code y /m)${taOff}"

    # tawrite demo
    use "tawrite"
    tawrite "@+c" "Terminal colors " "@+m" "made easy" "@y" " for all!!!" "@f" "\n" \
            "Now we are back to default text." "\n" \
            "Arguments starting with " "@w" "@" "@f" " are treated as specs (but not a lone @)." "\n"
    # NOTE-s on shell tawrite usage:
    #
    # For consistency with the tawrite function of the library, the tawrite
    # executable does not output a trailing newline automatically so you have to
    # add one manually.
    #
    # Since it may be difficult in some shells (like dash) to include a newline
    # character as part of a string literal, for convenience tawrite supports
    # providing the literal two-character sequence \n as a lone argument (i.e.
    # not as part of a larger string) to request a newline output. As seen
    # above, this can be anywhere in a tawrite command, not just at the end.
    #
    # It is also best to quote the \n to avoid the backslash being elided.
    #
    # tawrite also does not insert spaces between arguments since this is
    # undesirable between escape codes and the text they should apply to. So
    # spaces wanted in the output should be included as part of a quoted string.
    #
    # Thus tawrite is different from echo and should be treated differently.
    # In this demo we have used tawrite only where it's @-handling functionality
    # is required. This may be followed as a policy to reduce surprises.

    echo
}

function printSeparator
{
    echo -e "~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~\n"  # -e to evaluate \n; one \n is implicit in echo
}

taOfficialColors=$(ta g)"text"$taWhite"attr"$taOff
errorMessageLiteral=$(ta +r)"error message"$taOff

echo
echo "taOfficialColors length: ${taWhite}${#taOfficialColors}${taOff}"
echo "errorMessageLiteral length: ${taWhite}${#errorMessageLiteral}${taOff}"
echo

printSeparator
helper
TA_OUTPUT_BACKUP="$TA_OUTPUT"  # since it will be overwritten at next call to helper

# TA_DISABLED demo

use "TA_DISABLED"

echo -n "Now disabling $taOfficialColors: "
export TA_DISABLED=  # = is important to actually create the variable
done=("@y" "DONE!" "@f" "\n" "\n")
tawrite ${done[@]}
helper
echo "Note that ${taWhiteOnBlue}ta-code${taOff} works irrespective of ${taWhiteOnBlue}TA_DISABLED${taOff}."

echo -n "Now re-enabling $taOfficialColors: "
unset TA_DISABLED
tawrite ${done[@]}

printSeparator

# utilities demo

# ta-show demo
use "ta-show"
echo $TA_OUTPUT_BACKUP | ta-show

# ta-rm demo
use "ta-rm"
echo $TA_OUTPUT_BACKUP | ta-rm

# ta2html demo
use "ta2html"
tempFileName=$(mktemp "/tmp/XXXXXX.html")
echo $TA_OUTPUT_BACKUP | ta2html > $tempFileName
echo "HTML written to ${taWhite}${tempFileName}${taOff}; please check it."
echo

printSeparator

# error message demo

echo "If you input an illegal spec to ${taWhiteOnBlue}ta${taOff}: ${taBrightCyan}QXQX${taOff}"
echo $(ta QXQX)"Some text"
echo "... an $errorMessageLiteral is printed but the calling statement is otherwise executed."
echo

echo "Likewise in the case of" $taWhiteOnBlue"tawrite"$taOff":"
tawrite "Text preceding illegal spec " $taBrightCyan "XQXQ" $taOff "..." "@XQXQ" " text following it." "\n"
echo

printSeparator

# effect of TA_DISABLED on error messages

echo "Note that if $taOfficialColors is disabled via" $taWhiteOnBlue"TA_DISABLED"$taOff","
echo "illegal specs will not cause any $errorMessageLiteral."
echo
export TA_DISABLED=

echo "For example, when spec" $taBrightCyan"QQQ"$taOff "is input to" $taWhiteOnBlue"ta"$taOff "for:"
echo "$(ta QQQ)this text"
echo "... nothing happens."
echo

echo "Likewise, when spec" $taBrightCyan"XXX"$taOff "is input to" $taWhiteOnBlue"tawrite"$taOff "for:"
tawrite "@XXX" "this text" "\n"
echo

printSeparator

# end

echo "That concludes this demo!"
echo "Don't forget to read the help which you can see by running" $taWhite"ta-help"$taOff"."
echo
