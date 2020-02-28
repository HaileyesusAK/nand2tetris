load TFM16.hdl,
output-file TFM16.out,
compare-to TFM16.cmp,
output-list in%B1.16.1 z%B1.1.1 n%B1.1.1 out%B1.16.1;

set in %B0000000000000000,

set z 0,
set n 0,
eval,
output;

set z 0,
set n 1,
eval,
output;

set z 1,
set n 0,
eval,
output;

set z 1,
set n 1,
eval,
output;

set in %B1111111111111111,

set z 0,
set n 0,
eval,
output;

set z 0,
set n 1,
eval,
output;

set z 1,
set n 0,
eval,
output;

set z 1,
set n 1,
eval,
output;

set in %B0101101110100000,

set z 0,
set n 0,
eval,
output;

set z 0,
set n 1,
eval,
output;

set z 1,
set n 0,
eval,
output;

set z 1,
set n 1,
eval,
output;
