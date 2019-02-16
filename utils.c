#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

void fatal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("FATAL: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(1);
}

void write_string(FILE *f, const char *str)
{
	size_t len = strlen(str);
	if (!fwrite(str, 1, len, f))
		fatal("Cant write the string");
}

int msb_index(unsigned int val)
{
	int index = 0;
	for (int shift = 0; val; val >>= 1, shift++)
	{	
		if (val & 1)
			index = shift;
	}
	return index;
}

void write_digit(FILE *f, int val) // TODO: sign
{
	int index = msb_index(val);
	int padding = 31 - index - 1;

	char str[32]; // 1 - '\0'
	memset(str, '0', 32);

	for (int i = 31, temp_val = val; i > padding; i--, temp_val >>= 1)
	{
		str[i] = (temp_val & 1) + '0';
	}

	char comment[3 + 10 + 1 + 1]; // 3 - " ; ", 10 - "xxxxxxxxxx", 1 - '\n', 1 - '\0'
	sprintf(comment, " ; %d\n", val);

	if (!fwrite(str, 1, 32, f))
		fatal("Cant write the string");
	if (!fwrite(comment, 1, strlen(comment), f))
		fatal("Cant write the string");
}
