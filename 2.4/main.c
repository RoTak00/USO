#include <stdio.h>
#include <stdlib.h>

int compareFilesLexicographically(FILE*, FILE*);
int main(int argc, char* argv[])
{
    FILE * fp1, * fp2;

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        goto err0;
    }

    if((fp1 = fopen(argv[1], "r")) == NULL)
    {
        perror("Open file1");
        goto err1;
    }
    if((fp2 = fopen(argv[2], "r")) == NULL)
    {
        perror("Open file2");
        goto err2;
    }
    int res = compareFilesLexicographically(fp1, fp2);

    if(res == -1)
    {
        printf("%s este lexicografic mai mare\n", argv[1]);
    }
    else if(res == 1)
    {
        printf("%s este lexicografic mai mare\n", argv[2]);
    }
    else if(res == 0)
    {
        printf("%s si %s sunt lexicografic egale\n", argv[1], argv[2]);
    }
    else if(res < -1)
    {
        perror("compare");
        goto err2;
    }

    fclose(fp1);
    fclose(fp2);

    exit(EXIT_SUCCESS);

    
    err2: fclose(fp2);
    err1: fclose(fp1);
    err0:
    exit(EXIT_FAILURE);
}

int compareFilesLexicographically(FILE* f1, FILE* f2)
{
    char c1, c2;
    int bFileIsDone = 0;
    while(!feof(f1) && !feof(f2))
    {
        if(fread(&c1, 1, 1, f1) == 0)
        {
            if(ferror(f1))
                return -100;
            bFileIsDone = 1;
        }
        if(fread(&c2, 1, 1, f2) == 0)
        {
            if(ferror(f2))
                return -100;
            bFileIsDone = 1;
        }
        if(bFileIsDone) break;

        if(c1 > c2)
            return -1;
        else if(c2 > c1)
            return 1;
    }

    if(!feof(f2))
        return 1;
    else if(!feof(f1))
        return -1;
    else
        return 0;

}