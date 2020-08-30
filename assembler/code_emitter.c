#include "name_table.h"
#include "statement.h"

char* convert_to_binary_and_pad(int number, int final_length) {
	char *num_str = malloc(final_length+1);
	memset(num_str, '0', final_length);
	num_str[final_length] = '\0';
	
	int i = 0;
	while (number != 0 && i < final_length) {
		num_str[final_length-1 - i] = number % 2 + '0';
		number /= 2;
		i++;
	}
	
	return num_str;
}

char* get_addressing_instr(struct Statement* stmt, struct NameTable* table) {
	// e.g. 0xxx xxxx xxxx xxxx
	
	struct DataNode* node = get_node_info(table, stmt->label_name);
	char* final_instr = convert_to_binary_and_pad(node->address, 16);
	final_instr[0] = '0';
	
	return final_instr;
}

char* get_operation_instr(struct Statement* stmt, struct NameTable* table) {
	// e.g. 1 11 x xxxxxx xxx xxx
	char* fst_3bits = convert_to_binary_and_pad(0b111, 3);
	char* operand_name = convert_to_binary_and_pad(stmt->operand_name, 1);
	char* operation_type = convert_to_binary_and_pad(stmt->op_type, 6);
	char* dest_type = convert_to_binary_and_pad(stmt->dest_type, 3);
	char* jump_type = convert_to_binary_and_pad(stmt->jump_type, 3);
	
	char* final_instr = calloc(16, 1);
	strcat(final_instr, fst_3bits);
	strcat(final_instr, operand_name);
	strcat(final_instr, operation_type);
	strcat(final_instr, dest_type);
	strcat(final_instr, jump_type);
	
	free(fst_3bits);
	free(operand_name);
	free(operation_type);
	free(dest_type);
	free(jump_type);
	
	return final_instr;
}


char* get_instr(struct Statement* stmt, struct NameTable* table) {
	char* instr = NULL;
	if (stmt->type == ST_ADDRESSING) {
		instr = get_addressing_instr(stmt, table);
	} else if (stmt->type == ST_OPERATION) {
		instr = get_operation_instr(stmt, table);
	}
	
	return instr;
}

int print_program_to_file(struct Statement* stmt_array, int stmt_number,
						 struct NameTable* table, const char* filename) {
	FILE *f = fopen(filename, "w");
	if (!f) return -1;
	
	int i;
	for (i = 0; i < stmt_number; i++) {
		if (stmt_array[i].type != ST_JUMP_LABEL) {
			char* instr = get_instr(&stmt_array[i], table);
			//printf("%d) ", stmt_array[i].memory_address);
			//printf("%s \n", instr);

			fwrite(instr, 1, 16, f);
			fwrite("\n", 1, 1, f);
			// free(instr);
		}
	}
	
	
	fclose(f);
		
	return 0;
}


