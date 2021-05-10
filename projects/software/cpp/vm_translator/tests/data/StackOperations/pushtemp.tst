load pushtemp.asm,
output-file pushtemp.out,
compare-to pushtemp.cmp,
output-list RAM[0]%D2.6.2 RAM[10]%D2.6.2 RAM[256]%D2.6.2;

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[10] 51,     // Set 51 at TEMP + 5

repeat 20 {
  ticktock;
}
output;
