load func.asm,
output-file func.out,
compare-to func.cmp,
output-list RAM[1]%D2.6.2 RAM[300]%D2.6.2 RAM[301]%D2.6.2;

set RAM[1] 300,     // Set LCL base address
set RAM[300] -1,     // Reset LCL segment
set RAM[301] -1,     // Reset LCL segment

repeat 100 {
  ticktock;
}
output;
