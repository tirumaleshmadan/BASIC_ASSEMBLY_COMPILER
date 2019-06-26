#include"symbol_tables.h"

int main()
{
	char *command = (char*)malloc(sizeof(char) * 100);
	char **tokens = (char**)malloc(sizeof(char*) * 10);
	for (int index = 0; index < 10; index++)
		tokens[index] = (char*)malloc(sizeof(char) * 10);
	int no_of_tokens = 0;
	struct Intermediate_code **intermediate_code = (struct Intermediate_code**)malloc(sizeof(Intermediate_code*) * 100);
	for (int index = 0; index < 100; index++)
		intermediate_code[index] = (struct Intermediate_code*)malloc(sizeof(Intermediate_code));
	struct Metadata *metadata = (struct Metadata*)malloc(sizeof(struct Metadata));
	int offset = 0;
	metadata=compile_the_code(intermediate_code,command,tokens,&no_of_tokens,metadata,&offset);
	generate_the_intermediate_file(metadata,intermediate_code);
	_getch();
	return 0;
}
