#include "statement.h"

void print_statement_type(struct Statement* s) {
	printf(" - Statement type: ");
	switch (s->type) {
		case ST_ADDRESSING: printf("'Addressing'"); break;
		case ST_OPERATION: printf("'Operation'"); break;
		case ST_JUMP_LABEL: printf("'Jump label'"); break; 
		default: printf("'None'"); break;
	}
	printf("\n");
}

void print_label_info(struct Statement* s){
	if (s->type == ST_OPERATION) return;

	printf("\t Label name: %s \n", s->label_name);
	printf("\t Label type (if used): ");
	switch (s->label_type) {
		case LT_VARIABLE: printf("'Variable'"); break;
		case LT_JUMP: printf("'Jump'"); break;
		default: printf("'None'"); break;
	}
	printf("\n");
}

void print_operation_type(struct Statement *s) {
	if (s->type != ST_OPERATION) return;
	
	printf("\t Operation type: ");
	switch (s->op_type) {
		case OP_ZERO: printf("'OP_ZERO'"); break;
		case OP_ONE: printf("'OP_ONE'"); break;
		case OP_MINUS_ONE: printf("'OP_MINUS_ONE'"); break;
		case OP_X: printf("'OP_X'"); break;
		case OP_Y: printf("'OP_Y'"); break;
		case OP_NOT_X: printf("'OP_NOT_X'"); break;
		case OP_NOT_Y: printf("'OP_NOT_Y'"); break;
		case OP_MINUS_X: printf("'OP_MINUS_X'"); break;
		case OP_MINUS_Y: printf("'OP_MINUS_Y'"); break;
		case OP_X_INC: printf("'OP_X_INC'"); break;
		case OP_Y_INC: printf("'OP_Y_INC'"); break;
		case OP_X_DEC: printf("'OP_X_DEC'"); break;
		case OP_Y_DEC: printf("'OP_Y_DEC'"); break;
		case OP_X_PLUS_Y: printf("'OP_X_PLUS_Y'"); break;
		case OP_X_MINUS_Y: printf("'OP_X_MINUS_Y'"); break;
		case OP_Y_MINUS_X: printf("'OP_Y_MINUS_X'"); break;
		case OP_X_AND_Y: printf("'OP_X_AND_Y'"); break;
		case OP_X_OR_Y: printf("'OP_X_OR_Y'"); break;
		default: printf("'None'"); break;
	}
	printf("\n");
}

void print_2nd_operand_name(struct Statement *s) {
	if (s->type != ST_OPERATION) return;
	
	printf("\t 2nd operand name (if used): ");
	switch (s->operand_name) {
		case OPERAND_A: printf("'A'"); break;
		case OPERAND_M: printf("'M'"); break;
	}
	printf("\n");
}

void print_dest_type(struct Statement *s) {
	if (s->type != ST_OPERATION) return;
	
	printf("\t Destination type (%d): ", s->dest_type);
	if (s->dest_type & DEST_A) printf("A");
	if (s->dest_type & DEST_D) printf("D");
	if (s->dest_type & DEST_M) printf("M");
	if (s->dest_type == DEST_NONE) printf("None");
	printf("\n");
}

void print_jump_type(struct Statement *s) {
	if (s->type != ST_OPERATION) return;
	
	printf("\t Jump type: ");
	switch (s->jump_type) {
		case J_NONE: printf("'None'"); break;
		case J_JGT: printf("'JGT'"); break;
		case J_JEQ: printf("'JEQ'"); break;
		case J_JGE: printf("'JGE'"); break;
		case J_JLT: printf("'JLT'"); break;
		case J_JNE: printf("'JNE'"); break;
		case J_JLE: printf("'JLE'"); break;
		case J_JMP: printf("'JUMP'"); break;
		default: printf("'ERROR: %d'", s->operand_name); break;
	}
	printf("\n");
}

void print_memory_address(struct Statement* s) {
	if (s->type == ST_JUMP_LABEL) {
		printf("\t Label address: %d \n", s->memory_address);	
	} else {
		printf("\t Instruction address: %d \n", s->memory_address);	
	}
}

void print_stmt(struct Statement* s) {
	print_statement_type(s);

	print_label_info(s);	

	print_operation_type(s);
	print_2nd_operand_name(s);
	print_dest_type(s);
	print_jump_type(s);
	
	print_memory_address(s);
}

void pretty_print_stmt(struct Statement* s) {
	printf("%d) ", s->memory_address);
	if (s->type == ST_JUMP_LABEL) {
		printf("\t (%s) \n", s->label_name);	
	} else if (s->type == ST_ADDRESSING) {
		printf("\t @%s \n", s->label_name);	
	} else {
		printf("\t ");
		if (s->dest_type != DEST_NONE) {
			if (s->dest_type & DEST_A) printf("A");
			if (s->dest_type & DEST_D) printf("D");
			if (s->dest_type & DEST_M) printf("M");
			printf(" = ");
		}
		
		
		char *snd_reg_name = s->operand_name == OPERAND_A ? "A" : "M";
		switch (s->op_type) {
			case OP_ZERO: printf("0"); break;
			case OP_ONE: printf("1"); break;
			case OP_MINUS_ONE: printf("-1"); break;
			case OP_X: printf("D"); break;
			case OP_Y: printf("%s", snd_reg_name); break;
			case OP_NOT_X: printf("!D"); break;
			case OP_NOT_Y: printf("!%s", snd_reg_name); break;
			case OP_MINUS_X: printf("-D"); break;
			case OP_MINUS_Y: printf("-%s", snd_reg_name); break;
			case OP_X_INC: printf("D+1"); break;
			case OP_Y_INC: printf("%s+1", snd_reg_name); break;
			case OP_X_DEC: printf("D-1"); break;
			case OP_Y_DEC: printf("%s-1", snd_reg_name); break;
			case OP_X_PLUS_Y: printf("D+%s", snd_reg_name); break;
			case OP_X_MINUS_Y: printf("D-%s", snd_reg_name); break;
			case OP_Y_MINUS_X: printf("%s-D", snd_reg_name); break;
			case OP_X_AND_Y: printf("D&%s", snd_reg_name); break;
			case OP_X_OR_Y: printf("D|%s", snd_reg_name); break;
		}
		
		if (s->jump_type != J_NONE) {
			printf("; ");
			switch (s->jump_type) {
				case J_JGT: printf("JGT"); break;
				case J_JEQ: printf("JEQ"); break;
				case J_JGE: printf("JGE"); break;
				case J_JLT: printf("JLT"); break;
				case J_JNE: printf("JNE"); break;
				case J_JLE: printf("JLE"); break;
				case J_JMP: printf("JMP"); break;
			}
		}
		
		printf("\n");
	
	}
}


