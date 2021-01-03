load popstatic.asm,
output-file popstatic.out,
compare-to popstatic.cmp,
output-list RAM[0]%D2.6.2 RAM[16]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[16] 0,      // Reset the first variable

repeat 20 {
  ticktock;
}
output;
