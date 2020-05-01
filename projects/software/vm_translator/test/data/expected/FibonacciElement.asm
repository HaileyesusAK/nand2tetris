	@BEGIN
	0;JMP

(PUSH_TRUE)
	@SP
	A=M
	M=-1
	@SP
	M=M+1
	@R13
	A=M
	0;JMP

(PUSH_FALSE)
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@R13
	A=M
	0;JMP

(BEGIN)
	@256
	D=A
	@SP
	M=D
	@Sys.init
	0;JMP
//function Main.fibonacci 0
(Main.fibonacci)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Main.fibonacci_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Main.fibonacci_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Main.fibonacci_SET_LCL
	0;JMP
(Main.fibonacci_SET_LCL_END)

//push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 2
	@2
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//lt                     // checks if n<2
	@10_op_end
	D=A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@SP
	AM=M-1
	D=M-D
	@PUSH_TRUE
	D;JLT
	@PUSH_FALSE
	0;JMP
(10_op_end)

//if-goto IF_TRUE
	@SP
	AM=M-1
	D=M+1
	@Main.fibonacci$IF_TRUE
	D;JEQ

//goto IF_FALSE
	@Main.fibonacci$IF_FALSE
	0; JMP

//label IF_TRUE          // if n<2, return n
(Main.fibonacci$IF_TRUE)

//push argument 0        
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//return
	@LCL
	D=M
	@frame
	M=D
	@5
	D=A
	@frame
	A=M-D
	D=M
	@ret
	M=D
	@SP
	A=M-1
	D=M
	@ARG
	A=M
	M=D
	@ARG
	D=M+1
	@SP
	M=D
	@frame
	AM=M-1
	D=M
	@THAT
	M=D
	@frame
	AM=M-1
	D=M
	@THIS
	M=D
	@frame
	AM=M-1
	D=M
	@ARG
	M=D
	@frame
	AM=M-1
	D=M
	@LCL
	M=D
	@ret
	A=M
	0;JMP

//label IF_FALSE         // if n>=2, returns fib(n-2)+fib(n-1)
(Main.fibonacci$IF_FALSE)

//push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 2
	@2
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//sub
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=M-D

//call Main.fibonacci 1  // computes fib(n-2)
	@END_Main.fibonacci
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@5
	D=A
	@1
	D=D+A
	@SP
	D=M-D
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(END_Main.fibonacci)

//push argument 0
	@0
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 1
	@1
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//sub
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=M-D

//call Main.fibonacci 1  // computes fib(n-1)
	@END_Main.fibonacci
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@5
	D=A
	@1
	D=D+A
	@SP
	D=M-D
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(END_Main.fibonacci)

//add                    // returns fib(n-1) + fib(n-2)
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D+M

//return
	@LCL
	D=M
	@frame
	M=D
	@5
	D=A
	@frame
	A=M-D
	D=M
	@ret
	M=D
	@SP
	A=M-1
	D=M
	@ARG
	A=M
	M=D
	@ARG
	D=M+1
	@SP
	M=D
	@frame
	AM=M-1
	D=M
	@THAT
	M=D
	@frame
	AM=M-1
	D=M
	@THIS
	M=D
	@frame
	AM=M-1
	D=M
	@ARG
	M=D
	@frame
	AM=M-1
	D=M
	@LCL
	M=D
	@ret
	A=M
	0;JMP

//function Sys.init 0
(Sys.init)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Sys.init_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Sys.init_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Sys.init_SET_LCL
	0;JMP
(Sys.init_SET_LCL_END)

//push constant 4
	@4
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//call Main.fibonacci 1   // computes the 4'th fibonacci element
	@END_Main.fibonacci
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@5
	D=A
	@1
	D=D+A
	@SP
	D=M-D
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(END_Main.fibonacci)

//label WHILE
(Sys.init$WHILE)

//goto WHILE              // loops infinitely
	@Sys.init$WHILE
	0; JMP

(END)
	@END
	0;JMP

