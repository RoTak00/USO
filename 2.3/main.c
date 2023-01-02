#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int mymv(char* from, char* to);

char* getdir(char* path);

int main(int argc, char* argv[])
{

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s [file1] [file2]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(mymv(argv[1], argv[2]) == -1)
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

int mymv(char* from, char* to)
{
    static struct stat *sbuf;
    static char* dstfolder;

    int srcdev, dstdev;
    int srcnlink;

    sbuf = (struct stat*)malloc(sizeof(struct stat));
    if(sbuf == NULL)
    {
        perror("MALLOC STRUCT STAT");
        goto err1;
    }
    

    if(stat(from, sbuf) == -1)
    {
        perror("STAT SRC");
        goto err2;
    }
    
    srcdev = sbuf->st_dev;
    srcnlink = sbuf->st_nlink;

    int bDestExists = 1;
    // verificam daca exista fisierul 2
    if(stat(to, sbuf) == -1)
    {
        if(errno = ENOENT)
            bDestExists = 0;
        else
        {
            perror("STAT DEST");
            goto err2;
        }
    }

    if(bDestExists == 1)
        dstdev = sbuf->st_dev;
    
    if(bDestExists == 0)
    {
        dstfolder = getdir(to);
        if(dstfolder == NULL)
        {
            perror("GETDIR FUNCTION");
            goto err3;
        }

        if(stat(dstfolder, sbuf) == -1)
        {
            perror("STAT DEST FOLDER");
            goto err3;
        }
        dstdev = sbuf->st_dev;
    }


    if(srcdev == dstdev)
    {
        printf("Fisierele / folderele se afla pe acelasi disc (%d).\n", srcdev);

        if(bDestExists == 1)
        {
            printf("%s exista deja. Se reasociaza numele.\n", to);

            if(unlink(to) == -1)
            {
                perror("ULINK 1 | DEV = | EXISTS");
                goto err3;
            }   

            if(link(from, to) == -1)
            {
                perror("LINK 1 | DEV = | EXISTS");
                goto err3;
            }
            
            if(unlink(from) == -1)
            {
                perror("ULINK 2 | DEV = | EXISTS");
                goto err3;
            }
            
            goto success;
        }
        else if(bDestExists == 0)
        {
            printf("%s nu exista. Se inlocuieste legatura fizica.\n", to);

            if(link(from, to) == -1)
            {
                perror("LINK 1 | DEV = | NEXISTS");
                goto err3;
            }

            if(unlink(from) == -1)
            {
                perror("ULINK 1 | DEV = | NEXISTS");
                goto err3;
            }

            goto success;
        }
    }
    else if(srcdev != dstdev)
    {
        printf("Fisierele / folderele se afla pe discuri diferite (%d si %d)\n", srcdev, dstdev);

        if(srcnlink == 1)
        {
            printf("Fisierul %s are o singura legatura fizica pe discul sursa. Se muta fisierul byte cu byte la %s.\n", from, to);

            int srcd = open(from, O_RDONLY);
            if(srcd == -1)
            {
                perror("OPEN SRC FOR MOVE");
                close(srcd);
                goto err3;
            }

            int dstd = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if(dstd == -1)
            {
                perror("OPEN DST FOR MOVE");
                close(srcd);
                close(dstd);
                goto err3;
            }

            char c;
            int rerr;
            while(rerr = read(srcd, &c, 1))
            {
                if(rerr == -1)
                {
                    perror("READ SRC");
                    close(srcd);
                    close(dstd);
                    goto err3;
                }

                if(write(dstd, &c, 1) == -1)
                {
                    perror("WRITE DST");
                    close(srcd);
                    close(dstd);
                    goto err3;
                }
            }

            close(srcd);
            close(dstd);

            if(unlink(from) == -1)
            {
                perror("ULINK 1 | DEV !=");
                goto err3;
            }
            goto success;
        }

        else if(srcnlink > 1)
        {
            printf("Fisierul %s are mai multe legaturi fizice pe discul sursa. Se creaza in %s o legatura simbolica la %s\n", from, to, from);
            if(symlink(from, to) == -1)
            {
                perror("SYMLINK 1 | DEV !=");
                goto err3;
            }
            goto success;
        }
    }

    success:

    free(dstfolder);
    free(sbuf);

    return 0;

    err3: free(dstfolder);
    err2: free(sbuf);
    err1:
    return -1;
    
}

char* getdir(char* path)
{
    char* res;
    if((res = (char*)malloc(sizeof(char) * strlen(path))) == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    strcpy(res, path);

    char* pos = strrchr(res, '/');

    if(pos == NULL)
    {
        if(realloc(res, 4) == NULL)
        {
            perror("realloc in getdir");
            exit(EXIT_FAILURE);
        }
        strcpy(res, ".");

        return res;
    }
    *pos = '\0';

    return res;

}