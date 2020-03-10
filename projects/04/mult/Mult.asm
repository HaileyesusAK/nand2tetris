// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

//Initialization:
//    i=0, a counter variable
//    sum=0, to accumulate the sums,
//             this will be the product when the iteration finishes.

    @i              //create variable i
    M=0             //initialize i to zero
    @sum            //create variable sum
    M=0             //initialize sum to zero

(LOOP_BEG)
//Check if end of loop is reached
    @i
    D=M
    @R0
    D=D-M
    @LOOP_END
    D;JGE           //Goto LOOP_END if i >= R0
    @R1
    D=M
    @sum            //Load sum
    M=D+M           //Update sum += R1
    @i
    M=M+1           //Increment i += 1
    @LOOP_BEG
    0;JMP           //Goto LOOP_BEG

(LOOP_END)
    @sum
    D=M
    @R2
    M=D
    @END
    0; JMP
(END)
    @END
    0;JMP
