load poptemp.asm,
output-file poptemp.out,
compare-to poptemp.cmp,
output-list RAM[0]%D2.6.2 RAM[10]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[10] 0,      // Reset TEMP + 5

repeat 20 {
  ticktock;
}
output;
