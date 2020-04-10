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
//push constant 111
	@111
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 333
	@333
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 888
	@888
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//pop static 8
	@SP
	AM=M-1
	D=M
	@StaticTest.vm.8
	M=D

//pop static 3
	@SP
	AM=M-1
	D=M
	@StaticTest.vm.3
	M=D

//pop static 1
	@SP
	AM=M-1
	D=M
	@StaticTest.vm.1
	M=D

//push static 3
	@StaticTest.vm.3
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push static 1
	@StaticTest.vm.1
	D=M
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

//push static 8
	@StaticTest.vm.8
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//add
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D+M

(END)
	@END
	0;JMP

