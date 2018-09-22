#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    FILE* input;
    input = fopen("./test.txt", "r");
    char cCharBuf;
    int buf = 0;
    printf("Before read: %d \n", ftell(input));
    fread(&cCharBuf, 1, 1, input);
    printf("Read value: %c \n", cCharBuf);
    printf("After read: %d \n\n", ftell(input));
    return 0;
}