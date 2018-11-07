#! /usr/bin/env python3

# textattr demo
# =============
#
# Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
#
# Use, modification and distribution are permitted subject to the
# "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

from textattr import *

# repeatedly used escape codes
taWhite = ta("w")
taWhiteOnBlue = ta("w /b")
taBrightCyan = ta("+c")
taOff = ta("f")

def tempFileOpen():
    global tempFileName, tempFile
    from tempfile import mkstemp
    handle, tempFileName = mkstemp()
    tempFile = open(handle, "w")

def use(symbol):
    tawrite(taWhite, "Using ", taWhiteOnBlue, symbol, taOff, ":\n")

def helper():

    print("This is normal text.")

    # ta demo
    use("ta")
    print(ta("^510"), "Terminal colors ", ta("w"), "made easy", ta("+g"), " for all!!!", ta("f"), sep = "");

    # tawrite demo
    tawriteInput = ("@+c", "Terminal colors ", "@+m", "made easy", "@y", " for all!!!", "@f", "\n",
                    "Now we are back to default text.\n",
                    "@w i", "str", "@f", "-s starting with ", "@w", "@", "@f", " are treated as specs (but not a lone @).\n",
                    "Other types for which ", "@w i", "__str__", "@f", " is defined are also supported: ", "@n", 1, "@c", 'c', "@w", True, "@f", '\n')
    use("tawrite")
    tawrite(*tawriteInput)

    # tawrite file keyword demo
    use("tawrite keyword argument file")
    tempFileOpen()
    tawrite(*tawriteInput, file = tempFile)
    tempFile.close()
    print("Printed text to %s%s%s; please check it." % (taWhite, tempFileName, taOff))

    print()

def printSeparator():
    print("~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~ ~~~~\n") # one \n is implicit in print

taOfficialColors = ta("g") + "text" + taWhite + "attr" + taOff
errorMessageLiteral = ta("+r") + "error message" + taOff

print()
tawrite("taOfficialColors length: ", taWhite, len(taOfficialColors), taOff, "\n")
tawrite("errorMessageLiteral length: ", taWhite, len(errorMessageLiteral), taOff, "\n")
print()

printSeparator()
helper()

# taDisabled demo

use("taDisabled")

print("Now disabling %s: " % taOfficialColors, end = "")
taDisabled(True)
done = ("@y", "DONE!", "@f", "\n\n")
tawrite(*done)
helper()

print("Now re-enabling %s: " % taOfficialColors, end = "")
taDisabled(False)
tawrite(*done)

printSeparator()

# TextAttrError demo

use("TextAttrError")
print()

tawrite("If you input an illegal spec to ", taWhiteOnBlue, "ta", taOff, ": ", taBrightCyan, "QXQX", taOff, "\n")
try:
    print("%sSome text" % ta("QXQX"))
except TextAttrError as e:
    print("... the calling statement is never executed")
    tawrite("and we caught a ", taWhiteOnBlue, "TextAttrError", taOff, " with the ", errorMessageLiteral, ":\n")
    print("{%s}" % e)
print()

tawrite("But in the case of ", taWhiteOnBlue, "tawrite", taOff, ":\n")
try:
    tawrite("normal text preceding illegal spec ", taBrightCyan, "XQXQ", taOff, "...", "@XQXQ", "text with spec XQXQ")
except TextAttrError as e:
    tawrite("\n... will be output and we caught a ", taWhiteOnBlue, "TextAttrError", taOff, " with the ", errorMessageLiteral, ":\n")
    print("{%s}" % e)
print()

printSeparator()

# effect of taDisabled on exceptions

tawrite("Note that if ", taOfficialColors, " is disabled via ", taWhiteOnBlue, "taDisabled", taOff, ",\n")
print("illegal specs will not cause exceptions.")
print()
taDisabled(True)

tawrite("For example, when spec ", taBrightCyan, "QQQ", taOff, " is input to ", taWhiteOnBlue, "ta", taOff, " for:\n")
print("%sthis text" % ta("QQQ"))
print("... nothing happens.")
print()

tawrite("Likewise, when spec ", taBrightCyan, "XXX", taOff, " is input to ", taWhiteOnBlue, "tawrite", taOff, " for:\n")
tawrite("@XXX", "this text\n")
print("... no exception is thrown!")
print()

printSeparator()

# end

print("That concludes this demo!");
print("Don't forget to read the help which you can see by running %sta-help%s." % (taWhite, taOff))
print()
