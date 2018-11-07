# NOTE:
# 1) The variables PYTHON2_LIB_DIR and PYTHON3_LIB_DIR should be defined to
#    indicate where textattr.py should be placed for Python import
# 2) The variable DLANG_COMPILER should be defined to indicate the D compiler
#    executable for the D demo, example and help-compiler to be compiled.

PREFIX = /usr/local
DOC_DIR = $(PREFIX)/share/doc/textattr/

ifndef PYTHON2_LIB_DIR
$(warning PYTHON2_LIB_DIR not defined. Python 2 language parts will be skipped.)
endif

ifndef PYTHON3_LIB_DIR
$(warning PYTHON3_LIB_DIR not defined. Python 3 language parts will be skipped.)
endif

ifdef DLANG_COMPILER
COMPILABLE_DEMO_D = build/demos/demo.d.bin
COMPILABLE_EXAMPLE_D = build/examples/example.d.bin
LIB_D = lib/textattr.d
else
$(warning DLANG_COMPILER not defined. D language parts will be skipped.)
endif

COMPILABLE_DEMOS = build/demos/demo.c.bin build/demos/demo.cpp.bin $(COMPILABLE_DEMO_D)
NONCOMPILABLE_DEMOS = demos/demo.py demos/demo.sh
COMPILABLE_EXAMPLES = build/examples/example.c.bin build/examples/example.cpp.bin $(COMPILABLE_EXAMPLE_D)
NONCOMPILABLE_EXAMPLES = examples/example.py examples/example.sh
UTILS = utils/ta-rm utils/ta-show utils/ta2html

COMPILABLES = build/ta build/libta.so build/libta++.so \
	build/help/ta-help build/help/help.html \
	$(COMPILABLE_DEMOS) $(COMPILABLE_EXAMPLES)
INSTALLABLES = LICENSE.txt $(COMPILABLES) \
	lib/textattr.h lib/textattr.hpp lib/textattr.py $(LIB_D) \
	$(UTILS) $(NONCOMPILABLE_DEMOS) $(NONCOMPILABLE_EXAMPLES)

C_SOURCES = lib/textattr.c lib/textattr.h
CXX_SOURCES = lib/textattr.cpp lib/textattr.hpp lib/textattr.c

# starting rule

ifdef $(DLANG_COMPILER)
all: build $(COMPILABLES) test
else
all: build $(COMPILABLES)
endif

# rules: core C/C++ compilables

build:
	mkdir build build/demos build/examples build/help

build/ta: $(C_SOURCES)
	$(CC) $(CFLAGS) -o build/ta lib/textattr.c -DTA_EXEC

build/libta.so: $(C_SOURCES)
	$(CC) $(CFLAGS) -shared -fPIC -o build/libta.so lib/textattr.c

build/libta++.so: $(CXX_SOURCES)
	$(CXX) $(CXXFLAGS) -shared -fPIC -o build/libta++.so lib/textattr.cpp

# rules: pattern (for demos, examples and help)

build/%.c.bin: %.c $(C_SOURCES)
	$(CC) $(CFLAGS) -o $@ $< lib/textattr.c -I lib/

build/%.cpp.bin: %.cpp $(CXX_SOURCES)
	$(CXX) $(CXXFLAGS) -g3 -o $@ $< lib/textattr.cpp -I lib/

# rules: help files

build/help.txt: build/help/help-compiler.c.bin help/help.txt.src
	build/help/help-compiler.c.bin help/help.txt.src > build/help/help.txt
	#cat build/help/help.txt | utils/ta-rm > build/help/help.txt.plain  # uncomment for debugging

build/help/ta-help: help/ta-help.c build/help.txt
	cat build/help/help.txt | sed -e 's/\o033/\\033/g' -e 's/.*/"&\\n"/g' > build/help/help.txt.esc
	$(CC) $(CFLAGS) -o build/help/ta-help help/ta-help.c -DHELP_TXT_ESC=\"../build/help/help.txt.esc\"
	rm build/help/help.txt.esc

build/help/help.html: build/help/help.txt utils/ta2html
	cat build/help/help.txt | utils/ta2html title="textattr syntax" > build/help/help.html

# rules: D language compilables

ifdef DLANG_COMPILER

ifeq ($(DLANG_COMPILER),gdc)
DLANG_OUT = -o
DLANG_CLEAN = rm $<.o
else
DLANG_OUT = -of=
DLANG_CLEAN =
endif

build/%.d.bin: %.d lib/textattr.d
	$(DLANG_COMPILER) $(DLANG_OUT)$@ $^
	$(DLANG_CLEAN)

test: build/help.txt build/help/help-compiler.d.bin
	build/help/help-compiler.d.bin help/help.txt.src > build/help/help.d.txt
	cmp build/help/help.txt build/help/help.d.txt

# NOTE:
# Help compilers are written only in languages which have their own
# implementation of the parsing of input specs and generation of output codes
# and the above comparison just ensures they are all on par with one another.

endif  # DLANG_COMPILER

clean:
	rm -f $(COMPILABLES) build/help/help.txt build/help/help.txt.plain

install: $(INSTALLABLES)
	# command line utilities
	install build/ta build/help/ta-help $(UTILS) $(PREFIX)/bin/
	ln -sf ta $(PREFIX)/bin/ta-code
	ln -sf ta $(PREFIX)/bin/tawrite
	# libraries
	install build/libta.so build/libta++.so $(PREFIX)/lib/ && ldconfig
	install -m644 lib/textattr.h lib/textattr.hpp $(PREFIX)/include/
ifdef PYTHON2_LIB_DIR
	install lib/textattr.py $(PYTHON2_LIB_DIR)/
endif
ifdef PYTHON3_LIB_DIR
	install lib/textattr.py $(PYTHON3_LIB_DIR)/
endif
ifdef DLANG_COMPILER
	install lib/textattr.d $(PREFIX)/include/dmd/
endif
	# documentation, demos and examples
	mkdir -p $(DOC_DIR) $(DOC_DIR)/demos/ $(DOC_DIR)/examples/
	install -m644 LICENSE.txt build/help/help.html $(DOC_DIR)
	install $(COMPILABLE_DEMOS) $(NONCOMPILABLE_DEMOS) $(DOC_DIR)/demos/
	install $(COMPILABLE_EXAMPLES) $(NONCOMPILABLE_EXAMPLES) $(DOC_DIR)/examples/

uninstall:
	# command line utilities
	for x in ta ta-help $(notdir $(UTILS)) ta-code tawrite ; do rm $(PREFIX)/bin/$$x ; done
	# libraries
	for x in libta.so libta++.so ; do rm $(PREFIX)/lib/$$x ; done ; ldconfig
	for x in textattr.h textattr.hpp ; do rm $(PREFIX)/include/$$x ; done
	# conditional libraries
ifdef PYTHON2_LIB_DIR
	rm $(PYTHON2_LIB_DIR)/textattr.py
endif
ifdef PYTHON3_LIB_DIR
	rm $(PYTHON3_LIB_DIR)/textattr.py
endif
ifdef DLANG_COMPILER
	rm $(PREFIX)/include/dmd/textattr.d
endif
	# documentation, demos and examples
	rm -Rf $(DOC_DIR)
