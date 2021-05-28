# Vmachine

A virtual machine designed to learn about cpu architecture.

## Why ?

I made this virtual machine to confirm that I understood how computer and cpu worked.
As a side effect, I guess it can also be used to learn about cpu architectures, machine code, assembly and compilers !

This is the very first version, a proof of concept. I plan to 

## Architecture

- 16 bytes memory
- "8 bits" architecture : 8 bits instruction register, but in reality 4 bits IR and 4 bit MOD.
- 4 bits accumulator
- 4 bits program counter

## Instruction set

- NOP : ```0000 ----```` no op, used to store value
- LDA : ```0001 ADDR``` load value at addr into accumulator
- ADD : ```0010 ADDR``` add value at addr to accumulator
- SUB : ```0011 ADDR``` subst value at addr to accumulator
- STO : ```0100 ADDR``` store accumulator value into addr
- OUT : ```0101 ----``` output value in accumulator
- JMP : ```0110 ADDR``` change program counter to value in addr
- HLT : ```0111 ----``` halt computer
- CMP : ```1000 ADDR``` compare value in addr and ACC, PC +1 if true

## How to

- ```ESCAPE``` stop the machine at the current step.
- When stop, any key will quit the program.

## Requirements

- SDL2
- make

## TODO and Ideas

- [x] add comparator instruction
- [x] visual representation with SDL
- [ ] load "binary" from file to program memory
- [ ] better delay management: delay all the time and full-speed when key pressed ? Different speed settings ? Programmable frequency ?
- [ ] extend to 16 bits instructions : 4 bits op code, 12 bits modifier (4096 bytes of addressable memory) ?
- [ ] python assembler to translate assembly to "machine code" that can be read by the virtual machine.
- [ ] multiple memory banks ?
- [ ] framebuffer and display ?
- [ ] IOs ? At least keyboard ?
- [ ] Stack architecture ?

