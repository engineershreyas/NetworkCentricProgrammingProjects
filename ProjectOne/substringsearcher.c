#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*written by Shreyas Hirday*/

int main(int argc, char** argv){

    //total amount of substrings to find is the number of arguments minus the executable name and the file name
    int size = argc - 2;

    //filename is the second argument after the executable name
    char* filename = argv[1];

    //create an array for all the substrings passed in
    char* buffer[size];

    //copy substrings into array
    for(int i = 0; i < size; i++){
        buffer[i] = argv[i + 2];
    }



    //variable for counting occurrences of substrings in strings in file
    unsigned int count = 0;

    //pointer to file
    FILE *fp;
    //variable for length of line
    size_t len = 0;
    //variable to denote that line exists
    size_t read;
    //pointer to line
    char* line = NULL;


    //open the file
    fp = fopen(filename,"r");


    //for each line in the file...
    while((read = getline(&line, &len, fp)) != -1) {

        //for each substring passed in
        for(int i = 0; i < size; i++){

          //get the substring
          char* substring =  buffer[i];

          //temp pointer to line in file
          const char* word = line;

          //strstr points to beginning of occurence of substring in string, otherwise returns null
          while((word = strstr(word,substring)) != NULL){
            //increase count for each occurrence of substring found
            count++;
            //move up point for string
            word++;
          }

          //print the count of the substring in the file for the word
          printf("count : %d for %s\n in word %s",count, substring,line);
          //put count back to zero
          count = 0;

        }


    }


    return 0;



}
