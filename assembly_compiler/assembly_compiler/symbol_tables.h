#include"intermediate_code_generator.h"



char ** divide_into_tokens(char *command, int *no_of_tokens)
{
	char **tokens = (char**)malloc(sizeof(char) * 6);
	for (int index = 0; index < 6; index++)
		tokens[index] = (char*)malloc(sizeof(char) * 100);
	int token_index = 0;
	int token_point = 0;
	for (int index = 0; command[index] != '\0'; index++)
	{
		if (command[index] == 32)
		{
			tokens[token_index][token_point] = '\0';
			token_index++;
			token_point = 0;
		}
		else
		{
			tokens[token_index][token_point++] = command[index];
		}
	}
	tokens[token_index][token_point] = '\0';
	*no_of_tokens = token_index + 1;
	return tokens;
}





struct Metadata* store_variable_in_memorytable(char *variable,struct Metadata *metadata,int *offset)
{
	if (strlen(variable) == 1)
	{
		strcpy(metadata->symbol_table[variable[0] - 65].name, variable);
		metadata->symbol_table[variable[0] - 65].address = *offset;
		metadata->symbol_table[variable[0] - 65].size = 1;
		*offset+=1;
	}
	else
	{
		int range = 0;
		for (int index = 2; index < strlen(variable) - 1; index++)
			range = range * 10 + (variable[index] - 48);
		variable[1] = '\0';
		strcpy(metadata->symbol_table[variable[0] - 65].name, variable);
		metadata->symbol_table[variable[0] - 65].address = *offset;
		metadata->symbol_table[variable[0] - 65].size = range;
		*offset = *offset + range;
	}
	return metadata;
}




struct Metadata* store_const_in_memorytable(char *token,struct Metadata *metadata,int *offset)
{
	int variable=token[0] - 65;
	int value = 0;
	for (int index = 2; token[index] != '\0'; index++)
		value = value * 10 + (token[index] - 48);
	token[1] = '\0';
	char char_value = value;
	strcpy(metadata->symbol_table[variable].name,token);
	metadata->symbol_table[variable].address = *offset;
	metadata->symbol_table[variable].size = 0;
	metadata->memory_blocks[*offset] = char_value;
	*offset = *offset + 1;
	return metadata;
}




struct Metadata* compile_and_parse_actual_code(struct Intermediate_code **intermediate_code,struct Metadata *metadata,FILE *fstream,int *end_flag)
{
	char *command = (char*)malloc(sizeof(char) * 100);
	char **tokens = (char**)malloc(sizeof(char*) * 10);
	for (int index = 0; index < 10; index++)
		tokens[index] = (char*)malloc(sizeof(char) * 10);
	int *stack = (int*)malloc(sizeof(int) * 100);
	int top = -1;
	int no_of_tokens = 0;
	int instruction_count = 1;
	int no_of_blocks = 0;
	int code;
	while (1)
	{
		fscanf(fstream, "%[^\n]s", command);
		fscanf(fstream, "\n");
		tokens = divide_into_tokens(command, &no_of_tokens);
		if (!strcmp(tokens[0], (char*)"END"))
		{
			*end_flag = 1;
			metadata->instruction_count = instruction_count;
			break;
		}
		code = get_opcode_number(tokens[0]);
		if (code == 1)
			metadata=generate_intermediate_code_for_MOV(metadata,intermediate_code,&instruction_count,tokens[1]);
		else if (code == 3 || code == 4 || code == 5)
			metadata = generate_intermediate_code_for_ADD_MUL_SUB(metadata, intermediate_code, &instruction_count, tokens[1], code);
		else if (code == 13)
			metadata = generate_intermediate_code_for_PRINT(metadata, intermediate_code, &instruction_count, tokens[1]);
		else if (code == 14)
			metadata = generate_intermediate_code_for_READ(metadata, intermediate_code, &instruction_count, tokens[1]);
		else if (code == 7)
			metadata = generate_intermediate_code_for_IF(metadata, intermediate_code, &instruction_count, tokens,stack,&top);
		else if (code == 6)
			metadata = generate_intermediate_code_for_JUMP(metadata, intermediate_code, &instruction_count,tokens[1],&no_of_blocks);
		else if (no_of_tokens == 1 && !strcmp(tokens[0], (char*)"ELSE"))
			metadata = generate_intermediate_code_for_ELSE(metadata, intermediate_code, &instruction_count,&no_of_blocks,stack,&top);
		else if (no_of_tokens == 1 && !strcmp(tokens[0], (char*)"ENDIF"))
			metadata = generate_intermediate_code_for_ENDIF(metadata, intermediate_code, &instruction_count,&no_of_blocks,tokens[0],stack,&top);
		else
			metadata = generate_intermediate_code_for_LABEL(metadata, intermediate_code, &instruction_count,tokens[0],&no_of_blocks);
	}
	return metadata;
}




struct Metadata* compile_the_code(struct Intermediate_code **intermediate_code, char *command, char **tokens, int *no_of_tokens, struct Metadata *metadata, int *offset)
{
	int end_flag=0;
	printf("ENTER THE FILE NAME : ");
	gets(command);
	FILE *fstream = fopen(command,"r+");
	if (fstream)
	{
		while (1)
		{
			fscanf(fstream,"%[^\n]s",command);
			if (end_flag)
				break;
			fscanf(fstream, "\n");
			tokens = divide_into_tokens(command, no_of_tokens);
			if (*no_of_tokens == 1 && !strcmp(tokens[0], "START:"))
				metadata = compile_and_parse_actual_code(intermediate_code,metadata,fstream,&end_flag);
			if (*no_of_tokens == 2 && !strcmp(tokens[0], "DATA"))
				metadata = store_variable_in_memorytable(tokens[1], metadata, offset);
			if (*no_of_tokens == 2 && !strcmp(tokens[0], "CONST"))
				metadata = store_const_in_memorytable(tokens[1], metadata, offset);
		}
		fclose(fstream);
	}
	else
	{
		printf("FILE NOT OPENED\n");
		return NULL;
	}
	return metadata;
}



void generate_the_intermediate_file(struct Metadata *metadata, struct Intermediate_code **intermediate_code)
{
	char *buffer = (char*)malloc(sizeof(char)*(4 * 1024));
	if (metadata)
	{
		memcpy(buffer, metadata, sizeof(struct Metadata));
		FILE *fstream = fopen("output.txt", "wb");
		if (fstream)
		{
			if (!fwrite(buffer, 1, 4 * 1024, fstream))
				printf("FILE NOT READ\n");
			if (!fseek(fstream, 4 * 1024, SEEK_SET))
			{
				for (int instruction_count = 1; instruction_count <= metadata->instruction_count; instruction_count++)
				{
					if (!fwrite(intermediate_code[instruction_count], 1, sizeof(struct Intermediate_code), fstream))
						printf("FILE NOT READ\n");
				}
				printf("compilation completed\n");
				fclose(fstream);
			}
			else
				printf("SEEKING UNSUCCESFULL\n");
		}
		else
			printf("FILE NOT OPENED\n");
	}
}