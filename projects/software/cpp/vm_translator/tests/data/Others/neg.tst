load neg.asm,
output-file neg.out,
compare-to neg.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

// Test negate on zero
set RAM[256] 0,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test negate on positive number 
set PC 0;
set RAM[256] 1,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test negate on negative number 
set PC 0;
set RAM[256] -1,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;
