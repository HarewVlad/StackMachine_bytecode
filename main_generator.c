// expr3 = INT | '(' expr ')'
// expr2 = [-]expr3
// expr1 = expr2 ([/*]expr2)*
// expr0 = expr1 ([+-]expr1)*
// expr = expr0

typedef enum TokenKind
{
	TOKEN_INT,
	TOKEN_CHAR,
} TokenKind;

typedef struct Token
{
	TokenKind kind;
	union
	{
		int val;
		char ch;
	};

} Token;

Token token;
const char *stream;
FILE *output;

void next_token()
{
	repeat:
	switch (*stream)
	{
		case ' ':
			stream++;
			goto repeat;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			int value = 0;
			while (isdigit(*stream))
			{
				value *= 10;
				value += *stream++ - '0';
			}
			token.val = value;
			token.kind = TOKEN_INT;
			break;
		default:
			token.kind = *stream++;
			break;
	}
}

inline bool is_token(TokenKind kind)
{
	return token.kind == kind;
}

inline bool match_token(TokenKind kind)
{
	if (is_token(kind))
	{
		next_token();
		return true;
	}
	else
	{
		return false;
	}
}

inline bool expect_token(TokenKind kind)
{
	if (is_token(kind))
	{
		next_token();
		return true;
	}
	else
		fatal("Unexpected token %s", kind);
}

int parse_expr();

int parse_expr3()
{
	if (is_token(TOKEN_INT))
	{
		int val = token.val;
		write_string(output, "00000001 ; LIT\n");
		write_digit(output, val);
		next_token();
		return val;
	}
	else if (match_token('('))
	{
		int val = parse_expr();
		expect_token(')');
		return val;
	}
	else
	{
		fatal("expected integer or (, got %s", token.kind);
		return 0;
	}
}

int parse_expr2()
{
	if (match_token('~'))
	{
		return -parse_expr3();
	}
	else
		return parse_expr3();
}

int parse_expr1()
{
	int val = parse_expr2();
	while (is_token('/') || is_token('*'))
	{
		char op = token.kind;
		next_token();
		int rval = parse_expr2();

		if (op == '/')
		{
			write_string(output, "00000101 ; DIV\n");
			assert(rval != 0);
			val /= rval;
		}
		else if (op == '*')
		{
			write_string(output, "00000100 ; MUL\n");
			val *= rval;
		}
	}
	return val;
}

int parse_expr0()
{
	int val = parse_expr1();
	while (is_token('+') || is_token('-'))
	{
		char op = token.kind;
		next_token();
		int rval = parse_expr1();
		if (op == '+')
		{
			write_string(output, "00000010 ; ADD\n");
			val += rval;
		}
		else if (op == '-')
		{
			write_string(output, "00000011 ; SUB\n");
			val -= rval;
		}
	}
	return val;
}

int parse_expr()
{
	return parse_expr0();
}

void init_stream(const char *source)
{
	stream = source;
	next_token();
}

void generate_code(const char *source, const char *path)
{
	output = fopen(path, "w+");
	if (!output)
		fatal("Cant open the file");

	init_stream(source);
	parse_expr();
	write_string(output, "00000110 ; HALT\n");
	fclose(output);
}
