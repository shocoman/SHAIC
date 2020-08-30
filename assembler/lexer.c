#define INPUT_LINE_SIZE 64

char g_line[INPUT_LINE_SIZE];
int g_chars_read = 0;
int g_newlines_read = 0;
FILE* g_asm_file;

typedef enum { 
	T_NONE,
	T_EOF,
	T_ID,
	T_AT,
	T_EQUALS,
	T_SEMICOLON,
	T_NEWLINE,
	T_LPAREN,
	T_RPAREN,
	T_MINUS,
	T_PLUS,
	T_NOT,
	T_OR,
	T_AND,
	T_ERR
} Token;



int is_correct_char_for_id(char ch) {
	return isalnum(ch) || ch == '_' || ch == '.' || ch == '$';
}

Token read_token() {
	static char current_char = ' ';
	
	// SKIP SPACES
	while (current_char == ' ') {
		current_char = fgetc(g_asm_file);
	}
	
	// SKIP COMMENTED LINES (START WITH '#' SYMBOL)
	if (current_char == '/') {
		while (current_char != '\n' && current_char != EOF) 
			current_char = fgetc(g_asm_file);
	}

	// READ IDENTIFIER
	g_chars_read = 0;
	if (is_correct_char_for_id(current_char)) {
		while (is_correct_char_for_id(current_char)) {
			g_line[g_chars_read] = current_char;
			current_char = fgetc(g_asm_file);
			g_chars_read++;
		}
		g_line[g_chars_read] = '\0';
		return T_ID;
	}

	// READ ONE-CHAR OPERATOR
	Token t;
	switch (current_char) {
		case '@' : t = T_AT; break;
		case '=' : t = T_EQUALS; break;
		case ';' : t = T_SEMICOLON; break;
		case '\n': t = T_NEWLINE; g_newlines_read++; break;
		case '(': t = T_LPAREN; break;
		case ')': t = T_RPAREN; break;
		case '+': t = T_PLUS; break;
		case '-': t = T_MINUS; break;
		case '!': t = T_NOT; break;
		case '&': t = T_AND; break;
		case '|': t = T_OR; break;
		case EOF : t = T_EOF; break;
		default: t = T_ERR; break;
	}
	
	g_line[0] = current_char;
	g_line[1] = '\0';
	g_chars_read = 1;
	current_char = fgetc(g_asm_file);

	return t;
}


int count_non_empty_lines_in_file(const char* g_asm_filename) {
	FILE* f = fopen(g_asm_filename, "r");
	int lines = 0;
	
	int is_empty = 1;
	char ch;
	while ((ch = fgetc(f)) != EOF) {
		// comments
		if (ch == '/')  // skip chars until EOF or \n
			while (ch != '\n' && ch != EOF) 
				ch = fgetc(f);

		if (ch != '\n') is_empty = 0;
		if (!is_empty && ch == '\n') {
			lines++;
			is_empty = 1;
		}
	}
	if (!is_empty) lines++;
	
	fclose(f);
	
	return lines;
}

void test_token_function() {
	Token t;
	do {
		t = read_token();
		if (t == T_ID) {
			printf("Token is ID: %s\n", g_line);
		} else if (t == T_ERR) {
			printf("Error token: %s\n", g_line);
		} else if (t == T_EOF) {
			printf("Token is EOF \n", g_line);
		} else if (t == T_NEWLINE) {
			printf("Token is NEW LINE \n", g_line);
		} else {
			printf("Token is operator: %s\n", g_line);
		}
 	} while (t != T_EOF && t != T_ERR);
} 


