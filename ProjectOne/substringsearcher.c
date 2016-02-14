#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_SIZE 1024

/*written by Shreyas Hirday*/

int main(int argc, char** argv){

    //total amount of substrings to find is the number of arguments minus the executable name and the file name
    int size = argc - 2;

    //check if flag for system call was passed in
    int isFlag = strcmp(argv[1],"-s") == 0 ? 1 : 0;

    float secs_used,micros_used;
    struct timeval start,end;



    //if flag wasn't used...
    if(!isFlag){

      gettimeofday(&start,NULL);
      //filename is the second argument after the executable name
      char* filename = argv[1];

      if(filename == NULL || strlen(filename) < 1){
        printf("Oops, you must enter a filename!\n");
        return 1;
      }


      char* truefilename = malloc(strlen(filename) + strlen(".txt"));
      strcpy(truefilename, filename);
      strcat(truefilename,".txt");




      //create an array for all the substrings passed in
      char* buffer[size];

      //copy substrings into array
      for(int i = 0; i < size; i++){
        char* str = argv[i + 2];
        //make all characters lower case to ensure case-sensitiveness
        for(int j = 0;str[j];j++){
          str[j] = tolower(str[j]);
        }

        buffer[i] = str;
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
      fp = fopen(truefilename,"r");
      if(fp == NULL){
        printf("File does not exist!\n");
        return 2;
      }

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
            printf("%d\n",count);
            //put count back to zero
            count = 0;

          }


      }

      gettimeofday(&end,NULL);

      secs_used = (end.tv_sec - start.tv_sec);
      micros_used = (secs_used * 1000000 + end.tv_usec) - (start.tv_usec);
      printf("milliseconds passed: %f\n",micros_used/1000);

    }
    //if flag was passed in
    else{

        gettimeofday(&start,NULL);
        //get filename
        char* filename = argv[2];

        if(filename == NULL || strlen(filename) < 1){
          printf("Oops, please enter a filename!\n");
          return 1;
        }

        //number of substrings is number of args minus executable name, file name, and flag
        int size = argc - 3;

        //allocate array for substrings passed
        char* substrings[size];

        //copy passed in substrings
        for(int i = 0; i < size;i++){
          char* str = argv[i + 3];
          //make all characters lower case to ensure case-sensitiveness
          for(int j = 0;str[j];j++){
            str[j] = tolower(str[j]);
          }
          substrings[i] = str;
        }

        //eliminate need for user to enter .txt at end of file name
        char* result = malloc(strlen(filename) + strlen(".txt"));
        strcpy(result,filename);
        strcat(result,".txt");


        //get file descriptor of file to read
        int file_descriptor = open(result,O_RDONLY);
        //make sure file exists
        if(file_descriptor == -1){
          printf("File doesn't exist!\n");
          return 2;
        }

        //allocate string buffer for a line in the file
        char buffer[MAX_SIZE];
        //lets us know if string exists or not
        int n = 0;
        //variable for count of substring
        int count = 0;

        //for each line in the file
        while((n = read(file_descriptor,buffer,sizeof(buffer))) != 0) {

          //for each substring passed in
          for(int i = 0; i < size; i++){

            //get the substring
            char* substring =  substrings[i];

            //temp pointer to line in file
            const char* word = buffer;

            //strstr points to beginning of occurence of substring in string, otherwise returns null
            while((word = strstr(word,substring)) != NULL){
              //increase count for each occurrence of substring found
              count++;
              //move up point for string
              word++;
            }

            //print the count of the substring in the file for the word
            printf("%d\n",count);
            //put count back to zero
            count = 0;

          }



        }

        gettimeofday(&end,NULL);

        secs_used = (end.tv_sec - start.tv_sec);
        micros_used = (secs_used * 1000000 + end.tv_usec) - (start.tv_usec);
        printf("milliseconds passed: %f\n",micros_used/1000);


    }



    return 0;



}
