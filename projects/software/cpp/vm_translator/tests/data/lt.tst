load lt.asm,
output-file lt.out,
compare-to lt.cmp,
output-list RAM[0]%D2.6.2 RAM[256]%D2.6.2;

// Test when the top two elements are equal
set RAM[256] 111,     // Reset stack top
set RAM[257] 111,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test when the first element is less than the second element
set PC 0;
set RAM[256] 111,     // Reset stack top
set RAM[257] 110,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;

// Test when the first element is greater than the second element
set PC 0;
set RAM[256] 111,     // Reset stack top
set RAM[257] 112,     // Reset stack top
set RAM[0] 258,     // Set stack top pointer
repeat 20 {
  ticktock;
}
output;
