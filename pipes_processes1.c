// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // fd1 handles communication from Parent (P1) to child (P2)
    // fd2 handles communication from child (P2) back to parent (P1)

    int fd1[2];  // Pipe from P1 to P2
    int fd2[2];  // Pipe from P2 back to P1
  
    char fixed_str[] = "howard.edu"; 
    char final_str[] = "gobison.org"; 

    char input_str[100];
    char concat_str[300];
    char second_str[100];

    pid_t p; 
    // create both pipes
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    //P1 gets the first string
    printf("Input : ");
    scanf("%99s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process (P1)
    //sends the first input to P2 through fd1.
    //Receives the modified string from P2 through fd2.
    //Appends "gobison.org" and prints the final result
    else if (p > 0) 
    { 

      //P1 writes through fd1 and reads through fd2
        close(fd1[0]);
        close(fd2[1]);
  
        //send first string to P2 
        if (write(fd1[1], input_str, strlen(input_str)+1) == -1){
          perror("write");
          return 1;
        } 
        close(fd1[1]);
  
        // Get the string back from P2
        if (read (fd2[0], concat_str, sizeof(concat_str)) == -1){
          perror("read");
          return 1;
        }
        close(fd2[0]);
        
        //add gobison.org
        strcat(concat_str, final_str);
        printf("Output : %s\n", concat_str);
        wait(NULL);
    } 
  
    // child process (P2)
    //Receives the first string from P1 through fd1
    //Appends "howard.edu", gets the second user input,
    //and sends the modified string back to P1 through fd2 
    else
    { 
      //P2 reads through fd1 and writes through fd2
        close(fd1[1]);
        close(fd2[0]);
  
        // Receive first string from P1
        if (read (fd1[0], concat_str, sizeof(concat_str)) == -1){
          perror("read");
          exit(1);
        }
        close(fd1[0]);

        //Add howard.edu
        strcat(concat_str, fixed_str);
        printf("Output : %s\n", concat_str);

        //P2 gets second input
        printf("Input : ");
        scanf("%99s", second_str);

        //Append second input
        strcat(concat_str, second_str);

        //send everything back to P1
        if (write(fd2[1], concat_str, strlen(concat_str)+1) == -1){
          perror("write");
          exit(1);
        } 
        close(fd2[1]);
        
        exit(0); 
    } 
    return 0;
} 