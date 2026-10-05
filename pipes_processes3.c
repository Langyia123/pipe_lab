#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  int pipefd1[2];
  int pipefd2[2];
  int pid1, pid2;

  //make sure the user supplied an argument
  if (argc != 2){
    fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
    return 1;
  }
  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  //create two pipes

  pipe(pipefd1);
  pipe(pipefd2);

  //create P2
  pid1 = fork();

  if (pid1 == 0)
    {
      //P2 will execute grep.
      //its input comes from P1 through pipefd1
      dup2(pipefd1[0], STDIN_FILENO);

      //its output goes P3 through pipefd2
      dup2(pipefd2[1], STDOUT_FILENO);

      // close unused pipe descriptors
      close(pipefd1[0]);
      close(pipefd1[1]);
      close(pipefd2[0]);
      close(pipefd2[1]);

      // execute grep using the command line argument

      execvp("grep", grep_args);
      perror("execvp grep");
      exit(1);
    }
  //create P3
  pid2 = fork();

  if (pid2 == 0)
    {
      //P3 executes sort.
      //its input comes from P2 through pipefd2
      dup2(pipefd2[0], STDIN_FILENO);


      // close unused pipe descriptors
      close(pipefd1[0]);
      close(pipefd1[1]);
      close(pipefd2[0]);
      close(pipefd2[1]);

      // execute sort
      execvp("sort", sort_args);
      perror("execvp sort");
      exit(1);
    }
  //P1 (parent) executes cat scores
  //send cat's output into the first pipe.
  dup2(pipefd1[1], STDOUT_FILENO);

  // close unused pipe descriptors
  close(pipefd1[0]);
  close(pipefd1[1]);
  close(pipefd2[0]);
  close(pipefd2[1]);

  // execute cat
  execvp("cat", cat_args);
  perror("execvp cat");
  return 1;
}