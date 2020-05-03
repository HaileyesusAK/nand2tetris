//push constant 7
	@7
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 8
	@8
	D=A
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

