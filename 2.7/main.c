#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int opt;

    int bWords = 0, bCharacters = 0, bLines = 0;

    while ((opt = getopt(argc, argv, "wcl")) != -1) {
        switch (opt) {
        case 'w': bWords = 1; break;
        case 'c': bCharacters = 1; break;
        case 'l': bLines = 1; break;
        default:
            fprintf(stderr, "Usage: %s [-wcl] file1 [file2...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if(bWords == 0 && bCharacters == 0 && bLines == 0)
        bWords = bCharacters = bLines = 1;

    int tWords = 0, tCharacters = 0, tLines = 0;
    int fileCount = 0;

    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] == '-') continue;

        fileCount += 1;
        // parsing file i
        FILE * file;
        if((file = fopen(argv[i], "r")) == NULL)
        {
            perror("fopen");
            goto err1;
        }

        int fWords = 0, fCharacters = 0, fLines = 0;

        char c; int prevC;
        while(fread(&c, 1, 1, file))
        {
            fCharacters += 1;
            if(fCharacters == 1)
            {
                fWords = 1;
            }
            else
            if(!strchr(" \r\t", c))
            {
                //fprintf(stderr, "%c is space\n", c);
                if(strchr(" \r\t\n", prevC))
                    fWords += 1;
            }

            if(c == '\n')
            {
                fLines += 1;
            }


            prevC = c;
        }
        if(ferror(file))
        {
            perror("file error");
            fclose(file);
            goto err1;
        }
        
        tLines += fLines;
        tWords += fWords;
        tCharacters += fCharacters;

        if(bLines)
            printf("%d ", fLines);
        if(bWords)
            printf("%d ", fWords);
        if(bCharacters)
            printf("%d ", fCharacters);
        printf("%s\n", argv[i]);
    }

    if(fileCount > 1)
    {
        if(bLines)
            printf("%d ", tLines);
        if(bWords)
            printf("%d ", tWords);
        if(bCharacters)
            printf("%d ", tCharacters);
        printf("total\n");
    }

    if(fileCount == 0)
    {
        fprintf(stderr, "Usage: %s [-wcl] file1 [file2...]\n", argv[0]);
        goto err1;
    }


    exit(EXIT_SUCCESS);

    err1:
    exit(EXIT_FAILURE);

}