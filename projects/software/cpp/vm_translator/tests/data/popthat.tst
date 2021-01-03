load popthat.asm,
output-file popthat.out,
compare-to popthat.cmp,
output-list RAM[0]%D2.6.2 RAM[4]%D2.6.2 RAM[22]%D2.6.2; 

set RAM[256] 48,    // Set stack top
set RAM[0] 257,     // Set stack top pointer
set RAM[4] 17,      // Set address of THAT segment
set RAM[22] 0,      // Reset THAT + 5

repeat 20 {
  ticktock;
}
output;
