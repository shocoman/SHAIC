#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.c"
#include "statement.h"
#include "statement_printer.c"


typedef enum A {
  WRONG_TOKEN_ERR = -1,
  WRONG_JUMP_TYPE_ERR = -2,
  STATEMENT_PARSED = 0,
  ALL_STATEMENTS_PARSED = 1,
} ParsingResult;


Token g_token = T_NONE;

void print_error(ParsingResult err, const char* expected) {
	if (err == WRONG_TOKEN_ERR) {
		printf("Wrong token [%s]; ", g_line[0] == '\n' ? "\\n" : g_line);
	} else if (err == WRONG_JUMP_TYPE_ERR) {
		printf("Wrong jump type [%s]; ", g_line);
	}
	printf(" Line: %d; [%s] expected! \n", g_newlines_read, expected);
}

int read_next_token() {
	return (g_token = read_token());
}

int token_is(Token t) {
	return g_token == t;
}

int is_register_equal_to(char *line, char reg) {
	return line[0] == reg;
}

int is_register(char *line) {
	char ch = line[0];
	return ch == 'A' || ch == 'D' || ch == 'M';
}

int is_numeric_literal(char* line) {
	return line[0] == '0' || line[0] == '1';
}


void copy_string(char *dest, char *src, int len) {
	strncpy(dest, src, len);
	dest[len-1]='\0';
}

int parse_address_instr(struct Statement* stmt) {
	stmt->type = ST_ADDRESSING;
	read_next_token();
	if (token_is(T_ID)) {
		copy_string(stmt->label_name, g_line, sizeof stmt->label_name);
	} else {
		print_error(WRONG_TOKEN_ERR, "T_ID");
		return WRONG_TOKEN_ERR;
	}
	read_next_token();
}


int parse_jump_label(struct Statement* stmt) {
	stmt->type = ST_JUMP_LABEL;
	read_next_token();
	if (token_is(T_ID)) {
		copy_string(stmt->label_name, g_line, sizeof stmt->label_name);
	} else {
		print_error(WRONG_TOKEN_ERR, "T_ID");
		return WRONG_TOKEN_ERR;
	}
	read_next_token();
	if (g_token != T_RPAREN) {
		print_error(WRONG_TOKEN_ERR, "T_RPAREN");
		return WRONG_TOKEN_ERR;
	}
	read_next_token();
}



int parse_operating_instr(struct Statement* stmt) {
	stmt->type = ST_OPERATION;

	if (token_is(T_ID)) // parse destination OR operation type
	{
		copy_string(stmt->label_name, g_line, sizeof stmt->label_name);
		read_next_token();
		if (g_token == T_EQUALS) // it was destination
		{
			DestinationType dest_type = DEST_NONE;
			int i;
			for (i = 0; i < strlen(stmt->label_name); i++) {
				switch (stmt->label_name[i]) {
					case 'A': dest_type |= DEST_A; break;
					case 'D': dest_type |= DEST_D; break;
					case 'M': dest_type |= DEST_M; break;
				}
			}
			stmt->dest_type = dest_type;
			read_next_token();
		}						
	}
	
	if (!token_is(T_EOF) && !token_is(T_SEMICOLON) && !token_is(T_NEWLINE)) {
		copy_string(stmt->label_name, g_line, sizeof stmt->label_name);
		read_next_token();
	}
	

	// Parse operation type
	// Operation starts with register (R is (A/M)):
	// D, D+1, D-1, D+R, D-R, D&R, D|R
	// R, R+1, R-1, R-D,
	if (is_register(stmt->label_name)) {
		// R, R+1, R-1, R-D,
		if (!is_register_equal_to(stmt->label_name, 'D')) 
		{
			if (is_register_equal_to(stmt->label_name, 'M')) {
				stmt->operand_name = OPERAND_M;
			}

			if (g_token == T_PLUS) {
				g_token = read_token();
				if (is_numeric_literal(g_line)) {
					stmt->op_type = OP_Y_INC; // R+1				
				} else {
					stmt->op_type = OP_X_PLUS_Y; // R+D
				}
				g_token = read_token();
			} else if (g_token == T_MINUS) { // R-1 or R-D
				g_token = read_token();
				if (is_register(g_line)) {
					stmt->op_type = OP_Y_MINUS_X; // R-D
				} else {
					stmt->op_type = OP_Y_DEC; // R-1
				}
				g_token = read_token();
			} else if (g_token == T_OR) { // R|D
				g_token = read_token();
				stmt->op_type = OP_X_OR_Y;
				g_token = read_token();
			} else if (g_token == T_AND) { // R&D
				g_token = read_token();
				stmt->op_type = OP_X_AND_Y;
				g_token = read_token();
			} else {
				stmt->op_type = OP_Y; // R
			}
		}
		else 
		{
			// D, D+1, D+R, D-1, D-R, D&R, D|R
			if (token_is(T_OR)) {
				stmt->op_type = OP_X_OR_Y; // D|R
				read_next_token();
				if (is_register_equal_to(g_line, 'M')) {
					stmt->operand_name = OPERAND_M;
				}
				read_next_token();
			} else if (g_token == T_AND) {
				stmt->op_type = OP_X_AND_Y; // D&R
				g_token = read_token();
				if (is_register_equal_to(g_line, 'M')) {
					stmt->operand_name = OPERAND_M;
				}
				g_token = read_token();
			} else if (g_token == T_MINUS) {
				g_token = read_token();
				if (is_register(g_line)) {
					stmt->op_type = OP_X_MINUS_Y; // D-R
					if (is_register_equal_to(g_line, 'M')) {
						stmt->operand_name = OPERAND_M;
					}
				} else {
					stmt->op_type = OP_X_DEC; // D-1
				}
				g_token = read_token();
			} else if (g_token == T_PLUS) {
				g_token = read_token();
				if (is_register(g_line)) {
					stmt->op_type = OP_X_PLUS_Y; // D+R
					if (is_register_equal_to(g_line, 'M')) {
						stmt->operand_name = OPERAND_M;
					}
				} else {
					stmt->op_type = OP_X_INC; // D+1
				}
				g_token = read_token();
			} else {
				stmt->op_type = OP_X; // D
			}
			
		}
	
	} 
	// Operation starts with non-register:
	// 0, 1, -1, -D, -(A/M), !D, !(A/M)
	else
	{
		if (stmt->label_name[0] == '0') {
			stmt->op_type = OP_ZERO; // 0
		} else if (stmt->label_name[0] == '1') {
			stmt->op_type = OP_ONE; // 1
		} else if (stmt->label_name[0] == '-') {
		
			if (g_line[0] == '1') {
				stmt->op_type = OP_MINUS_ONE; // -1
			} else if (is_register_equal_to(g_line, 'D')) {
				stmt->op_type = OP_MINUS_X; // -D
			} else if (is_register_equal_to(g_line, 'A')) {
				stmt->op_type = OP_MINUS_Y; // -A
			} else if (is_register_equal_to(g_line, 'M')) {
				stmt->op_type = OP_MINUS_Y; // -M
				stmt->operand_name = OPERAND_M;
			}
			g_token = read_token();
		} else if (stmt->label_name[0] == '!') {
			if (is_register_equal_to(g_line, 'D')) {
				stmt->op_type = OP_NOT_X; // !D
			} else if (is_register_equal_to(g_line, 'A')) {
				stmt->op_type = OP_NOT_Y; // !A
			} else if (is_register_equal_to(g_line, 'M')) {
				stmt->op_type = OP_NOT_Y; // !M
				stmt->operand_name = OPERAND_M;
			}
			read_next_token();
		}
	}


	// parse semicolon with jump (or just semicolon or nothing) and newline/EOF
	if (g_token == T_SEMICOLON) {
		read_next_token(); // Skip semicolon
		if (g_token == T_NEWLINE || g_token == T_EOF) {
			// Successfully done parsing current statement
		} else if (g_token == T_ID) {
			// Parse jump type
			if (!strcmp("JGT", g_line)) stmt->jump_type = J_JGT; 
			else if (!strcmp("JEQ", g_line)) stmt->jump_type = J_JEQ; 
			else if (!strcmp("JGE", g_line)) stmt->jump_type = J_JGE; 
			else if (!strcmp("JLT", g_line)) stmt->jump_type = J_JLT; 
			else if (!strcmp("JNE", g_line)) stmt->jump_type = J_JNE; 
			else if (!strcmp("JLE", g_line)) stmt->jump_type = J_JLE; 
			else if (!strcmp("JMP", g_line)) stmt->jump_type = J_JMP;
			else 
			{
				print_error(WRONG_JUMP_TYPE_ERR, "Correct jump type");
				return WRONG_JUMP_TYPE_ERR;
			}
		} else {
			print_error(WRONG_TOKEN_ERR, "T_ID (jump label)");
			return WRONG_TOKEN_ERR;
		}
	} else if (!token_is(T_EOF) && !token_is(T_NEWLINE)) {
		print_error(WRONG_TOKEN_ERR, "SEMICOLON or EOF/NEWLINE");
		return WRONG_TOKEN_ERR;
	}
	read_next_token();
}


int parse_statement_into(struct Statement* stmt) {
	memset(stmt, 0, sizeof(struct Statement)); // init struct with zeroes

	while (token_is(T_NONE) || token_is(T_NEWLINE) || token_is(T_SEMICOLON)) { 
		read_next_token();
	}
	if (token_is(T_EOF)) {
		return ALL_STATEMENTS_PARSED;
	}

	int result;
	if (token_is(T_AT)) { // parse address: @AAA
		result = parse_address_instr(stmt);
	} else if (token_is(T_LPAREN)) { // parse jump label: (AAA)
		result = parse_jump_label(stmt);		
	} else { // parse operation statement: A=D;JMP
		result = parse_operating_instr(stmt);
	}


	return result;
}

void test_parser() {
	struct Statement stmt;

	int n = 0;
	int res = parse_statement_into(&stmt);
	while (res == STATEMENT_PARSED) {
 		print_stmt(&stmt);
 		printf("\t Line number: %d \n", g_newlines_read);
 		n++;
 		
 		res = parse_statement_into(&stmt);
	}

	printf("Parsed statements: %d;\n", n);	
}


