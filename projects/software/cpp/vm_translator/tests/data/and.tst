load and.asm,
output-file and.out,
compare-to and.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

// Test when the top two elements are false 
set RAM[256] 0,     // Reset stack top
set RAM[257] 0,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test when the first element is false while the second is true
set PC 0;
set RAM[256] 111,     // Reset stack top
set RAM[257] 0,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test when the first element is false while the second is true
set PC 0;
set RAM[256] 0,     // Reset stack top
set RAM[257] -1,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test when both elements are true 
set PC 0;
set RAM[256] 1,     // Reset stack top
set RAM[257] -1,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;
