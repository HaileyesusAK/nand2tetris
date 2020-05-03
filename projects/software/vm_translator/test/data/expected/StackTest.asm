//push constant 17
	@17
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 17
	@17
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//eq
	@1_op_end
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
	D;JEQ
	@PUSH_FALSE
	0;JMP
(1_op_end)

//push constant 17
	@17
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 16
	@16
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//eq
	@2_op_end
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
	D;JEQ
	@PUSH_FALSE
	0;JMP
(2_op_end)

//push constant 16
	@16
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 17
	@17
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//eq
	@3_op_end
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
	D;JEQ
	@PUSH_FALSE
	0;JMP
(3_op_end)

//push constant 892
	@892
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 891
	@891
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//lt
	@4_op_end
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
(4_op_end)

//push constant 891
	@891
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 892
	@892
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//lt
	@5_op_end
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
(5_op_end)

//push constant 891
	@891
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 891
	@891
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//lt
	@6_op_end
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
(6_op_end)

//push constant 32767
	@32767
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 32766
	@32766
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//gt
	@7_op_end
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
	D;JGT
	@PUSH_FALSE
	0;JMP
(7_op_end)

//push constant 32766
	@32766
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 32767
	@32767
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//gt
	@8_op_end
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
	D;JGT
	@PUSH_FALSE
	0;JMP
(8_op_end)

//push constant 32766
	@32766
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 32766
	@32766
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//gt
	@9_op_end
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
	D;JGT
	@PUSH_FALSE
	0;JMP
(9_op_end)

//push constant 57
	@57
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 31
	@31
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 53
	@53
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

//push constant 112
	@112
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

//neg
	@SP
	A=M-1
	M=-M

//and
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D&M

//push constant 82
	@82
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//or
	@SP
	AM=M-1
	D=M
	@SP
	A=M-1
	M=D|M

//not
	@SP
	A=M-1
	M=!M

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

