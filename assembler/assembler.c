#include "parser.c"
#include <stdlib.h>
#include "statement.h"
#include "name_table.c"
#include "code_emitter.c"


struct NameTable* init_table() {
	struct NameTable* table = create_table(32);	
	
	// fill table with default registers (R0-R16, SCREEN, KBD)
	int i;
	for (i = 0; i <= 15; i++) {
		char name[4] = {'R', '\0', '\0', '\0'};
		if (i < 10) {
			name[1] = i + '0';
		} else {
			name[1] = '1';
			name[2] = i - 10 + '0';
		}
		add_name(table, name, i, N_NONE);
	}
	add_name(table, "SCREEN", 0b100000000000000, N_NONE);
	add_name(table, "KBD", 0b110000000000000, N_NONE);	

	add_name(table, "SP", 0, N_NONE);
	add_name(table, "LCL", 1, N_NONE);
	add_name(table, "ARG", 2, N_NONE);
	add_name(table, "THIS", 3, N_NONE);
	add_name(table, "THAT", 4, N_NONE);
	
	// print_table(table);

	
	
	return table;
}


int fill_stmt_addresses(struct Statement* stmts, int stmt_number) {
	int i, address;
	for (i = 0, address = 0; i < stmt_number; i++) {
		stmts[i].memory_address = address;
		if (stmts[i].type != ST_JUMP_LABEL) {
			address++;
		}
	}
}


int assembler(const char* asm_input_file, const char* output_file) {

	g_asm_file = fopen(asm_input_file, "r");
	if (!g_asm_file) {
		printf("File reading error!\n");
		return -1;
	}	
	

	// init statement array
	int stmt_number = count_non_empty_lines_in_file(asm_input_file);

	int stmt_size = sizeof(struct Statement);
	struct Statement* stmt_array = (struct Statement*) calloc(stmt_number, stmt_size);
	if (!stmt_array) {
		printf("Statement array allocation error!\n");
		return -1;
	}
	
	// parse statements
	int i;
	for (i = 0; i < stmt_number; i++) {
		int res = parse_statement_into(&stmt_array[i]);
		if (res < 0) return -1;
	}
	
	// set location address for every statement
	fill_stmt_addresses(stmt_array, stmt_number);
	
	// create table and fill it with default variables
	struct NameTable* name_table = init_table();
	
	// add all user defined variables and potential jump labels to name table
	for (i = 0; i < stmt_number; i++) {
		struct Statement* stmt = &stmt_array[i];
		if (stmt->type == ST_ADDRESSING && !table_contains(name_table, stmt->label_name)) {
			add_name(name_table, stmt->label_name, 0, N_VAR);
		}
	}
	
	
	// change certain variables to jump labels
	for (i = 0; i < stmt_number; i++) {
		struct Statement* stmt = &stmt_array[i];
		if (stmt->type == ST_JUMP_LABEL) {
			if (table_contains(name_table, stmt->label_name)) {
				set_node_info(name_table, stmt->label_name, stmt->memory_address, N_JUMP);
			} else {
				add_name(name_table, stmt->label_name, stmt->memory_address, N_JUMP);
			}
		}
	}
	
	// set right memory locations for variables
	int counter = 16;
	for (i = 0; i < stmt_number; i++) {
		struct Statement* stmt = &stmt_array[i];
		if (stmt->type == ST_ADDRESSING) 
		{
			struct DataNode* node = get_node_info(name_table, stmt->label_name);
			if (node && node->label_type == N_VAR) {
				
				// check if number
				int is_num = 1;
				int j;
				for (j = 0; j < strlen(stmt->label_name); j++) {
					if (!isdigit(stmt->label_name[j])) 
						is_num = 0;
				}
				if (is_num) {
					set_node_info(name_table, stmt->label_name, atoi(stmt->label_name), N_VAR);
				} else {
					set_node_info(name_table, stmt->label_name, counter, N_VAR);
					counter++;
				}

			}
		}
	}
	
	// print instructions
	for (i = 0; i < stmt_number; i++) {
		// print_stmt(&stmt_array[i]);
		// pretty_print_stmt(&stmt_array[i]);
	}
	
	// print_table(name_table);
	
	print_program_to_file(stmt_array, stmt_number, name_table, output_file);
	
	free_table(name_table);
	fclose(g_asm_file);
	
	return 0;
}

