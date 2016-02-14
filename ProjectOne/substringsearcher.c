#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char** argv){


    int size = argc - 2;

    char* filename = argv[1];


    char* buffer[size];

    for(int i = 0; i < size; i++){
        buffer[i] = argv[i + 2];
    }

    int count = 0;

    FILE *fp;
    size_t len = 0;
    size_t read;
    char* line = NULL;



    fp = fopen(filename,"r");

    while((read = getline(&line, &len, fp)) != -1) {

      for(int i = 0; i < size; i++){

        char* substring = buffer[i];
        const char *word = line;

        while(word = strstr(word,substring)){
          printf("word: %s",word);
          count++;
          word++;
        }

        printf("count : %d for %s\n",count, substring);
        count = 0;

      }


    }


    return 0;



}
