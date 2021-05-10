load pushconstant.asm,
output-file pushconstant.out,
compare-to pushconstant.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top

repeat 20 {
  ticktock;
}
output;
