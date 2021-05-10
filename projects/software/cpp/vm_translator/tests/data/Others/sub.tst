load sub.asm,
output-file sub.out,
compare-to sub.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

// Test a-b 
set RAM[256] 2,     // Reset stack top
set RAM[257] 3,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test b-a
set PC 0;
set RAM[256] 3,     // Reset stack top
set RAM[257] 2,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;
