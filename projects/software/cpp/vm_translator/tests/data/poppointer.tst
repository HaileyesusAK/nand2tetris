load poppointer.asm,
output-file poppointer.out,
compare-to poppointer.cmp,
output-list RAM[0]%D2.6.2 RAM[4]%D2.6.2; 

set RAM[256] 14,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[4] 0,      // Reset POINTER + 1

repeat 20 {
  ticktock;
}
output;
