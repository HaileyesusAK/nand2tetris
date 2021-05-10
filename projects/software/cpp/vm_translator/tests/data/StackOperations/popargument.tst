load popargument.asm,
output-file popargument.out,
compare-to popargument.cmp,
output-list RAM[0]%D2.6.2 RAM[2]%D2.6.2 RAM[22]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[2] 17,      // Set address of ARG segment
set RAM[22] 0,      // Reset ARG + 5

repeat 20 {
  ticktock;
}
output;
