load pushpointer.asm,
output-file pushpointer.out,
compare-to pushpointer.cmp,
output-list RAM[0]%D2.6.2 RAM[4]%D2.6.2 RAM[256]%D2.6.2;

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[4] 37,     // Set 37 at POINTER + 1

repeat 20 {
  ticktock;
}
output;
