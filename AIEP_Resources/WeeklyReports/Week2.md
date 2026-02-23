# Week 2 Progress

## Aston's Progress
- Started working on the engine along with Shashi
- Implemented instructions such as ``block``, ``loop``, ``end``,``br``,``if`` and ``else``
- Added stack validation function and error reporting
- Misc docs
### Next Week
- Implement Function calls and call stack
- Implement ``local`` instructions
- Get a basic calculator type programing working


## Samyuktha's Progress
- Set up the environment for parsing (defined the module class and the structs inside it, set up the header files and supporting .cpp files)
- The program is currently able to access each section of the WASM file individually
### Next Week
- Complete the parse_code_section function
   - extract the function code data from the code section
   - set up the mapping from raw binary to opcodes for translation purposes
   - store the translated opcodes in the functions part of the module object
- Start looking into how to parse the linear memory and export sections


## Shashi's Progress
- Initialized the engine loop & added basic arithmetic operations (``CONST``, ``ADD``, ``SUB``, ``MUL``, ``END``)
- Made a vector to store the operands & the ability to push & pop from it
- Also implemented a instruction pointer(``ip``) to track operations sequentially
### Next Week
- Implement improved error handling
- Implementing branch conditionals
- Reading up on ``local`` instructions

## Likith's Progress
- Implemented Load wasm file function
- Implemented a LEB128 decoder

### Next week
- Complete parse_section function 
- Refactor and improve the structure of the existing codebase
- Develop validation mechanisms for parsed sections

 
  

