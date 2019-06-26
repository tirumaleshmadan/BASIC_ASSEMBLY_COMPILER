#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#pragma once

struct Symbol_table
{
	char name[10];
	int address;
	int size;
};

struct Block_table
{
	char block_name[20];
	int address;
};

struct Intermediate_code
{
	int array[5];
};

struct Metadata
{
	int instruction_count;
	int no_of_blocks;
	char memory_blocks[100];
	struct Symbol_table symbol_table[5];
	struct Block_table block_table[50];
};

int search_for_block_address(struct Metadata *metadata,char *token,int *no_of_blocks)
{
	for (int block = 0; block < *no_of_blocks; block++)
	{
		if (!strcmp(metadata->block_table[block].block_name, token))
			return metadata->block_table[block].address;
	}
	return 0;
}

int get_opcode_number(char *instruction)
{
	if (!strcmp(instruction, (char*)"MOV"))
		return 1;
	if (!strcmp(instruction, (char*)"ADD"))
		return 3;
	if (!strcmp(instruction, (char*)"SUB"))
		return 4;
	if (!strcmp(instruction, (char*)"MUL"))
		return 5;
	if (!strcmp(instruction, (char*)"JUMP"))
		return 6;
	if (!strcmp(instruction, (char*)"IF"))
		return 7;
	if (!strcmp(instruction, (char*)"EQ"))
		return 8;
	if (!strcmp(instruction, (char*)"LT"))
		return 9;
	if (!strcmp(instruction, (char*)"GT"))
		return 10;
	if (!strcmp(instruction, (char*)"LTEQ"))
		return 11;
	if (!strcmp(instruction, (char*)"GTEQ"))
		return 12;
	if (!strcmp(instruction, (char*)"PRINT"))
		return 13;
	if (!strcmp(instruction, (char*)"READ"))
		return 14;
	return 0;
}

struct Metadata* generate_intermediate_code_for_MOV(struct Metadata *metadata,struct Intermediate_code **intermediate_code, int *instruction_count, char *token)
{
	if (strlen(token) == 4)
	{
		if (token[1] == ',')
		{
			intermediate_code[*instruction_count]->array[0] = 1;
			intermediate_code[*instruction_count]->array[1] = metadata->symbol_table[token[0] - 65].address;
			intermediate_code[*instruction_count]->array[2] = token[2] - 65;
		}
		else
		{
			intermediate_code[*instruction_count]->array[0] = 2;
			intermediate_code[*instruction_count]->array[1] = token[0] - 65;
			intermediate_code[*instruction_count]->array[2] = metadata->symbol_table[token[3] - 65].address;
		}
	}
	else
	{
		if (token[2] == ',')
		{
			int range = 0;
			for (int index = 5; index < strlen(token) - 1; index++)
				range = range * 10 + (token[index] - 48);
			intermediate_code[*instruction_count]->array[0] = 2;
			intermediate_code[*instruction_count]->array[2] = token[0] - 65;
			intermediate_code[*instruction_count]->array[1] = metadata->symbol_table[token[3] - 65].address+range;
		}
		else
		{
			int range = 0;
			for (int index = 2; index < strlen(token) - 4; index++)
				range = range * 10 + (token[index] - 48);
			intermediate_code[*instruction_count]->array[0] = 1;
			intermediate_code[*instruction_count]->array[1] = metadata->symbol_table[token[0] - 65].address + range;
			intermediate_code[*instruction_count]->array[2] = token[strlen(token)-2] - 65;
		}
	}
	*instruction_count += 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_ADD_MUL_SUB(struct Metadata *metadata, 
	struct Intermediate_code **intermediate_code, int *instruction_count, char *token,int code)
{
	intermediate_code[*instruction_count]->array[0] = code;
	intermediate_code[*instruction_count]->array[1] = token[0] - 65;
	intermediate_code[*instruction_count]->array[2] = token[3] - 65;
	intermediate_code[*instruction_count]->array[3] = token[6] - 65;
	*instruction_count += 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_PRINT(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count, char *token)
{
	intermediate_code[*instruction_count]->array[0] = 13;
	if (strlen(token) == 1)
		intermediate_code[*instruction_count]->array[1] = metadata->symbol_table[token[0]-65].address;
	else
	{
		int range = 0;
		for (int index = 2; index < strlen(token) - 1; index++)
			range = range * 10 + (token[index] - 48);
		intermediate_code[*instruction_count]->array[1] = metadata->symbol_table[token[0] - 65].address+range;
	}
	*instruction_count += 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_READ(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count, char *token)
{
	intermediate_code[*instruction_count]->array[0] = 14;
	intermediate_code[*instruction_count]->array[1] = token[0] - 65;
	*instruction_count += 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_JUMP(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count,char *token,int *no_of_blocks)
{
	intermediate_code[*instruction_count]->array[0] = 6;
	intermediate_code[*instruction_count]->array[1] = search_for_block_address(metadata,token,no_of_blocks);
	*instruction_count = *instruction_count + 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_IF(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count,char **tokens,int *stack,int *top)
{
	intermediate_code[*instruction_count]->array[0] = 7;
	intermediate_code[*instruction_count]->array[1] = tokens[1][0] - 65;
	intermediate_code[*instruction_count]->array[2] = tokens[3][0] - 65;
	intermediate_code[*instruction_count]->array[3] = get_opcode_number(tokens[2]);
	intermediate_code[*instruction_count]->array[4] = 0;
	*top = *top + 1;
	stack[*top] = *instruction_count;
	*instruction_count += 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_ELSE(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count,int *no_of_blocks,int *stack,int*top)
{
	intermediate_code[*instruction_count]->array[0] = get_opcode_number((char*)"JUMP");
	intermediate_code[*instruction_count]->array[1] = 0;
	*top = *top + 1;
	stack[*top] = *instruction_count;
	*instruction_count += 1;
	strcpy(metadata->block_table[*no_of_blocks].block_name, (char*)"ELSE");
	metadata->block_table[*no_of_blocks].address = *instruction_count;
	*no_of_blocks = *no_of_blocks + 1;
	return metadata;
}

struct Metadata* generate_intermediate_code_for_ENDIF(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count,int *no_of_blocks,char *token,int *stack,int *top)
{
	strcpy(metadata->block_table[*no_of_blocks].block_name, token);
	metadata->block_table[*no_of_blocks].address = *instruction_count;
	*no_of_blocks = *no_of_blocks + 1;
	int position = *instruction_count;
	while (*top >= 0)
	{
		int level = stack[*top];
		if (intermediate_code[level]->array[0] == 6)
		{
			intermediate_code[level]->array[1] = position;
			position = level + 1;
			*top = *top - 1;
		}
		else if (intermediate_code[level]->array[0] == 7)
		{
			intermediate_code[level]->array[4] = position;
			*top = *top - 1;
			break;
		}
	}
	return metadata;
}

struct Metadata* generate_intermediate_code_for_LABEL(struct Metadata *metadata, struct Intermediate_code **intermediate_code, int *instruction_count,char *token,int *no_of_blocks)
{
	token[strlen(token) - 1] = '\0';
	strcpy(metadata->block_table[*no_of_blocks].block_name, token);
	metadata->block_table[*no_of_blocks].address = *instruction_count;
	*no_of_blocks = *no_of_blocks + 1;
	return metadata;
}

