#define _GNU_SOURCE
#include <ctype.h>
#include <search.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm_translator.h"

#define CMD_LEN					9
#define SEGMENT_LEN				9
#define N_INST		29

#define PRE_POP					"@SP\nM=M-1\n@SP\nA=M\n"			//M is the top stack element
#define	POP_ASM					"@SP\nA=M-1\nD=M\n"				//Pop from the stack onto D register
#define	PUSH_ASM				"@SP\nA=M\nM=D\n@SP\nM=M+1\n"	//Push the content of D register onto the stack		
#define PUSH_R14				"@R14\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@R13\n0;JMP\n"

typedef enum {
	EQ,
	LT,
	GT
}LogicalOp;

typedef enum {
	LCL=1,
	ARG=2,
	THIS=3,
	THAT=4,
	TEMP=5,
	STATIC,
	POINTER,
	CONSTANT
}SegmentBase;

typedef enum {
	BLANK,
	STACK_OP,
	ALU_OP
}InstType;

typedef size_t Generator(char *inst, ...);


static void *RAM = NULL;
static uint16_t PC;				//Program counter, used when generating instruction for logical operations

static struct hsearch_data generator_mapping;

static size_t parse_word(const char* src, char* dst, size_t n_dst);
static int parse_vm_inst(const char* vm_inst, char *cmd, InstType* inst_type,
						 char *segment, uint16_t* idx);

static size_t gen_add_asm(char *asm_inst);
static size_t gen_and_asm(char *asm_inst);
static size_t gen_binary_logical_asm(char *asm_inst, LogicalOp op);
static size_t gen_binary_op_asm(char *asm_inst, char *op);
static size_t gen_eq_asm(char *asm_inst);
static size_t gen_gt_asm(char *asm_inst);
static size_t gen_jmp_log_asm(char *asm_inst, const char *jmp_cmd);
static size_t gen_lt_asm(char *asm_inst);
static size_t gen_neg_asm(char *asm_inst);
static size_t gen_not_asm(char *asm_inst);
static size_t gen_or_asm(char *asm_inst);
static size_t gen_PC_R13_asm(char *asm_inst, uint16_t offset);
static size_t gen_sub_asm(char *asm_inst);
static size_t gen_unary_op_asm(char *asm_inst, char *op);

static size_t gen_pop_asm(char *asm_inst, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_asm(char *asm_inst, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_lcl_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_lcl_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_arg_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_arg_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_static_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_static_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_ptr_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_ptr_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_this_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_this_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_that_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_that_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_temp_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_temp_asm(char *asm_inst, uint16_t idx);
static size_t gen_push_const_asm(char *asm_inst, uint16_t idx);
static size_t gen_pop_const_asm(char *asm_inst, uint16_t idx);


void *init_translator()
{
	if(!hcreate_r(N_INST, &generator_mapping))
		return NULL;

	RAM = calloc(1, RAM_SIZE);
	if(!RAM)
		hdestroy_r(&generator_mapping);

	struct {
		char *cmd;
		void *generator;
	} cmd_generators[N_INST] = { 
		{"add",  gen_add_asm}, {"sub", gen_sub_asm}, {"neg", gen_neg_asm},
		{"eq", gen_eq_asm}, {"gt", gen_gt_asm}, {"lt", gen_lt_asm},
		{"and", gen_and_asm}, {"or", gen_or_asm}, {"not", gen_not_asm},
		{"push_local", gen_push_lcl_asm}, {"pop_local", gen_pop_lcl_asm},
		{"push_argument", gen_push_arg_asm}, {"pop_argument", gen_pop_arg_asm},
		{"push_this", gen_push_this_asm}, {"pop_this", gen_pop_this_asm},
		{"push_that", gen_push_that_asm}, {"pop_that", gen_pop_that_asm},
		{"push_pointer", gen_push_ptr_asm}, {"pop_pointer", gen_pop_ptr_asm},
		{"push_temp", gen_push_temp_asm}, {"pop_temp", gen_pop_temp_asm},
		{"push_static", gen_push_static_asm}, {"pop_static", gen_pop_static_asm},
		{"push_constant", gen_push_const_asm}, {"pop_constant", gen_pop_const_asm},
	};

	size_t i = 0; 
	ENTRY item, *entry;
	for(i = 0; i < N_INST; ++i)
	{
		item.key = cmd_generators[i].cmd;
		item.data = (void*)cmd_generators[i].generator;
		hsearch_r(item, ENTER, &entry, &generator_mapping);
	}
	
	return RAM;
}

static size_t parse_word(const char* src, char* dst, size_t n_dst)
{
	size_t i = 0;
	const char *c = src;
	
	while(*c && isblank(*c))
		c++;
	
	while(*c && !isblank(*c) && i < n_dst - 1)
		dst[i++] = *c++;

	dst[i] = '\0';

	return i;
}

int translate_vm_inst(const char* vm_inst, char* asm_inst, size_t* inst_len)
{
	if(!vm_inst || !asm_inst || !inst_len)
		return -1;

	char cmd[CMD_LEN];
	char segment[SEGMENT_LEN];
	uint16_t idx;
	InstType inst_type;
	ENTRY item, *entry;
	parse_vm_inst(vm_inst, cmd, &inst_type, segment, &idx);

	if(inst_type == STACK_OP)
	{
		char command[CMD_LEN + SEGMENT_LEN + 2];
		snprintf(command, CMD_LEN + SEGMENT_LEN + 2, "%s_%s", cmd, segment);
		item.key = command;
		if(!hsearch_r(item, FIND, &entry, &generator_mapping))
			return -2;
		
		Generator *generator = (Generator*)entry->data;
		*inst_len = generator(command, idx);
	}
	else
	{
		item.key = cmd;
		if(!hsearch_r(item, FIND, &entry, &generator_mapping))
			return -2;
		
		Generator *generator = (Generator*)entry->data;
		*inst_len = generator(cmd);
	}
	
	return 0;
}

static int parse_vm_inst(const char* vm_inst, char *cmd, InstType* inst_type,
						 char *segment, uint16_t* idx)
{
	const char *c = vm_inst;
	size_t word_len;	
	*inst_type = BLANK;

	while(*c && isblank(*c))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	word_len = parse_word(c, cmd, CMD_LEN);
	if(!strcmp(cmd, "push") || !strcmp(cmd, "pop"))
	{
		*inst_type = STACK_OP;
		word_len = parse_word(c + word_len, segment, SEGMENT_LEN);
		sscanf(c + word_len, "%hu", idx);
	}
	else
	{
		*inst_type = ALU_OP;
	}

	return 0;
}


static size_t gen_binary_op_asm(char *asm_inst, char *op)
{
	return sprintf(asm_inst, "%s\nA=M-1\nM=M%sD\n@SP\nM=M-1\n", POP_ASM, op);
}

static size_t gen_add_asm(char *asm_inst)
{
	return gen_binary_op_asm(asm_inst, "+");
}

static size_t gen_sub_asm(char *asm_inst)
{
	return gen_binary_op_asm(asm_inst, "-");
}

static size_t gen_or_asm(char *asm_inst)
{
	return gen_binary_op_asm(asm_inst, "|");
}

static size_t gen_and_asm(char *asm_inst)
{
	return gen_binary_op_asm(asm_inst, "&");
}

static  size_t gen_unary_op_asm(char *asm_inst, char *op)
{
	return sprintf(asm_inst, "@SP\nA=M-1\nM=%sD\n", op);
}

static size_t gen_not_asm(char *asm_inst)
{
	return gen_unary_op_asm(asm_inst, "!");
}

static size_t gen_neg_asm(char *asm_inst)
{
	return gen_unary_op_asm(asm_inst, "-");
}

static size_t gen_jmp_log_asm(char *asm_inst, const char *jmp_cmd)
{
	return sprintf(asm_inst,
				   "@R14\n"
				   "M=-1\n"
				   "@PUSH_R14\n"
				   "D;%s\n"
				   "@R14\n"
				   "M=0\n"
				   "@PUSH_R14\n"
				   "0;JMP\n",
				   jmp_cmd);
}


static size_t gen_PC_R13_asm(char *asm_inst, uint16_t offset)
{
	return sprintf(asm_inst, "@%hu\n"
							 "D=A\n"
							 "@%hu\n"
							 "D=D+A\n"
							 "@R13\n"
							 "M=D\n",
				   offset,
				   PC);
}

static size_t gen_binary_logical_asm(char *asm_inst, LogicalOp op)
{

	const size_t offset = 24;		//number of lines generated by this function

	char *jmp_cmd;
	switch(op)
	{
		case EQ: jmp_cmd = "JEQ"; break;
		case LT: jmp_cmd = "JLT"; break;
		case GT: jmp_cmd = "JGT"; break;
	}

	size_t len = gen_PC_R13_asm(asm_inst, offset);
	len += sprintf(asm_inst + len, "%sD=M\n%sD=M-D\n", PRE_POP, PRE_POP);
	len += gen_jmp_log_asm(asm_inst + len, jmp_cmd);
	return len;
}

static size_t gen_eq_asm(char *asm_inst)
{
	return gen_binary_logical_asm(asm_inst, EQ);
}

static size_t gen_lt_asm(char *asm_inst)
{
	return gen_binary_logical_asm(asm_inst, LT);
}

static size_t gen_gt_asm(char *asm_inst)
{
	return gen_binary_logical_asm(asm_inst, GT);
}

static size_t gen_push_asm(char *asm_inst, SegmentBase seg_base, uint16_t idx)
{
	return sprintf(asm_inst, "@%hu\nD=M\n%s", seg_base + idx, PUSH_ASM);
}

static size_t gen_pop_asm(char *asm_inst, SegmentBase seg_base, uint16_t idx)
{
	size_t inst_len;
	if(seg_base == CONSTANT)
		inst_len =sprintf(asm_inst, "%s", PRE_POP);
	else
		inst_len = sprintf(asm_inst, "%sD=M\n@%hu\nM=D\n", PRE_POP, seg_base + idx);
	
	return inst_len;
}

static size_t gen_push_lcl_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, LCL, idx);
}

static size_t gen_pop_lcl_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, LCL, idx);
}

static size_t gen_push_arg_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, ARG, idx);
}

static size_t gen_pop_arg_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, ARG, idx);
}

static size_t gen_push_static_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, STATIC, idx);
}

static size_t gen_pop_static_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, STATIC, idx);
}

static size_t gen_push_ptr_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, POINTER, idx);
}

static size_t gen_pop_ptr_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, POINTER, idx);
}

static size_t gen_push_this_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, THIS, idx);
}

static size_t gen_pop_this_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, THIS, idx);
}

static size_t gen_push_that_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, THAT, idx);
}

static size_t gen_pop_that_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, THAT, idx);
}

static size_t gen_push_temp_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, TEMP, idx);
}

static size_t gen_pop_temp_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, TEMP, idx);
}

static size_t gen_push_const_asm(char *asm_inst, uint16_t idx)
{
	return gen_push_asm(asm_inst, CONSTANT, idx);
}

static size_t gen_pop_const_asm(char *asm_inst, uint16_t idx)
{
	return gen_pop_asm(asm_inst, CONSTANT, 0);
}
