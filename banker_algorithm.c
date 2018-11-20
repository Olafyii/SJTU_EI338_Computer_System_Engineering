#include <stdlib.h>
#include <stdio.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define true 1
#define false 0
typedef int bool;

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int resourceNum[NUMBER_OF_RESOURCES];

bool check()
{
	int tempAvailable[NUMBER_OF_RESOURCES];
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) 
		tempAvailable[i] = available[i];
	bool *finished = malloc(sizeof(bool) * NUMBER_OF_CUSTOMERS);//已经满足
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
		finished[i] = 0;
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)//最多customers遍
	{
		for (int j = 0; j < NUMBER_OF_CUSTOMERS; ++j)
		{	
			if (finished[j])
				continue;
			bool flag = 1;
			for (int x = 0; x < NUMBER_OF_RESOURCES; ++x)
			{
				if (tempAvailable[x] < need[j][x])
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				printf("finish one----------------------%d\n",j);
				finished[j] = 1;
				for (int x = 0; x < NUMBER_OF_RESOURCES; ++x)
					tempAvailable[x] += allocation[j][x];
				break;
			}			
		}

	}
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
		if (finished[i] == false)
			return false;
	return true;
}

void RL(char* cmd)
{
	int index = 3, ithNumber = 0, numberIndex = 0, processNum;
	char number[10];
	while (1)
	{
		if (cmd[index] != ' ')
			number[numberIndex++] = cmd[index];
		if (cmd[index] == ' ' || cmd[index] == '\n')
		{
			number[numberIndex] = '\0';
			if (ithNumber == 0)
			{
				processNum = atoi(number);
				ithNumber++;
			}
			else
			{
				available[ithNumber - 1] += atoi(number);
				allocation[processNum][ithNumber - 1] -= atoi(number);
				need[processNum][ithNumber - 1] = maximum[processNum][ithNumber - 1] - allocation[processNum][ithNumber - 1];
				ithNumber++;
			}
			numberIndex = 0;
		}
		if (cmd[index] == '\n')
			break;
		index++;
	}
	printf("Released!\n");
}

void RQ(char* cmd)
{
	int tempAvailable[NUMBER_OF_RESOURCES];
	int tempAllocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	int tempNeed[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
		tempAvailable[i] = available[i];
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
			tempAllocation[i][j] = allocation[i][j];
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
			tempNeed[i][j] = need[i][j];

	int index = 3, ithNumber = 0, numberIndex = 0, processNum;
	char number[10];
	while (1)
	{
		if (cmd[index] != ' ')
			number[numberIndex++] = cmd[index];
		if (cmd[index] == ' ' || cmd[index] == '\n')
		{
			number[numberIndex] = '\0';
			if (ithNumber == 0)
			{
				processNum = atoi(number);
				ithNumber++;
			}
			else
			{
				available[ithNumber - 1] -= atoi(number);
				allocation[processNum][ithNumber - 1] += atoi(number);
				need[processNum][ithNumber - 1] = maximum[processNum][ithNumber - 1] - allocation[processNum][ithNumber - 1];
				ithNumber++;
			}
			numberIndex = 0;
		}
		if (cmd[index] == '\n')
			break;
		index++;
	}

	if (!check())
	{
		for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
			available[i] = tempAvailable[i];
		for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
			for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
				allocation[i][j] = tempAllocation[i][j];
		for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
			for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
				need[i][j] = tempNeed[i][j];
		printf("RQ not safe\n");
	}
	else
		printf("RQ, safe\n");
}

int main(int argc, char *argv[])
{
	if (argc - 1 != NUMBER_OF_RESOURCES)
	{
		printf("ARG ERROR\n");
		return 0;
	}
	for (int i = 1; i < argc; ++i)
	{
		//printf("%d ", atoi(argv[i]));
		resourceNum[i - 1] = atoi(argv[i]);
		available[i - 1] = atoi(argv[i]);
	}
	
	FILE *f = NULL;
	char buffer[1000];
	f = fopen("max.txt", "r");
	for (int row = 0; row < NUMBER_OF_CUSTOMERS; ++row)
		for (int column = 0; column < NUMBER_OF_RESOURCES; ++column)
		{
			char number[10], c;
			int index = 0;
			while (c = fgetc(f))
			{
				if (c < 0 || c > 128)
					break;
				//printf("%c\n", c);
				if (c != ',' && c != '\n')
					number[index++] = c;
				else
					break;
			}
			number[index] = '\0';
			maximum[row][column] = atoi(number);
			allocation[row][column] = 0;
			need[row][column] = maximum[row][column];
		}
		
	char cmd[1000];
	while (fgets(cmd, 1000, stdin))
	{
		if (cmd[0] == 'R' && cmd[1] == 'Q')
		{
			printf("RQcmd\n");
			RQ(cmd);
		}
		if (cmd[0] == 'R' && cmd[1] == 'L')
		{
			printf("RLcmd\n");
			RL(cmd);
		}
		if (cmd[0] == '*')
		{
			printf("available:\n");
			for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
				printf("%d ", available[i]);
			printf("\n");
			printf("maximum:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
			{
				for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
					printf("%d ", maximum[i][j]);
				printf("\n");
			}
			printf("allocation:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
			{
				for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
					printf("%d ", allocation[i][j]);
				printf("\n");
			}
			printf("need:\n");
			for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
			{
				for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
					printf("%d ", need[i][j]);
				printf("\n");
			}
		}
	}
}
