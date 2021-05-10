load pushstatic.asm,
output-file pushstatic.out,
compare-to pushstatic.cmp,
output-list RAM[0]%D2.6.2 RAM[16]%D2.6.2 RAM[256]%D2.6.2;

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[16] 51,     // Set 51 at the first variable

repeat 20 {
  ticktock;
}
output;
