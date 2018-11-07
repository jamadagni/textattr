# textattr (ta)
# =============
#
# Makes adding color and attributes to beautify the terminal output of your
# program easier by translating human-readable specs into ANSI escape codes
#
# Copyright (C) 2018, Shriramana Sharma, samjnaa-at-gmail-dot-com
#
# Use, modification and distribution are permitted subject to the
# "BSD-2-Clause"-type license stated in the accompanying file LICENSE.txt

__all__ = ["ta", "tawrite", "taDisabled", "TextAttrError"]

from ctypes import CDLL, c_char_p
_lib = CDLL("libta.so")
_ta_n = _lib._ta_n
_ta_n.argtypes = [c_char_p]
_ta_n.restype = c_char_p

class TextAttrError(ValueError):
    pass

_taDisabled = False

def taDisabled(val):
    global _taDisabled
    if type(val) is not bool:
        raise TypeError("argument to ‘taDisabled’ should be of type bool")
    _taDisabled = val

def ta(specString):

    if _taDisabled:
        return ""  # NOTE: not checking for errors
    if specString == "f":
        return "\033[0m"  # frequent use

    t = type(specString)
    if t is bytes:
        pass
    elif t is str:
        specString = specString.encode()
    else:
        raise TypeError("argument to ‘ta’ should be of type str or bytes")

    codeSeq = _ta_n(specString, len(specString))
    if codeSeq == b"":
        raise TextAttrError(c_char_p.in_dll(_lib, "taErrorMsg").value.decode())
    return codeSeq.decode()
    # NOTE: .decode() above is unnecessary as far as a terminal is concerned since
    # anyhow it has to be provided encoded bytes in the end. However, without it, print()
    # runs str() on the bytes object and produces undesired output as b'\x1b[...'

def tawrite(*args, **kwargs):

    import sys
    _file = sys.stdout
    for kw in kwargs:
        if kw == "file":
            _file = kwargs[kw]
        else:
            raise TypeError("'{}' is an invalid keyword argument for this function".format(kw))

    write = _file.write
    for arg in args:
        if type(arg) is str and len(arg) > 1 and arg[0] == "@":
            write(ta(arg[1:]))
        else:
            write(str(arg))
