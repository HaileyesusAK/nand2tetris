load return.asm,
output-file return.out,
compare-to return.cmp,
output-list RAM[0]%D2.6.2 RAM[1]%D2.6.2 RAM[2]%D2.6.2 RAM[3]%D2.6.2 RAM[4]%D2.6.2; 

set RAM[0] 300,     // Set stack top pointer
set RAM[1] 300,       // Reset callee's LCL address
set RAM[2] 295,       // Reset callee's ARG address
set RAM[3] 0,       // Reset caller's THIS address
set RAM[4] 0,       // Reset caller's THAT address
set RAM[295] 500, // Return address
set RAM[296] 600,    // Caller's LCL address
set RAM[297] 700,   // Caller's ARG address
set RAM[298] 800,    // Caller's THIS address
set RAM[299] 900,   // Caller's THAT address

repeat 100 {
  ticktock;
}
output;
