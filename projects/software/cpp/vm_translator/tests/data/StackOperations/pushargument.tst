load pushargument.asm,
output-file pushargument.out,
compare-to pushargument.cmp,
output-list RAM[0]%D2.6.2 RAM[2]%D2.6.2 RAM[256]%D2.6.2 RAM[25]%D2.6.2; 

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[2] 20,      // Set address of ARG segment
set RAM[25] 51,     // Set 51 at ARG + 5

repeat 20 {
  ticktock;
}
output;
