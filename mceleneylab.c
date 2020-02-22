#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_ARGS 100
#define BUFFER_SIZE 1000

//checks to make sure ALL white space is removed from the tokens
char *remove_white_space(char* input){
  int i, j;
  char *output = input;
  for(i=0, j=0; i<strlen(input); i++, j++){
    if(input[i] != ' '){
        output[j] = input[i];
    }else{
      j--;
    }
  }
  output[j]='\0';
  return output;
}

void null_out_all_args(char **argv)
{
    //memset: sets everything that param1 points to to the value of param2, for param3 bytes
    memset(argv, 0, sizeof(char *)*MAX_ARGS);
}


int main(){
  bool amp = 0;
  char buffer[BUFFER_SIZE] = "";
  char *argv[MAX_ARGS] = {0};
  char *tokenizer = NULL;
  char *exit_check = "exit";
  char *cd_check = "cd";
  char *amp_check = "&";
  pid_t pid = 0;
  printf("Enter a command or enter 'exit' to leave...\n");

  while(1){
    //sets the ampersand check to false for each run through
    amp = 0;
    printf("Shell> ");
    //read in the input
    char *readin = fgets(buffer, BUFFER_SIZE, stdin);

    //if the input is just whitespace
    if(!(isspace(*readin))){ 
      //parses in the first command
      tokenizer = remove_white_space(strtok(readin, " \n\t"));  // does this line match?
      int i = 0;
      //if the user enters "exit", this exits
      if(strcmp(tokenizer, exit_check) == 0){
        printf("Exiting...\n");
        exit(0);
      }

      do{
        argv[i] = remove_white_space(tokenizer);
        //reads next command
        tokenizer = strtok(NULL, " \n\t");
        i++;
      }while(tokenizer != NULL);

      if(strcmp(argv[i-1], amp_check) == 0){
          amp = 1;
          argv[i-1] = NULL;
        }
      //create a child proces
  if(strcmp(argv[0], cd_check) == 0){
      //printf("hits the inside of cd");
      chdir(argv[1]);
   }else{
      pid = fork();
      if(pid==0){
        //printf("arg2: \"%s\"\n", argv[1]);
        //execute and check for error
        if(execvp(argv[0], argv) == -1){
          perror("ERROR");
          exit(0);
        }
        exit(0);
      }else if(pid == -1){ //if pid -1, there was an error
        perror("ERROR CREATING CHILD");

      }else{
        // this is the amp check
        if(amp == 0){ 
          waitpid(pid, NULL, 0);
        }
      }
    }
    null_out_all_args(argv);
    }
  }
}
