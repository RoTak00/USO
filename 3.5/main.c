#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int in_array(int x, int* v, int size)
{
	for(int i = 0; i < size; ++i)
		if(v[i] == x) return 1;
	return 0;
}

long long fact(int n)
{
	if(n == 0) return 1;
	return n * fact(n - 1);
}

int main()
{
	int n;
	scanf("%d", &n);
	
	int fd[2];
	if(pipe(fd) == -1)
	{
		perror("pipe");
		return 1;
	}
	
	int *v;
	if((v = (int*)malloc(sizeof(int) * n)) == NULL)
	{
		perror("malloc");
		return 2;
	}
	
	int main_pid = fork();
	if(main_pid == -1)
	{
		perror("main_fork");
		free(v);
		return 3;
	}
	
	if(main_pid == 0)
	{
		
		int size = 0;
		for(int i = 1; i <= n; ++i)
		{
			if(in_array(i, v, size))
				continue;
			
			v[size] = i;
			
			if(size != n - 1)
			{
				int pid = fork();
				if(pid == -1)
				{
					perror("fork");
					free(v);
					return 4;
				}
				
				
				if(pid == 0)
				{
					i = 0;
					size += 1;
				}
				
			}
			else
			{
				for(int i = 0; i < n; ++i)
				{
					if(write(fd[1], &v[i], sizeof(int)) == -1)
					{
						perror("write");
						free(v);
						wait(NULL);
						return 5;
					}
				}
			}
		}
		wait(NULL);
		close(fd[1]);
		return 0;
	}
	else
	{
		wait(NULL);
		
		long long index = 0;
		long long lim = fact(n);
		
		while(index++ < lim)
		{
			for(int i = 0; i < n; ++i)
			{
				if(read(fd[0], &(v[i]), sizeof(int)) == -1)
				{
					perror("read");
					free(v);
					wait(NULL);
					return 6;
				}
			}
			
			for(int i = 0; i < n; ++i)
				printf("%d ", v[i]);
			printf("\n");
		}
		close(fd[0]);
		return 0;
	}
}