load poplocal.asm,
output-file poplocal.out,
compare-to poplocal.cmp,
output-list RAM[0]%D2.6.2 RAM[1]%D2.6.2 RAM[22]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[1] 17,      // Set address of LCL segment
set RAM[22] 0,      // Reset LCL + 5

repeat 20 {
  ticktock;
}
output;
