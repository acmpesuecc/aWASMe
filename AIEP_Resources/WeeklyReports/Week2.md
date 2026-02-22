# Week 2 Progress

## Samyuktha's Progress
- Set up the environment for parsing (defined the module class and the structs inside them, set up the header files and supporting .cpp files)
- The program is currently able to access each section of the WASM file individually
### Next Week
- Complete the parse_code_section function
   - extract the function code data from the code section
   - set up the mapping from raw binary to opcodes for translation purposes
   - store the translated opcodes in the functions part of the module object)
- Start looking into how to parse the linear memory and export sections
