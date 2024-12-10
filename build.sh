#!/bin/sh
clang++ -g -O3 \
    main.cpp \
    Lexer.cpp \
    Parser.cpp \
    CodeGen.cpp \
    `llvm-config --cxxflags --ldflags --system-libs --libs core` \
    -o kaleidoscope 