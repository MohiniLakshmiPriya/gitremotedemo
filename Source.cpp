#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *regis[8] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
int regsVals[8];

int *memory = (int*)malloc(sizeof(int) * 100);

int getRegis(char *op) {
	for (int i = 0; i < 8; i++) {
		if (!strcmp(regis[i], op)) {
			return i;
		}
	}
	return 0;
}

void regToMem(char *token) {
	char *s = (char *)malloc(sizeof(char) * 5);
	char *s1 = (char *)malloc(sizeof(char) * 5);
	token = strtok(NULL, " ");
	FILE *fp = fopen("symbol.txt", "r");
	int n = 0 , st = 0, et = 0, read = 0, val = 0;
	while (fscanf(fp, "%d %s %d %d %d %d\n",&n, s1, &st, &et, &read, &val) != EOF) {
		if (atoi(token) == n) {
			break;
		}
	}
	if (read == 1)
		return;
	if (et - st > 1) {
		token = strtok(NULL, " ");
		int idx = atoi(token);
		token = strtok(NULL, "\n");
		memory[st + idx] = regsVals[getRegis(regis[atoi(token)])];
	}
	else {
		s = strtok(NULL, "\n");
		memory[st] = regsVals[getRegis(regis[atoi(s)])];
	}
	fclose(fp);
}

void MemToReg(char *token) {
	token = strtok(NULL," ");
	int reg = atoi(token);
	int n = 0, st = 0, et = 0, read = 0, val = 0;
	token = strtok(NULL,"\n");
	FILE *fp = fopen("symbol.txt","r");
	char *s1 = (char *)malloc(sizeof(char) * 5);
	if (strlen(token) > 1) {
		char *s = strtok(token," ");
		while (fscanf(fp, "%d %s %d %d %d %d\n", &n, s1, &st, &et, &read, &val) != -1) {
			if (atoi(s) == n) {
				break;
			}
		}
		s = strtok(NULL, "\n");
		regsVals[reg] = memory[st+atoi(s)];
	}
	else {
		while (fscanf(fp, "%d %s %d %d %d %d\n", &n, s1, &st, &et, &read, &val) != -1) {
			if (atoi(token) == n) {
				break;
			}
		}
		regsVals[reg] = memory[st];
	}
	fclose(fp);
}

void read(int reg) {
	scanf("%d", &regsVals[reg]);
}

void add(int reg1,int reg2,int reg3) {
	regsVals[reg1] = regsVals[reg2] + regsVals[reg3];
}

void sub(int reg1, int reg2, int reg3) {
	regsVals[reg1] = regsVals[reg2] - regsVals[reg3];
}

void mul(int reg1, int reg2, int reg3) {
	regsVals[reg1] = regsVals[reg2] * regsVals[reg3];
}

void print(char *token) {
	if (strlen(token) == 1) {
		printf("\n\n%d\n\n", regsVals[atoi(token)]);
		return;
	}
	char *s = strtok(token," ");
	s = strtok(NULL, "\n");
	int n = 0, st = 0, et = 0, read = 0, val = 0;
	FILE *fp = fopen("symbol.txt", "r");
	char *s1 = (char *)malloc(sizeof(char) * 5);
	if (strlen(s) > 1) {
		char *s2 = strtok(s, " ");
		while (fscanf(fp, "%d %s %d %d %d %d\n", &n, s1, &st, &et, &read, &val) != -1) {
			if (atoi(s2) == st) {
				break;
			}
		}
		s2 = strtok(NULL, "\n");
		printf("%d\n",memory[st + atoi(s2)]);
	}
	else {
		while (fscanf(fp, "%d %s %d %d %d %d\n", &n, s1, &st, &et, &read, &val) != -1) {
			if (atoi(s) == st) {
				break;
			}
		}
		if (read == 1) {
			printf("%d\n", val);
			return;
		}
		printf("%d\n",memory[st]);
	}
	fclose(fp);
}

FILE *jmp(int op,FILE *fp,int flag) {
	int n = ftell(fp);
	fpos_t pos;
	fgetpos(fp, &pos);
	rewind(fp);
	int i = 1;
	if (i == op)
		return fp;
	char buf[100];
	while (!feof(fp)) {
		fgets(buf, 100, fp);
		if ((i + 1) == op) {
			/*if (!flag && ftell(fp) > n) {
				fsetpos(fp,&pos);
				return fp;
			}*/
			return fp;
		}
		i++;
	}
	return fp;
}

char *cond(int op,char *s) {
	if (op == 8)
		s = "EQ";
	if (op == 9)
		s = "LT";
	if (op == 10)
		s = "GT";
	if (op == 11)
		s = "LTEQ";
	if (op == 12)
		s = "GTEQ";
	return s;
}

FILE *ifThenElse(char *token, FILE *fp) {
	token = strtok(NULL," ");
	int val1 = regsVals[getRegis(regis[atoi(token)])];
	token = strtok(NULL, " ");
	char *s = (char*)malloc(sizeof(char) * 5);
	s = cond(atoi(token), s);
	token = strtok(NULL," ");
	int val2 = regsVals[getRegis(regis[atoi(token)])];
	if (!strcmp(s,"LT")) {
		if (val1 < val2) {
			return fp;
		}
	}
	if (!strcmp(s, "LTEQ")) {
		if (val1 <= val2) {
			return fp;
		}
	}
	if (!strcmp(s, "GT")) {
		if (val1 > val2) {
			return fp;
		}
	}
	if (!strcmp(s, "GTEQ")) {
		if (val1 >= val2) {
			return fp;
		}
	}
	if (!strcmp(s, "EQ")) {
		if (val1 == val2) {
			return fp;
		}
	}
	token = strtok(NULL, "\n");
	return jmp(atoi(token), fp,1);
}

int main() {
	FILE *fp = fopen("interm.txt", "r");
	char *buf = (char*)malloc(sizeof(char) * 100);
	while (!feof(fp)) {
		if (fgets(buf, 100, fp) == NULL)
			break;
		if (buf == "\n")
			break;
		char *token;
		token = strtok(buf, " ");
		token = strtok(NULL, " ");
		if (atoi(token) == 1) {
			regToMem(token);
		}
		if (atoi(token) == 2) {
			MemToReg(token);
		}
		if (atoi(token) == 3) {
			token = strtok(NULL, " ");
			int reg1 = atoi(token);
			token = strtok(NULL, " ");
			int reg2 = atoi(token);
			token = strtok(NULL, "\n");
			int reg3 = atoi(token);
			add(reg1, reg2, reg3);
		}
		if (atoi(token) == 4) {
			token = strtok(NULL, " ");
			int reg1 = atoi(token);
			token = strtok(NULL, " ");
			int reg2 = atoi(token);
			token = strtok(NULL, "\n");
			int reg3 = atoi(token);
			sub(reg1, reg2, reg3);
		}
		if (atoi(token) == 5) {
			token = strtok(NULL, " ");
			int reg1 = atoi(token);
			token = strtok(NULL, " ");
			int reg2 = atoi(token);
			token = strtok(NULL, "\n");
			int reg3 = atoi(token);
			mul(reg1, reg2, reg3);
		}
		if (atoi(token) == 6) {
			token = strtok(NULL, "\n");
			fp = jmp(atoi(token),fp,0);
		}
		if (atoi(token) == 7) {
			fp = ifThenElse(token, fp);
		}
		if (atoi(token) == 14) {
			token = strtok(NULL, "\n");
			read(atoi(token));
		}
		if (atoi(token) == 13) {
			token = strtok(NULL, "\n");
			print(token);
		}
	}
	fclose(fp);
	system("PAUSE");
}