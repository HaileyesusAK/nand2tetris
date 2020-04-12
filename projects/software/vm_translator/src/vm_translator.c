#define _GNU_SOURCE
#include <ctype.h>
#include <search.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "vm_translator.h"

#define CMD_LEN				9
#define SEGMENT_LEN			9
#define VM_LINE_LEN			256

typedef enum {
	POINTER = 3, //RAM location 3, coincides with THIS
	TEMP = 5,	//RAM location 5
	LCL,
	ARG,
	THIS,
	THAT,
	CONSTANT,
	STATIC
}SegmentBase;

typedef enum {
	BLANK_CODE = 0, 
	PUSH_CODE = 448,
	POP_CODE = 335,
	FUNC_CODE = 870,
	CALL_CODE = 412,
	RET_CODE = 672,
	LABEL_CODE = 512,
	GOTO_CODE = 441,
	IF_GOTO_CODE = 693,
	ALU_CODE
}InstCode;

typedef struct  {
	size_t n_cmds;
	char **cmds;
	struct hsearch_data* generator_mapping;
	char *static_prefix;
}VmTranslator;

typedef size_t Generator(FILE *, ...);


static char* static_prefix;
static char current_function[VM_LINE_LEN];

static size_t parse_word(const char* src, char* dst, size_t n_dst);
static int parse_vm_inst(const char* vm_inst, char *cmd, InstCode* inst_code,
						 char *arg1, uint16_t* arg2);
static size_t get_inst_code(const char *cmd);
static int translate_vm_inst(const VmTranslator* vm_translator,
							 const char* vm_inst, FILE* asm_inst,
							 InstCode* inst_code);

static size_t gen_add_asm(FILE *asm_file);
static size_t gen_and_asm(FILE *asm_file);
static size_t gen_eq_asm(FILE *asm_file);
static size_t gen_gt_asm(FILE *asm_file);
static size_t gen_lt_asm(FILE *asm_file);
static size_t gen_neg_asm(FILE *asm_file);
static size_t gen_not_asm(FILE *asm_file);
static size_t gen_or_asm(FILE *asm_file);
static size_t gen_sub_asm(FILE *asm_file);
static size_t gen_binary_rel_asm(FILE *asm_file, char op);
static size_t gen_binary_alu_op_asm(FILE *asm_file, char op);

static size_t gen_pop_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_lcl_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_lcl_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_arg_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_arg_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_static_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_static_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_ptr_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_ptr_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_this_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_this_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_that_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_that_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_temp_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_temp_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_const_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_const_asm(FILE *asm_file, uint16_t idx);

static size_t gen_func_asm(FILE *asm_file, char *f_name, uint16_t n_locals);
static size_t gen_call_asm(FILE *asm_file, char *f_name, uint16_t n_args);
static size_t gen_ret_asm(FILE *asm_file);

void *init_vm_translator(char *prefix)
{

	if(!prefix)
	{
		fprintf(stderr, "NULL input pointer at %s, line %d\n", __FILE__, __LINE__);
		return NULL;
	}

	VmTranslator* vm_translator = calloc(1, sizeof(VmTranslator));
	if(!vm_translator)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		return NULL;
	}
	
	vm_translator->generator_mapping = calloc(1, sizeof(struct hsearch_data));
	if(!vm_translator->generator_mapping)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}

	vm_translator->static_prefix = strdup(prefix);
	static_prefix = vm_translator->static_prefix;

	if(!vm_translator->static_prefix)
	{
		fprintf(stderr, "strdup failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}
	
	struct CmdGen{
		char *cmd;
		void *generator;
	} cmd_generators[] = { 
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
		{"function", gen_func_asm}, {"call", gen_call_asm}, {"ret", gen_ret_asm}
	};

	vm_translator->n_cmds = sizeof(cmd_generators) / sizeof(struct CmdGen);	
	vm_translator->cmds = calloc(1, vm_translator->n_cmds * sizeof(char*));
	if(!vm_translator->cmds)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}
	
	for(size_t i = 0; i < vm_translator->n_cmds; ++i)
	{
		vm_translator->cmds[i] = strdup(cmd_generators[i].cmd);
		if(!vm_translator->cmds[i])
		{
			fprintf(stderr, "strdup failed at %s, line %d\n", __FILE__, __LINE__);
			destroy_vm_translator(vm_translator);
			return NULL;
		}
	}
	
	if(!hcreate_r(vm_translator->n_cmds, vm_translator->generator_mapping))
	{
		fprintf(stderr, "hcreate_r failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}

	size_t i = 0; 
	ENTRY item, *entry;
	for(i = 0; i < vm_translator->n_cmds; ++i)
	{
		item.key = vm_translator->cmds[i];
		item.data = cmd_generators[i].generator;
		if(!hsearch_r(item, ENTER, &entry, vm_translator->generator_mapping))
		{
			fprintf(stderr, "hsearch_r failed at %s, line %d\n", __FILE__, __LINE__);
			destroy_vm_translator(vm_translator);
			return NULL;
		}
	}
	
	return vm_translator;
}

void destroy_vm_translator(void* handle)
{
	VmTranslator *vm_translator = (VmTranslator *)handle;
	if(vm_translator)
	{
		if(vm_translator->static_prefix)
			free(vm_translator->static_prefix);
		
		if(vm_translator->cmds)
		{
			for(size_t i = 0; i < vm_translator->n_cmds; ++i)
				if(vm_translator->cmds[i])
					free(vm_translator->cmds[i]);
			free(vm_translator->cmds);
		}
		
		if(vm_translator->generator_mapping)
		{
			hdestroy_r(vm_translator->generator_mapping);
			free(vm_translator->generator_mapping);
		}
		
		free(vm_translator);
	}
}

int translate_vm_inst(const VmTranslator* vm_translator, const char* vm_inst,
					  FILE* asm_inst, InstCode* inst_code)
{
	char cmd[CMD_LEN];
	char segment[SEGMENT_LEN];
	uint16_t idx;
	ENTRY item, *entry;

	parse_vm_inst(vm_inst, cmd, inst_code, segment, &idx);

	switch(*inst_code)
	{
		case PUSH_CODE:
		case POP_CODE:
		{
			char key[CMD_LEN + SEGMENT_LEN + 1];
			snprintf(key, CMD_LEN + SEGMENT_LEN + 1, "%s_%s", cmd, segment);
			item.key = key;
			if(!hsearch_r(item, FIND, &entry, vm_translator->generator_mapping))
			{
				fprintf(stderr, "Cannot translate VM command '%s %s' at %s, line %d\n",
								cmd, segment, __FILE__, __LINE__);
				return -1;
			}	

			Generator *generator = (Generator*)entry->data;
			generator(asm_inst, idx);
		}
		break;
		
		case FUNC_CODE:
		case CALL_CODE:
		{
			item.key = cmd;
			if(!hsearch_r(item, FIND, &entry, vm_translator->generator_mapping))
			{
				fprintf(stderr, "Cannot translate VM command '%s %s' at %s, line %d\n",
								cmd, segment, __FILE__, __LINE__);
				return -1;
			}	

			Generator *generator = (Generator*)entry->data;
			generator(asm_inst, segment, idx);
		}
		break;

		case ALU_CODE:
		case RET_CODE:
		{
			item.key = cmd;
			if(!hsearch_r(item, FIND, &entry, vm_translator->generator_mapping))
			{
				fprintf(stderr, "Cannot translate VM command '%s' at %s, line %d\n",
								cmd, __FILE__, __LINE__);
				return -1;
			}

			Generator *generator = (Generator*)entry->data;
			generator(asm_inst);
		}
		break;

		default:
			break;
	}

	return 0;
}

int translate_vm_file(const void* handle, FILE* vm_file, FILE* asm_file)
{
	if(!handle || !vm_file || !asm_file)
	{
		fprintf(stderr, "NULL input pointer at %s, line %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	InstCode inst_code;
	char vm_inst[VM_LINE_LEN];
	VmTranslator* vm_translator = (VmTranslator*) handle;

	fprintf(asm_file, "\t@BEGIN\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(PUSH_TRUE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=-1\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\tA=M\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(PUSH_FALSE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=0\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\tA=M\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(BEGIN)\n");
	while(fgets(vm_inst, VM_LINE_LEN, vm_file))
	{
		int len = fprintf(asm_file, "//%s", vm_inst);
		int rc = translate_vm_inst(vm_translator, vm_inst, asm_file, &inst_code);
		if(rc)
		{
			fprintf(stderr, "Cannot translate VM instruction '%s' at %s, line %d\n",
							vm_inst, __FILE__, __LINE__);
			return -1;	
		}

		//Overwrite comment if no instruction is translated
		if(inst_code == BLANK_CODE)
			fseek(asm_file, -len, SEEK_CUR); 
		else
			fprintf(asm_file, "\n");
	}

	//End of translation
	fprintf(asm_file, "(END)\n"
					  "\t@END\n"
					  "\t0;JMP\n\n");

	return 0;
}

size_t parse_word(const char* src, char* dst, size_t n_dst)
{
	size_t i = 0;
	const char *c = src;
	
	while(*c && isblank(*c))
		c++;
	
	while(*c && !isblank(*c) && !isspace(*c) && i < n_dst - 1)
		dst[i++] = *c++;

	dst[i] = '\0';

	return i;
}

size_t get_inst_code(const char *cmd)
{
	size_t sum = 0;
	const char *c = cmd;
	while(*c)
		sum += *c++;
	return sum;
}

int parse_vm_inst(const char* vm_inst, char *cmd, InstCode* inst_code,
						 char *arg1, uint16_t* arg2)
{
	const char *c = vm_inst;
	size_t word_len;	
	*inst_code = BLANK_CODE;

	while(*c && (isblank(*c) || isspace(*c)))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	word_len = parse_word(c, cmd, CMD_LEN);

	size_t code = get_inst_code(cmd);
	*inst_code = code;
	switch(code)
	{
		case PUSH_CODE:
		case POP_CODE:
		case FUNC_CODE:
		case CALL_CODE:
			sscanf(c + word_len, "%s %hu", arg1, arg2); break;
		case GOTO_CODE:
		case LABEL_CODE:
		case IF_GOTO_CODE:
			sscanf(c + word_len, "%s", arg1); break;
		case RET_CODE:
			*inst_code = code;
		default:
			*inst_code = ALU_CODE;
	}

	return 0;
}


static size_t gen_binary_alu_op_asm(FILE *asm_file, char op)
{

	fprintf(asm_file,
			"\t@SP\n"
			"\tAM=M-1\n"
			"\tD=M\n"
			"\t@SP\n"
			"\tA=M-1\n");
	
	if(op == '-')
		fprintf(asm_file, "\tM=M-D\n");
	else
		fprintf(asm_file, "\tM=D%cM\n", op);

	return 0;
}

static size_t gen_add_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '+');
}

static size_t gen_sub_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '-');
}

static size_t gen_or_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '|');
}

static size_t gen_and_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '&');
}

static size_t gen_not_asm(FILE *asm_file)
{
	fprintf(asm_file, 
			"\t@SP\n"
			"\tA=M-1\n"
			"\tM=!M\n");
	
	return 0;
}

static size_t gen_neg_asm(FILE *asm_file)
{
	fprintf(asm_file,
		    "\t@SP\n"
		    "\tA=M-1\n"
		    "\tM=-M\n");
	
	return 0;
}

static size_t gen_binary_rel_asm(FILE *asm_file, char op)
{
	char *jmp_cmd;
	static size_t i = 1;	
	switch(op)
	{
		case '=': jmp_cmd = "JEQ"; break;
		case '<': jmp_cmd = "JLT"; break;
		case '>': jmp_cmd = "JGT"; break;
	}

	char return_label[VM_LINE_LEN];
	snprintf(return_label, VM_LINE_LEN, "%zu_op_end", i++);

	//Save return address on R13
	fprintf(asm_file, "\t@%s\n"
				      "\tD=A\n"
				      "\t@R13\n"
				      "\tM=D\n",
				      return_label);

	//Compute x - y and put the result on D
	fprintf(asm_file, "\t@SP\n"
					  "\tAM=M-1\n"
					  "\tD=M\n"
					  "\t@SP\n"
					  "\tAM=M-1\n"
					  "\tD=M-D\n");
	
	fprintf(asm_file, "\t@PUSH_TRUE\n"
					  "\tD;%s\n"
					  "\t@PUSH_FALSE\n"
					  "\t0;JMP\n",
					  jmp_cmd);
	
	fprintf(asm_file, "(%s)\n", return_label);	

	return 0;
}

static size_t gen_eq_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '=');
}

static size_t gen_lt_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '<');
}

static size_t gen_gt_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '>');
}

static size_t gen_push_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx)
{
	char *push_D = "@SP\n"
				   "\tA=M\n"
				   "\tM=D\n"
				   "\t@SP\n"
				   "\tM=M+1";
					
	if(seg_base == CONSTANT)
	{
		fprintf(asm_file,
				"\t@%hu\n"
				"\tD=A\n"
				"\t%s\n", idx, push_D);
	}
	else if(seg_base == STATIC)
	{
		fprintf(asm_file,
			    "\t@%s.%hu\n"
			    "\tD=M\n"
			    "\t%s\n", static_prefix, idx, push_D);
	}
	else
	{
		char *symbol;
		switch(seg_base)
		{
			case LCL: symbol = "LCL"; break;
			case ARG: symbol = "ARG"; break;
			case THIS: symbol = "THIS"; break;
			case THAT: symbol = "THAT"; break;
			default: break;
		}
		
		switch(seg_base)
		{
			case LCL:
			case ARG:
			case THIS:
			case THAT:
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tA=D+M\n"
						"\tD=M\n"
						"\t%s\n",
						idx, symbol, push_D);
				break;
			case POINTER:
			case TEMP:
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=M\n"
						"\t%s\n", seg_base + idx, push_D);
				break;
			default:
				break;
		}
	}
	return 0;
}

static size_t gen_pop_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx)
{
	char *pop_D = "@SP\n"
				  "\tAM=M-1\n"
				  "\tD=M";

	if(seg_base == STATIC)
	{
		fprintf(asm_file,
				"\t%s\n"
				"\t@%s.%hu\n"
				"\tM=D\n",
				pop_D, static_prefix, idx);
	}
	else
	{
	
		char *symbol;
		switch(seg_base)
		{
			case LCL: symbol = "LCL"; break;
			case ARG: symbol = "ARG"; break;
			case THIS: symbol = "THIS"; break;
			case THAT: symbol = "THAT"; break;
			default: break;
		}

		switch(seg_base)
		{
			case LCL:
			case ARG:
			case THIS:
			case THAT:
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tM=D+M\n"
						"\t%s\n"
						"\t@%s\n"
						"\tA=M\n"
						"\tM=D\n"
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tM=M-D\n",
						idx, symbol, pop_D, symbol, idx, symbol);
				break;
			case TEMP:
			case POINTER:
				fprintf(asm_file,
						"\t%s\n"
						"\t@%hu\n"
						"\tM=D\n", pop_D, seg_base + idx);
				break;
			default:
				break;
		}
	}
	
	return 0;
}

static size_t gen_push_lcl_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, LCL, idx);
}

static size_t gen_pop_lcl_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, LCL, idx);
}

static size_t gen_push_arg_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, ARG, idx);
}

static size_t gen_pop_arg_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, ARG, idx);
}

static size_t gen_push_static_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, STATIC, idx);
}

static size_t gen_pop_static_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, STATIC, idx);
}

static size_t gen_push_ptr_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, POINTER, idx);
}

static size_t gen_pop_ptr_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, POINTER, idx);
}

static size_t gen_push_this_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, THIS, idx);
}

static size_t gen_pop_this_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, THIS, idx);
}

static size_t gen_push_that_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, THAT, idx);
}

static size_t gen_pop_that_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, THAT, idx);
}

static size_t gen_push_temp_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, TEMP, idx);
}

static size_t gen_pop_temp_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, TEMP, idx);
}

static size_t gen_push_const_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, CONSTANT, idx);
}

static size_t gen_pop_const_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, CONSTANT, 0);
}

static size_t gen_func_asm(FILE *asm_file, char *f_name, uint16_t n_locals)
{
	/*
		create a label (f_name)
		push constant 0 n_locals times
	*/

	char loop_beg[VM_LINE_LEN], loop_end[VM_LINE_LEN];

	snprintf(loop_beg, VM_LINE_LEN, "%s_SET_LCL", f_name);
	snprintf(loop_end, VM_LINE_LEN, "%s_SET_LCL_END", f_name);

	//Label function's starting point
	fprintf(asm_file, "(%s)\n", f_name);

	//Setup loop variables
	fprintf(asm_file, "\t@i\n"
					  "\tM=0\n"
					  "\t@%hu\n"
					  "\tD=A\n"
					  "\t@n\n"
					  "\tM=D\n",
			n_locals);
	
	fprintf(asm_file, "(%s)\n", loop_beg);
	
	//Check for loop termination condition
	fprintf(asm_file, "\t@n\n"
					  "\tD=M\n"
					  "\t@i\n"
					  "\tD=D-M\n"
					  "\t@%s\n"
					  "\tD;JEQ\n",
			loop_end);
			
	//Push 0 onto the stack	
	fprintf(asm_file, "\t@SP\n"
					  "\tA=M\n"
					  "\tM=0\n"
					  "\t@SP\n"
					  "\tM=M+1\n");
	
	//Increment loop variable and goto the beginning
	fprintf(asm_file, "\t@i\n"
					  "\tM=M+1\n"
					  "\t@%s\n"
					  "\t0;JMP\n",
			loop_beg);

	fprintf(asm_file, "(%s)\n",loop_end);
	return 0;
}

static size_t gen_call_asm(FILE *asm_file, char *f_name, uint16_t n_args)
{
	/*
		*SP++ = (return_address)
		*SP++ = *LCL
		*SP++ = *ARG
		*SP++ = *THIS
		*SP++ = *THAT
		*ARGS = *SP - n_locals - 5
		*LCL = *SP
		goto f_name
		(return_address)
	*/

	char ret_addr_label[VM_LINE_LEN];

	char *push_D = "@SP\n"
				   "\tA=M\n"
				   "\tM=D\n"
				   "\t@SP\n"
				   "\tM=M+1";

	//Save the called function name since it will be used to prefix labels
	//referenced in its lifetime
	snprintf(current_function, VM_LINE_LEN, "%s", f_name);

	//save return address
	snprintf(ret_addr_label, VM_LINE_LEN, "END_%s", f_name);
	fprintf(asm_file, "@%s\n"
					  "D=A\n"
					  "\t%s\n", ret_addr_label, push_D);
	
	//Save caller's state
	char *segments[] = {"LCL", "ARG", "THIS", "THAT"}; //Don't reorder the elements
	for(size_t i = 0; i < sizeof(segments) / sizeof(char*); ++i)
	{
		fprintf(asm_file, "\t@%s\n"
						  "\tD=M\n"
					      "\t%s\n", segments[i], push_D);
	}

	//setup ARG for the callee
	fprintf(asm_file, "\t@5\n"
					  "\tD=A\n"
					  "\t@%hu\n"
					  "\tD=D+A\n"
					  "\t@SP\n"
					  "\tD=M-D\n"
					  "\t@ARG\n"
					  "\tM=D\n",
			n_args);

	//jump to the called function
	fprintf(asm_file, "\t@%s\n"
					  "\t0;JMP\n",
			f_name);

	//Label end of function
	fprintf(asm_file, "(%s)\n", ret_addr_label);

	return 0;
}

static size_t gen_ret_asm(FILE *asm_file)
{
	/*
		startFrame = ARG
		endFrame = LCL
		
		THAT = --endFrame
		THIS = --endFrame
		ARG = --endFrame
		LCL = --endFrame

		*startFrame = *--SP
		goto endFrame
	*/

	//Save current function's ARG in a variable
	fprintf(asm_file, "@ARG\n"
					  "D=M\n"
					  "@startFrame\n"
					  "M=D\n");
	
	//Save current function's LCL in a variable
	fprintf(asm_file, "@LCL\n"
					  "D=M\n"
					  "@endFrame\n"
					  "M=D\n");

	//Reset caller's state
	char *segments[] = {"THAT", "THIS", "ARG", "LCL"}; //Don't reorder the elements
	for(size_t i = 0; i < sizeof(segments) / sizeof(char*); ++i)
	{
		fprintf(asm_file, "\t@endFrame\n"
						  "\tDM=M-1\n"
						  "\t@%s\n"
						  "\tM=D\n", segments[i]);
	}

	//Return value to the caller
	fprintf(asm_file, "\t@SP\n"
					  "\tAM=M-1\n"
					  "\tD=M\n"
					  "@startFrame\n"
					  "M=D\n");
	
	//Return control to the caller
	fprintf(asm_file, "\t@endFrame\n"
					  "\tA=M-1\n"
					  "\t0; JMP\n");

	return 0;
}
