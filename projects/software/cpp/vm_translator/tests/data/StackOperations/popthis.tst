load popthis.asm,
output-file popthis.out,
compare-to popthis.cmp,
output-list RAM[0]%D2.6.2 RAM[3]%D2.6.2 RAM[22]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[3] 17,      // Set address of THIS segment
set RAM[22] 0,      // Reset THIS + 5

repeat 20 {
  ticktock;
}
output;
