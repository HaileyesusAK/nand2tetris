load pushthat.asm,
output-file pushthat.out,
compare-to pushthat.cmp,
output-list RAM[0]%D2.6.2 RAM[4]%D2.6.2 RAM[256]%D2.6.2 RAM[25]%D2.6.2; 

set RAM[0] 256,     // Set stack top pointer
set RAM[256] 0,     // Reset stack top
set RAM[4] 20,      // Set address of THAT segment
set RAM[25] 51,     // Set 51 at THAT + 5

repeat 20 {
  ticktock;
}
output;
