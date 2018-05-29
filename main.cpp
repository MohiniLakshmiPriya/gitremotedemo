#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Header.h"

char *regs[8] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };

int stack[20];
int top = -1;

struct intermediate {
	int inst;
	int opcode;
	int parameters[10];
};

struct symbol {
	char name[10];
	int add;
	int size;
	int readOnly = 0;
	int digit;
};

struct Label {
	char name[10];
	int val;
};

int getOpcode(char *op) {
	char *insts[14] = { "MOV1", "MOV2","ADD","SUB","MUL","JMP","IF","EQ","LT","GT","LTEQ","GTEQ","PRINT","READ" };
	for (int i = 0; i < 14; i++) {
		if (!strcmp(insts[i], op)) {
			return i+1;
		}
	}
	return 0;
}

int getReg(char *op) {
	for (int i = 0; i < 8; i++) {
		if (!strcmp(regs[i], op)) {
			return i;
		}
	}
	return -1;
}

int getSize(char *token, struct symbol *table, int *Tvar) {
	char *s = (char*)malloc(sizeof(char)  * 2);;
	int i = 0;
	for (i; token[i] != '[' && token[i] != '\0'; i++) {
		s[i] = token[i];
	}
	s[i] = '\0';
	for (int i = 0; i < *Tvar; i++) {
		if (!strcmp(s, table[i].name)) {
			return i;
		}
	}
	return -1;
}

int getLabel(char *token, struct Label *label, int *Lvar) {
	for (int i = 0; i < *Lvar; i++) {
		if (!strcmp(token, label[i].name)) {
			return i;
		}
	}
	return -1;
}

void addToLabel(char *buf, struct Label *label, int *Lsize, int *Lvar, int line) {
	if (*Lsize == *Lvar) {
		*Lsize *= 2;
		label = (struct Label *)realloc(label, *Lsize);
	}
	char *token = strtok(buf, ":");
	int i = 0;
	for (i; token[i] != '\0'; i++) {
		label[*Lvar].name[i] = token[i];
	}
	label[*Lvar].name[i] = '\0';
	label[*Lvar].val = line;
	(*Lvar)++;
}

void mov1(char *token, struct intermediate *opCodes, int *Opvar, struct symbol *table,int *Tvar) {
	int i = 0;
	token = strtok(NULL, ",");
	char *s = strtok(NULL, "\n");
	int var = getSize(token, table, Tvar);
	if (table[var].size - table[var].add <= 1)
		opCodes[*Opvar].parameters[i++] = table[var].add;
	else {
		int j = 0;
		for (j; token[j] != '['; j++);
		j++;
		int n = token[j] - '0';
		opCodes[*Opvar].parameters[i++] = table[var].add;
		opCodes[*Opvar].parameters[i++] = n;
	}
	opCodes[*Opvar].parameters[i++] = getReg(s);
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void mov2(char *token, struct intermediate *opCodes, int *Opvar, struct symbol *table, int *Tvar) {
	int i = 0;
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, "\n");
	int var = getSize(token, table, Tvar);
	if (table[var].size - table[var].add <= 1)
		opCodes[*Opvar].parameters[i++] = table[var].add;
	else {
		int j = 0;
		for (j; token[j] != '['; j++);
		j++;
		int n = token[j] - '0';
		opCodes[*Opvar].parameters[i++] = table[var].add;
		opCodes[*Opvar].parameters[i++] = n;
	}
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void add(char *token, struct intermediate *opCodes, int *Opvar) {
	int i = 0;
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, "\n");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void sub(char *token, struct intermediate *opCodes, int *Opvar) {
	int i = 0;
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, "\n");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void mul(char *token, struct intermediate *opCodes, int *Opvar) {
	int i = 0;
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, ",");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, "\n");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void jump(char *token, struct intermediate *opCodes, int *Opvar, struct Label *label, int *Lvar) {
	int i = 0;
	token = strtok(NULL, "\n");
	int var = getLabel(token, label, Lvar);
	opCodes[*Opvar].parameters[i++] = label[var].val;
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void ifThen(char *token, struct intermediate *opCodes, int *Opvar,int *line) {
	int i = 0;
	token = strtok(NULL, " ");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	token = strtok(NULL, " ");
	opCodes[*Opvar].parameters[i++] = getOpcode(token);
	token = strtok(NULL, " ");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	opCodes[*Opvar].parameters[i++] = -2;
	opCodes[*Opvar].parameters[i++] = -1;
	stack[++top] = *line;
	(*Opvar)++;
}

void print(char *token, struct intermediate *opCodes, int *Opvar, struct symbol *table, int *Tvar) {
	int i = 0;
	token = strtok(NULL, "\n");
	int d = getReg(token);
	if (d != -1) {
		opCodes[*Opvar].parameters[i++] = d;
		opCodes[*Opvar].parameters[i++] = -1;
		(*Opvar)++;
		return;
	}
	int var = getSize(token, table, Tvar);
	opCodes[*Opvar].parameters[i++] = 0;
	if (table[var].size - table[var].add <= 1)
		opCodes[*Opvar].parameters[i++] = table[var].add;
	else {
		int j = 0;
		for (j; token[j] != '['; j++);
		j++;
		int n = token[j] - '0';
		opCodes[*Opvar].parameters[i++] = table[var].add;
		opCodes[*Opvar].parameters[i++] = n;
	}
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void read(char *token, struct intermediate *opCodes, int *Opvar) {
	int i = 0;
	token = strtok(NULL, "\n");
	opCodes[*Opvar].parameters[i++] = getReg(token);
	opCodes[*Opvar].parameters[i++] = -1;
	(*Opvar)++;
}

void parse(char *buf, struct intermediate *opCodes, int *Opsize, int *Opvar, int *line, struct symbol *table, int *Tvar,struct Label *label,int *Lvar,int *Lsize) {
	char *token;
	token = strtok(buf, " ");
	int Opcode = 0;
	if (!strcmp(token, "ELSE\n")) {
		Opcode = 6;
		opCodes[*Opvar].inst = *line;
		opCodes[*Opvar].opcode = Opcode;
		opCodes[*Opvar].parameters[0] = -2;
		opCodes[*Opvar].parameters[1] = -1;
		(*Opvar)++;
		int n = stack[top--],i = 0;
		for (i; opCodes[n-1].parameters[i] != -2; i++);
		opCodes[n-1].parameters[i] = (*line) + 1;
		stack[++top] = *line;
		(*line)++;
		return;
	}
	else if (!strcmp(token, "ENDIF\n")) {
		//addToLabel(buf, label, Lsize, Lvar, (*line) + 1);
		int n = stack[top--], i = 0;
		for (i; opCodes[n-1].parameters[i] != -2; i++);
		opCodes[n-1].parameters[i] = (*line);
		//stack[++top] = *line;
		//(*line)++;
		return;
	}
	else
		Opcode = getOpcode(token);
	if (*Opvar == *Opsize) {
		*Opsize *= 2;
		opCodes = (struct intermediate *)realloc(opCodes, *Opsize);
	}
	opCodes[*Opvar].inst = *line;
	opCodes[*Opvar].opcode = Opcode;
	if (Opcode == 1) {
		mov1(token, opCodes, Opvar, table, Tvar);
	}
	if (Opcode == 2) {
		mov2(token, opCodes, Opvar, table, Tvar);
	}
	if (Opcode == 3) {
		add(token, opCodes, Opvar);
	}
	if (Opcode == 4) {
		sub(token, opCodes, Opvar);
	}
	if (Opcode == 5) {
		mul(token, opCodes, Opvar);
	}
	if (Opcode == 6) {
		jump(token, opCodes, Opvar,label,Lvar);
	}
	if (Opcode == 7) {
		ifThen(token, opCodes, Opvar,line);
	}
	if (Opcode == 13) {
		print(token, opCodes, Opvar,table,Tvar);
	}
	if (Opcode == 14) {
		read(token, opCodes, Opvar);
	}
	(*line)++;
}

void addSymbol(char *buf, struct symbol *table,int *Tsize,int *Tvar,int *total_size) {
	char *token;
	token = strtok(buf, " ");
	if (!strcmp("DATA", token)) {
		token = strtok(NULL, "\n");
		if (strlen(token) == 1) {
			if (*Tvar == *Tsize) {
				*Tsize *= 2;
				table = (struct symbol *)realloc(table,  *Tsize);
			}
			int i = 0;
			for (i; token[i] != '\0'; i++) {
				table[*Tvar].name[i] = token[i];
			}
			table[*Tvar].name[i] = '\0';
			table[*Tvar].add = *total_size;
			*total_size += 1;
			table[*Tvar].size = *total_size;
			table[*Tvar].readOnly = 0;
			table[*Tvar].digit = 0;
			(*Tvar)++;
		}
		else {
			int i = 0;
			for (i; token[i] != '['; i++) {
				table[*Tvar].name[i] = token[i];
			}
			//printf("%c\n", token[0]);
			table[*Tvar].name[i] = '\0';
			table[*Tvar].add = *total_size;
			int size = 0;
			i++;
			for (i; token[i] != ']'; i++) {
				size = size * 10 + token[i] - '0';
			}
			*total_size +=  size;
			table[*Tvar].size = *total_size;
			table[*Tvar].readOnly = 0;
			table[*Tvar].digit = 0;
			(*Tvar)++;
		}
	}
	if (!strcmp("CONST", token)) {
		table[*Tvar].readOnly = 1;
		token = strtok(NULL, "=");
		int i = 0;
		for (i; token[i] != '\0'; i++) {
			table[*Tvar].name[i] = token[i];
		}
		table[*Tvar].name[i] = '\0';
		table[*Tvar].add = *total_size;
		*total_size += 1;
		table[*Tvar].size = 0;
		token = strtok(NULL, "\n");
		int digit = 0;
		i = 0;
		for (i; token[i] != '\0'; i++) {
			digit = digit * 10 + token[i] - '0';
		}
		table[*Tvar].digit = digit;
		(*Tvar)++;
	}
}

/*int main() {
	int Tsize = 2,Tvar = 0,total_size = 0;
	struct symbol *table = (struct symbol *)calloc(sizeof(struct symbol) , Tsize);
	int Opsize = 20, Opvar = 0;
	struct intermediate *opCodes = (struct intermediate *)calloc(sizeof(struct intermediate), Opsize);
	int Lsize = 20, Lvar = 0;
	struct Label *label = (struct Label *)calloc(sizeof(struct Label), Lsize);
	FILE *fp = fopen("input.txt", "r+");
	if (!fp) {
		printf("file open error\n");
		exit(0);
	}
	char *buf = (char*)malloc(sizeof(char) * 100);
	int line = 0;
	while (!feof(fp)) {
		fgets(buf, 100, fp);
		if (buf == NULL || buf == "\n")
			break;
		if (!strcmp("START:\n", buf)) {
			line++;
			continue;
		}
		if (buf[strlen(buf) - 2] == ':') {
			addToLabel(buf,label,&Lsize,&Lvar,line);
		}
		else if (line) {
			parse(buf, opCodes, &Opsize, &Opvar, &line, table, &Tvar,label,&Lvar,&Lsize);
		}
		else {
			addSymbol(buf,table,&Tsize,&Tvar,&total_size);
		}
	}
	fclose(fp);
	fp = fopen("symbol.txt", "w");
	for (int i = 0; i < Tvar; i++) {
		printf("%s %d %d %d %d\n", table[i].name, table[i].add, table[i].size, table[i].readOnly, table[i].digit);
		fprintf(fp, "%d %s %d %d %d %d\n",i, table[i].name, table[i].add, table[i].size, table[i].readOnly, table[i].digit);
	}
	fclose(fp);
	fp = fopen("interm.txt", "w");
	for (int i = 0; i < Opvar; i++) {
		printf("%d %d", opCodes[i].inst, opCodes[i].opcode);
		fprintf(fp, "%d %d", opCodes[i].inst, opCodes[i].opcode);
		fseek(fp, ftell(fp), SEEK_SET);
		for (int j = 0; opCodes[i].parameters[j] != -1; j++) {
			fprintf(fp, " %d", opCodes[i].parameters[j]);
			fseek(fp, ftell(fp), SEEK_SET);
			printf(" %d", opCodes[i].parameters[j]);
		}
		printf("\n");
		fprintf(fp, "\n");
		fseek(fp, ftell(fp), SEEK_SET);
	}
	fclose(fp);
	//loadCode(opCodes, &Opsize, table, &Tsize);
	system("PAUSE");
}*/