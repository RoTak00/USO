#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[])
{

    FILE * fp1, * fp2;
    unsigned f2len;
    unsigned app = 0;
    unsigned app_pos = 0;

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        goto err0;
    }

    
    if((fp1 = fopen(argv[1], "r")) == NULL)
    {
        perror("fp1 open");
        goto err1;
    }

    if((fp2 = fopen(argv[2], "r")) == NULL) 
    {
        perror("fp2 open");
        goto err2;
    }

    // get length of file2
    if(fseek(fp2, 0, SEEK_END) == -1)
    {
        perror("fseek fp2");
        goto err2;
    }
    if((f2len = ftell(fp2)) == -1)
    {
        perror("ftell fp2");
        goto err2;
    }
    if(fseek(fp2, 0, SEEK_SET) == -1)
    {
        perror("fseek fp2");
        goto err2;
    }


    char c1, c2;
    int seq_len = 0;
    while(1)
    {
        if(fread(&c1, 1, 1, fp1) == 0)
        {
            if(ferror(fp1))
            {
                perror("fread fp1");
                goto err2;
            }
            // we've finished the first file, whatever we could have found was found
            if(feof(fp1))
            {
                //printf("Done!\n");
                break;
            }
        }

        if(fread(&c2, 1, 1, fp2) == 0)
        {
            if(ferror(fp2))
            {
                perror("fread fp2");
                goto err2;
            }
            // we always rewind fp2 when reaching it's end, this should not happen
            if(feof(fp2))
            {
                fprintf(stderr, "This shouldn't have happened\n");
                goto err2;
            }
        }

        //printf("l %2d %c - %c\n", seq_len, c1, c2);

        // current secquence ok
        if(c1 == c2)
        {
            seq_len += 1;

            // if we found the whole secquence
            if(seq_len == f2len)
            {
                app += 1;

                if(fseek(fp2, 0, SEEK_SET) == -1)
                {
                    perror("fseek fp2");
                    goto err2;
                }

                if(fseek(fp1, -seq_len + 1, SEEK_CUR) == -1)
                {
                    perror("fseek fp1");
                    goto err2;
                }

                seq_len = 0;
            }
        }
        // reset
        else
        {
            if(fseek(fp2, 0, SEEK_SET) == -1)
            {
                perror("fseek fp2");
                goto err2;
            }

            if(fseek(fp1, -seq_len, SEEK_CUR) == -1)
            {
                perror("fseek fp1");
                goto err2;
            }

            seq_len = 0;
        }
    }


    if(app == 0)
    {
        printf("Fisierul %s nu apare in %s\n", argv[2], argv[1]);
    }
    else
    {
        printf("Fisierul %s apare de %2d ori in fisierul %s\n", argv[2], app, argv[1]);
    }

    fclose(fp2);
    fclose(fp1);

    exit(EXIT_SUCCESS);

    err3:
    err2: fclose(fp2);
    err1: fclose(fp1);
    err0:
    exit(EXIT_FAILURE);
}