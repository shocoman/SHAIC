#ifndef NAME_TABLE_H
#define NAME_TABLE_H

typedef enum {
	N_NONE,
	N_VAR,
	N_JUMP
} NLabelType;

struct DataNode {
	char name[64];
	int address;
	NLabelType label_type;
};

struct NameTable {
	struct DataNode *data;	
	int current_size;
	int max_size;
};


struct DataNode* get_node_info(struct NameTable* as, char*);

#endif
