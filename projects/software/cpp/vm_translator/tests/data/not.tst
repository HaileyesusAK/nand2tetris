load not.asm,
output-file not.out,
compare-to not.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

// Test NOT on zero
set RAM[256] 0,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test NOT on positive number 
set PC 0;
set RAM[256] 1,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test NOT on negative number 
set PC 0;
set RAM[256] -1,     // Reset stack top
set RAM[0] 257,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;
