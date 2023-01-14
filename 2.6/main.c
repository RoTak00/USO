#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int fsize(int fd);
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDWR | O_SYNC);
    if(fd == -1)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    
    int this_start = 0;
    int this_end = fsize(fd);

    int child1_start, child1_end;
    int child2_start, child2_end;

    while(this_end - this_start > 1)
    {
        child1_start = this_start;
        child1_end = (this_start + this_end) / 2;

        child2_start = (this_start + this_end) / 2;
        child2_end = this_end;

        if(child1_end - child1_start > 1)
        {
            int pid_child1 = fork();
            switch(pid_child1)
            {
                case -1:
                    perror("fork");
                    goto err0;
                // CHILD PROCESS
                case 0:
                    this_start = child1_start;
                    this_end = child1_end;
                    continue;
            

                    break;
                // PARENT PROCESS
                default:
                    
            }
        }
        if(child2_end - child2_start > 1)
        {
            int pid_child2 = fork();
            switch(pid_child2)
            {
                case -1:
                    perror("fork");
                    goto err0;
                // CHILD PROCESS
                case 0:
                    this_start = child2_start;
                    this_end = child2_end;

                    continue;

                    break;
                // PARENT PROCESS
                default:
                    
            }
        }

        while(wait(NULL) != -1) ;

        break;
    }


    // aici parintele interclaseaza cei 2 copii

    // citim in memorie string-urile din jumatatile fisierului
    while(wait(NULL) != -1) ;
    
    int dim1 = child1_end - child1_start;
    int dim2 = child2_end - child2_start;

    char* half1, *half2;
    //printf("sortez (%d) - %d - %d si %d - %d\n", dim1 + dim2, child1_start, child1_end, child2_start, child2_end);
    //printf("getpid() = %d  getppid() = %d - st: %d end: %d\n", getpid(), getppid(), this_start, this_end);
    if((half1 = malloc(sizeof(char) * dim1)) == NULL)
    {
        perror("malloc");
        goto err1;
    }
    if(lseek(fd, child1_start, SEEK_SET) == -1)
    {
        perror("fseek");
        goto err1;
    }
    if(read(fd, half1, dim1) == 0)
    {
        perror("fread");
        goto err1;
    }


    if((half2 = malloc(sizeof(char) * dim2)) == NULL)
    {
        perror("malloc");
        goto err2;
    }
    
    if(read(fd, half2, dim2) == 0)
    {
        perror("fread");
        goto err2;
    }


    //printf("pid %d - half1 = %s half2 = %s\n", getpid(), half1, half2);

    close(fd);
    if((fd = open(argv[1], O_RDWR | O_SYNC)) == -1)
    {
        perror("open2");
        goto err2;
    }


    int index1 = 0, index2 = 0;

    if(lseek(fd, child1_start, SEEK_SET) == -1)
    {
        perror("fseek");
        goto err1;
    }

    while(index1 < dim1 && index2 < dim2)
    {
        if(half1[index1] <= half2[index2])
        {
            //printf("%d - #%d, written %c\n", getpid(), index1 + index2, half1[index1]);
            write(fd, half1 + index1, 1);
            lseek(fd, 0, SEEK_CUR);
            index1 += 1;
        }
        else
        {
            //printf("%d - #%d, written %c\n", getpid(), index1 + index2, half2[index2]);
            write(fd, half2 + index2, 1);
            lseek(fd, 0, SEEK_CUR);
            index2 += 1;
        }
    }

    while(index1 < dim1)
    {
        //printf("%d - #%d, written %c\n", getpid(), index1 + index2, half1[index1]);
        write(fd, half1 + index1, 1);
            lseek(fd, 0, SEEK_CUR);
        index1 += 1;
    }
    while(index2 < dim2)
    {
        //printf("%d - #%d, written %c\n", getpid(), index1 + index2, half2[index2]);
        write(fd, half2 + index2, 1);
        lseek(fd, 0, SEEK_CUR);
        index2 += 1;
    }

    if(lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("fseek");
        goto err2;
    }
    

    

    free(half2);
    free(half1);
    close(fd);

    exit(EXIT_SUCCESS);

    err2: free(half2);
    err1: free(half1);
    err0: close(fd);
    while(wait(NULL) != -1) ;
    exit(EXIT_FAILURE);
}

int fsize(int f)
{
    int flen;
    if((flen = lseek(f, 0, SEEK_END)) == -1)
    {
        perror("SEEK END");
        return -1;
    }

    if(lseek(f, 0, SEEK_SET) == -1)
    {
        perror("FSEEK SET");
        return -1;
    }

    return flen;
}
