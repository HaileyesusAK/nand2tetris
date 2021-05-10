load pushthis.asm,
output-file pushthis.out,
compare-to pushthis.cmp,
output-list RAM[0]%D2.6.2 RAM[3]%D2.6.2 RAM[256]%D2.6.2 RAM[25]%D2.6.2; 

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[3] 20,      // Set address of THIS segment
set RAM[25] 51,     // Set 51 at THIS + 5

repeat 20 {
  ticktock;
}
output;
