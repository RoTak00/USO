#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char * argv[], char * env[])
{
	char * result = NULL;
	char * currenv1 = NULL, * currenv2 = NULL;
	
	unsigned len = 0;
	while(env[len] != NULL)
	{
		printf("%2u: %s\n", len, env[len]);
		len ++;
	}
	
	for(int i = 2; i < len; i += 2)
	{
		
		
		if((currenv1 = (char*)malloc(sizeof(char) * (strlen(env[i-1]) + 1))) == NULL)
		{
			perror("malloc");
			goto error1;
		}
		strncpy(currenv1, env[i-1], strlen(env[i-1]));
		currenv1[strlen(env[i-1])] = '\0'; 
		
		if((currenv2 = (char*)malloc(sizeof(char) * (strlen(env[i]) + 1))) == NULL)
		{
			perror("malloc");
			goto error2;
		}
		strncpy(currenv2, env[i], strlen(env[i]));
		currenv2[strlen(env[i])] = '\0'; 
		
		char *name = strtok(currenv1, "=");
		//char *base = strtok(NULL, "=");
		char *base = name + strlen(name) + 1;
		char *add = strtok(currenv2, "=");
		//add = strtok(NULL, "=");
		add = add + strlen(add) + 1;
		
		if((result = (char*)malloc(sizeof(char) * (strlen(base) + strlen(add) + 1))) == NULL)
		{
			perror("malloc");
			goto error3;
		}
		
		strncpy(result, base, strlen(base));
		strncpy(result + strlen(base), add, strlen(add));
		result[strlen(base) + strlen(add)] = '\0';
		
		if(setenv(name, result, 1) == -1)
		{
			perror("setenv");
			goto error4;
		}
		
		free(currenv1);
		free(currenv2);
		free(result);
	}
	
	unsigned index = 0;
	while(env[index] != NULL)
	{
		printf("%2u: %s\n", index, env[index]);
		index ++;
	}
	return 0;
	
	error4: 
	error3: free(result);
	error2: free(currenv2);
	error1: free(currenv1);
	return 1;
}