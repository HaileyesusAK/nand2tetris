load call.asm,
output-file call.out,
compare-to call.cmp,
output-list RAM[0]%D2.6.2 RAM[1]%D2.6.2 RAM[2]%D2.6.2 RAM[257]%D2.6.2 RAM[258]%D2.6.2 RAM[259]%D2.6.2 RAM[260]%D2.6.2; 

set RAM[0] 256,     // Set stack top pointer
set RAM[1] 300,     // Set LCL base of the caller
set RAM[2] 400,     // Set ARG base of the caller
set RAM[3] 500,     // Set THIS base of the caller
set RAM[4] 600,     // Set THAT base of the caller

repeat 100 {
  ticktock;
}
output;
