#include "utils.c"
#include "main_generator.c"

enum {MAX_STACK = 1024};

int32_t stack[MAX_STACK];
int32_t *top = stack;

inline void push(int32_t val)
{
	*top++ = val;
}

inline int32_t pop()
{
	return *--top; 
}

inline void pushes(int n)
{
	assert(top + n <= stack + MAX_STACK);
}

inline void pops(int n)
{
	assert(top - n >= stack);
}

void print_stack()
{
	int32_t *p_stack = stack;
	while (p_stack != top)
	{
		printf("%d\n", *p_stack++);
	}
}

typedef enum Opcodes
{
	LIT = 1,
	ADD,
	SUB,
	MUL,
	DIV,
	HALT,
} Opcodes;

const char *opcode_to_name[] =
{
	[LIT] = "LIT",
	[ADD] = "ADD",
	[SUB] = "SUB",
	[MUL] = "MUL",
	[DIV] = "DIV",
	[HALT] = "HALT",
};

void stack_test()
{
	push(32);
	push(0);
	push(64);

	assert(pop() == 64);
	assert(pop() == 0);
	assert(pop() == 32);
}

void vm_exec(const char *code)
{
	int32_t right;
	int32_t left;
	for (; ;)
	{
		uint8_t op = my_atoi(code, code + 8, 2);
		code += 8;
		switch (op)
		{
			case LIT:
				pushes(1);
				push(my_atoi(code, code + 32, 2));
				code += 32;
				break;
			case SUB:
				pops(2);
				right = pop();
				left = pop();
				pushes(1);
				push(left - right);
				break;
			case ADD:
				pops(2);
				right = pop();
				left = pop();
				pushes(1);
				push(left + right);
				break;
			case MUL:
				pops(2);
				right = pop();
				left = pop();
				pushes(1);
				push(left * right);
				break;
			case DIV:
				pops(2);
				right = pop();
				left = pop();
				pushes(1);
				push(left / right);
				break;
			case HALT:
				return;
			default:
				fatal("vm_exec: illegal opcode");
		}
	}
}

int my_atoi(const char *source, const char *end, int base)
{	
	int result = 0;
	switch (base)
	{
		case 10:
			while (source != end)
			{
				result *= 10;
				result += *source++ - '0';
			}
			break;
		case 2:
			while (source != end)
			{
				result *= 2;
				result += *source++ - '0';
			}
			break;
		default:
			fatal("my_atoi: incorrect base");
	}
	
	return result;
}

int bytes_in_file(FILE *f)
{
	int result = 0;
	char ch;
	while ((ch = getc(f)) != EOF)
	{
		result++;
	}
	fseek(f, 0, SEEK_SET);
	return result;
}

char *read_file(const char *filename)
{	
	FILE *f = fopen(filename, "r");
	if (!f)
		fatal("read_file: can't open the file");

	int size = bytes_in_file(f);
	char *stream = malloc(size);
	char *stream_return = stream;
	char ch;
	while ((ch = fgetc(f)) != EOF)
	{
		if (ch == ';')
		{
			while ((ch = fgetc(f)) && ch != '\n' && ch != EOF);
			continue;
		}
		if (ch != '\n' && ch != '\r' && ch != ' ')
			*stream++ = ch;
	}
	*stream = '\0';
	fclose(f);
	return stream_return;
}

#define GEN_CODE(x, y) generate_code(#x, y)

int main(void)
{
	GEN_CODE((1 + -2) * 3 / 2 - 1, "code.txt");

	const char *source = read_file("code.txt");

	stack_test();
	vm_exec(source);
	print_stack();
}
