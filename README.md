# aWASMe
A WASM interpreter with dual modes - one as a browser-based VM and one as a simulator for WASM. It uses a structured parsing and execution pipeline in C++ & emscripten, to load binary modules, validate and decode instructions, and simulate a full virtual machine directly in the browser.

## Mentors
- [Selvaganesh Arunmozhi](https://github.com/thisisselva18)
- [Sarah Kazi](https://github.com/Sarah-Kazi)
- [Nirupama Jayaraman](https://github.com/midnightlune)

## Mentees 
- [Aston Venol D'Souza](https://github.com/Quan1umMango)
- [Samyuktha Ramesh Babu](https://github.com/gitsumchips)
- [Shashidharan VS](https://github.com/shashidharanvs)
- [Chitra Likith Kumar](https://github.com/likith951)

## Dependencies
- Make
- C++20
- Emscripten compiler (emcc)

## Build and run
For engine: `make engine && ./engine.exe`
For parser: `make parser && ./parser.exe "<path_to_wasm_file>"`

## Future improvements
- Supporting tables, reference types
- Adding support for exception handling instructions
- Supporting SIMD vector instructions
- Complete the simulator, which will allow users to step through the code as well as view WASM's various stacks and linear memory.
