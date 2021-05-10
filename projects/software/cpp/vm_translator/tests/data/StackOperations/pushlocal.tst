load pushlocal.asm,
output-file pushlocal.out,
compare-to pushlocal.cmp,
output-list RAM[0]%D2.6.2 RAM[1]%D2.6.2 RAM[256]%D2.6.2 RAM[25]%D2.6.2; 

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[1] 20,      // Set address of LCL segment
set RAM[25] 51,     // Set 51 at LCL + 5

repeat 20 {
  ticktock;
}
output;
