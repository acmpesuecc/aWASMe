# Week 4 Progress

## Samyuktha's Progress
- Decoded the code section 
  - All opcodes except scope types (currently, engine supports `block`, `loop`, and `if`) have been decoded
  - If arguments exist for an opcode, they have been decoded and pushed into module object as well
### Next Week
- Decode the scope types - will use a stack for this to match the beginning and end of blocks properly
  - Figure out how to handle if-else blocks 

## Shashi's Progress
- Implemented linear memory
   - Initialized `memory` vector with it's attributes
   - Implemented memory based functions from the [docs](https://developer.mozilla.org/en-US/docs/WebAssembly/Reference/Memory)
   - Also connected it to the parser, so data can load directly into the memory
- Changed the demo to show memory ops
### Next Week
   - Host Functions using [embind](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#)
   - Testing with actual code
   - UI, if possible

## Aston's Progress
- Fixed argument loading order in ``src/engine/main.cpp``
- Handled floating point division
- Added support for ``br_if`` instruction
- Implemented Global variables
- Added conversion instructions
  - Conversion from floats to floats, ints to ints and floats to ints and the other way around
### Next Week
- Support imported functions
  - call imported JS functions from C++/VM using [embind](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#)
  - Handle returning values from JS functions
