#include "name_table.h"

struct NameTable* create_table(int init_size) {
	struct DataNode* nodes = calloc(init_size, sizeof(struct DataNode));
	struct NameTable* table = malloc(sizeof(struct NameTable));
	table->data = nodes;
	table->max_size = init_size;
	table->current_size = 0;
	return table;
}

void add_name(struct NameTable* table, char* name, int address, NLabelType label_type) {
	// increase size
	if (table->current_size == table->max_size) {
		table->max_size *= 2;
		table->data = realloc(table->data, table->max_size * sizeof(struct DataNode));
	}
	
	struct DataNode *node = &table->data[table->current_size];
	
	int name_len = sizeof(node->name);
	strncpy(node->name, name, name_len);
	node->name[name_len-1] = '\0';
	node->address = address;
	node->label_type = label_type;

	table->current_size++;
}

int set_node_info(struct NameTable* table, char* name, int address, NLabelType label_type) {
	int i, found;
	for (i = 0, found = 0; i < table->current_size; i++) {
		if (!strcmp(table->data[i].name, name)) {
			if (address > 0) table->data[i].address = address;
			if (label_type != N_NONE) table->data[i].label_type = label_type;
			found = 1;
		}
	}
	return found;
}

struct DataNode* get_node_info(struct NameTable* table, char* name) {
	int i;
	for (i = 0; i < table->current_size; i++) {
		if (!strcmp(table->data[i].name, name)) 
		{
			return &table->data[i];
		}
	}

	return NULL;
}

int table_contains(struct NameTable* table, char* name) {
	return get_node_info(table, name) != NULL;
}

void print_table(struct NameTable* table) {
	int i;
	for (i = 0; i < table->current_size; i++) {
		struct DataNode* node = &table->data[i];
		printf(" - Node N%d\n", i);
		printf("\t Name: %s\n", node->name);
		printf("\t Address: %d\n", node->address);
		printf("\t Label type: ");
		switch (node->label_type) {
			case N_NONE: printf("None"); break;
			case N_VAR: printf("Variable"); break;
			case N_JUMP: printf("Jump label"); break;
		}
		printf("\n");
	}
}

void free_table(struct NameTable* table) {
	free(table->data);
	free(table);
}


