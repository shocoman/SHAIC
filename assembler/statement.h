#ifndef STMT_H
#define STMT_H

typedef enum {
	ST_ADDRESSING,  // @AAA
	ST_OPERATION, 	// D | A=D;JEQ 
	ST_JUMP_LABEL,  // (LABEL)
} StatementType;

typedef enum {
	J_NONE, // No jump
	J_JGT,	// >0
	J_JEQ,  // =0
	J_JGE,  // >=0
	J_JLT,  // <0
	J_JNE,  // <>0
	J_JLE,  // <=0
	J_JMP,  // always jump
} JumpType;

typedef enum {
	OP_ZERO 		= 0b101010,
	OP_ONE 			= 0b111111,
	OP_MINUS_ONE 	= 0b111010,
	OP_X 			= 0b001100,
	OP_Y 			= 0b110000,
	OP_NOT_X 		= 0b001101,
	OP_NOT_Y 		= 0b110001,
	OP_MINUS_X 		= 0b001111,
	OP_MINUS_Y 		= 0b110011,
	OP_X_INC 		= 0b011111,
	OP_Y_INC		= 0b110111,
	OP_X_DEC 		= 0b001110,
	OP_Y_DEC 		= 0b110010,
	OP_X_PLUS_Y 	= 0b000010,
	OP_X_MINUS_Y 	= 0b010011,
	OP_Y_MINUS_X 	= 0b000111,
	OP_X_AND_Y 		= 0b000000,
	OP_X_OR_Y 		= 0b010101,
} OperationType;

typedef enum {
	DEST_NONE	= 0b000,
	DEST_A 		= 0b100,
	DEST_D		= 0b010,
	DEST_M 		= 0b001,
} DestinationType;

typedef enum {
	LT_VARIABLE,
	LT_JUMP,
} LabelType;

typedef enum {
	OPERAND_A,
	OPERAND_M
} OperandName;

struct Statement {
	// For parsing
	StatementType type;	
	char label_name[64];
	LabelType label_type;

	OperationType op_type;
	OperandName operand_name;
	DestinationType dest_type;
	JumpType jump_type;
	
	// For emitting
	int memory_address; // also label address for 'Jump label' type 
};


#endif
