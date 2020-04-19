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
//function Class1.set 0
(Class1.set)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Class1.set_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Class1.set_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Class1.set_SET_LCL
	0;JMP
(Class1.set_SET_LCL_END)

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

//pop static 0
	@SP
	AM=M-1
	D=M
	@Class1.vm.0
	M=D

//push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//pop static 1
	@SP
	AM=M-1
	D=M
	@Class1.vm.1
	M=D

//push constant 0
	@0
	D=A
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

//function Class1.get 0
(Class1.get)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Class1.get_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Class1.get_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Class1.get_SET_LCL
	0;JMP
(Class1.get_SET_LCL_END)

//push static 0
	@Class1.vm.0
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push static 1
	@Class1.vm.1
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

//function Class2.set 0
(Class2.set)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Class2.set_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Class2.set_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Class2.set_SET_LCL
	0;JMP
(Class2.set_SET_LCL_END)

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

//pop static 0
	@SP
	AM=M-1
	D=M
	@Class2.vm.0
	M=D

//push argument 1
	@1
	D=A
	@ARG
	A=D+M
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//pop static 1
	@SP
	AM=M-1
	D=M
	@Class2.vm.1
	M=D

//push constant 0
	@0
	D=A
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

//function Class2.get 0
(Class2.get)
	@i
	M=0
	@0
	D=A
	@n
	M=D
(Class2.get_SET_LCL)
	@n
	D=M
	@i
	D=D-M
	@Class2.get_SET_LCL_END
	D;JEQ
	@SP
	A=M
	M=0
	@SP
	M=M+1
	@i
	M=M+1
	@Class2.get_SET_LCL
	0;JMP
(Class2.get_SET_LCL_END)

//push static 0
	@Class2.vm.0
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push static 1
	@Class2.vm.1
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

//push constant 6
	@6
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

//call Class1.set 2
	@END_Class1.set
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
	@2
	D=D-A
	@SP
	D=M-D
	@ARG
	M=D
	@Class1.set
	0;JMP
(END_Class1.set)

//pop temp 0 // Dumps the return value
	@SP
	AM=M-1
	D=M
	@5
	M=D

//push constant 23
	@23
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//push constant 15
	@15
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1

//call Class2.set 2
	@END_Class2.set
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
	@2
	D=D-A
	@SP
	D=M-D
	@ARG
	M=D
	@Class2.set
	0;JMP
(END_Class2.set)

//pop temp 0 // Dumps the return value
	@SP
	AM=M-1
	D=M
	@5
	M=D

//call Class1.get 0
	@END_Class1.get
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
	@0
	D=D-A
	@SP
	D=M-D
	@ARG
	M=D
	@Class1.get
	0;JMP
(END_Class1.get)

//call Class2.get 0
	@END_Class2.get
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
	@0
	D=D-A
	@SP
	D=M-D
	@ARG
	M=D
	@Class2.get
	0;JMP
(END_Class2.get)

//label WHILE
(Class2.get$WHILE)

//goto WHILE
	@Class2.get$WHILE
	0; JMP

(END)
	@END
	0;JMP

