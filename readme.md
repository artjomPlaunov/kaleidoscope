This is a simple implementation of the Kaleidoscope language in C++, following the tutorial from the LLVM documentation.

The code is based on the tutorial from the LLVM documentation, but it has been modified to make it modular and easier to understand.

The code is available on GitHub at https://github.com/llvm/llvm-project/tree/main/llvm/examples/kaleidoscope

My goal is to learn LLVM by hacking on this code, and I will also be writing a series of blog posts where I provide challenges and solutions to the problems I encounter: https://artjomplaunov.github.io/llvm/compilers/2024/12/09/llvm_codegen.html

The first challenge was to simply make the code more modular, and this version is provided on a separate branch labeled "beginnings". It uses the code from chapter 1-3 of the tutorial and splits it into separate files, to serve as a good starting point. It also makes the global context localized to the CodeGen class, to avoid polluting the global namespace.

