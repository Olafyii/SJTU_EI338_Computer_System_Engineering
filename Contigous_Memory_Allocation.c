#include <stdlib.h>
#include <stdio.h>

int *arr;
int totalSpace;

typedef struct map
{
	int index;//corresponding number in arr
	char* name;//process name
	struct map* next;
} Map;

Map head;
int uniqueIndex = 0;

void RL(char* cmd, int totalSpace)
{
	int index = 3, nameIndex = 0, targetIndex = -1;
	char name[20];
	while(cmd[index++] != '\n')
		name[nameIndex++] = cmd[index - 1];
	name[nameIndex] = '\0';
	Map* p = head.next;
	while (p != NULL)
	{
		if (strcmp(p->name, name) == 0)
		{
			targetIndex = p->index;
			break;
		}
		p = p->next;
	}
	if (targetIndex == -1)
	{
		printf("RL error\n");
		return;
	}
	for (int i = 0; i < totalSpace; ++i)
		if (arr[i] == targetIndex)
			arr[i] = -1;
}

void RQ(char* cmd)
{
	int i = 3, index = 0, intSpace;
	char *name = malloc(sizeof(char)*20);
	while (cmd[i++] != ' ')
		name[index++] = cmd[i - 1];
	name[index] = '\0';
	index = 0;
	char *space = malloc(sizeof(char) * 20);
	while (cmd[i++] != ' ')
		space[index++] = cmd[i - 1];
	space[index] = '\0';
	intSpace = atoi(space);
	//check if already in the map
	Map*p = head.next;
	int targetIndex, flag = 0;
	while (p != NULL)
	{
		if (strcmp(p->name, name) == 0)
		{
			targetIndex = p->index;
			flag = 1;
		}
		p = p->next;
	}
	//
	
	if (!flag)//not in map
	{
		//add to map
		Map *tmp = malloc(sizeof(Map));
		tmp->index = uniqueIndex++;
		targetIndex = uniqueIndex - 1;
		tmp->name = name;
		tmp->next = head.next;
		head.next = tmp;
		//
	}
	if (cmd[i] == 'F')
	{
		int sp = 0, start = 0;
		for (int j = 0; j < totalSpace; ++j)
		{
			if (arr[j] == -1)
				sp++;
			else
			{
				sp = 0;
				start = j + 1;
			}
			if (sp == intSpace)
			{
				for (int x = start; x < start + intSpace; ++x)
					arr[x] = targetIndex;
				break;
			}
		}
		if (sp < intSpace)
			printf("RQ failure\n");
	}
	else if (cmd[i] == 'B')
	{
		int sp = 0, start = 0;
		int bestStart, bestSpace = 0x3f3f3f3f;
		for (int j = 0; j < totalSpace; ++j)
		{
			if (arr[j] == -1)
				sp++;
			else
			{
				if (sp != 0)
				{
					if (j - start >= intSpace && j - start < bestSpace)
					{
						bestStart = start; 
						bestSpace = j - start;
					}
					sp = 0;
					start = j + 1;
				}
				else
				{
					sp = 0;
					start = j + 1;
				}
			}
		}
		if (bestSpace == 0x3f3f3f3f)
			printf("RQ error\n");
		else
		{
			for (int x = bestStart; x < bestStart + intSpace; ++x)
				arr[x] = targetIndex;
		}
	}
	else if (cmd[i] == 'W')
	{
		int sp = 0, start = 0;
		int worstStart, worstSpace = -1;
		for (int j = 0; j < totalSpace; ++j)
		{
			if (arr[j] == -1)
				sp++;
			else
			{
				if (sp != 0)
				{
					if (j - start >= intSpace && j - start > worstSpace)
					{
						worstStart = start; 
						worstSpace = j - start;
					}
					sp = 0;
					start = j + 1;
				}
				else
				{
					sp = 0;
					start = j + 1;
				}
			}
		}
		if (worstSpace == -1)
			printf("RQ error\n");
		else
		{
			for (int x = worstStart; x < worstStart + intSpace; ++x)
				arr[x] = targetIndex;
		}
	}
}

char* find(int index)
{
	char* un = "Unused";
	if (index == -1)
		return un;
	Map* p = head.next;
	while (p != NULL)
	{
		if (p->index == index)
			return p->name;
		p = p->next;
	}
	printf("find function error, index:%d\n", index);
	return NULL;
}

void stat()
{
	int start = 0;
	for (int i = 1; i < totalSpace; ++i)
	{
		if (arr[i] != arr[i - 1])
		{
			printf("Addresses [%d: %d] %s\n", start, i - 1, find(arr[i - 1]));
			start = i;
		}
	}
	printf("Addresses [%d: %d] %s\n", start, totalSpace - 1, find(arr[totalSpace - 1]));
}

void C(int totalSpace)
{
	int index = 0;
	for (int i = 0; i < totalSpace; ++i)
		if (arr[i] != -1)
			arr[index++] = arr[i];
	for (; index < totalSpace; ++index)
		arr[index] = -1;
}

int main(int argc, char** argv)
{
	totalSpace = atoi(argv[1]);
	arr = malloc(sizeof(int) * atoi(argv[1]));
	for (int i = 0; i < totalSpace; ++i)
		arr[i] = -1;
	char cmd[1000];
	printf("allocator>");
	while (fgets(cmd, 1000, stdin))
	{
		if (cmd[0] == 'R' && cmd[1] == 'Q')
		{
			RQ(cmd);
			usleep(3);
		}
		else if (cmd[0] == 'R' && cmd[1] == 'L')
		{
			RL(cmd, totalSpace);
		}
		else if (cmd[0] == 'X' || cmd[0] == 'x')
		{
			return 0;
		}
		else if (cmd[0] == 'C')
		{
			C(totalSpace);
		}
		else if (cmd[0] == 'S')
		{
			stat();
		}
		else
		{
			printf("cmd error\n");
			return 0;
		}
		printf("allocator>");
	}
	free(arr);
	return 0;
}
