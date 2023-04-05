#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fds[2];
  if(pipe(fds) < 0){
    printf("pingpong: pipe failed\n");
    exit(1);
  }
  int pid = fork();
  if(pid == 0){ // child
    char c;
    if (read(fds[0], &c, 1) != 1){
      printf("pingpong: child pipe read failed\n");
      exit(-1);
    }
    close(fds[0]);
    printf("%d: received ping\n", getpid());
    if (write(fds[1], "x", 1) != 1){
      printf("pingpong: child pipe write failed\n");
      exit(-1);
    }
    close(fds[1]);
  } else if (pid > 0){ // parent
    if (write(fds[1], "x", 1) != 1){
      printf("pingpong: parent pipe write failed\n");
      exit(-1);
    }
    close(fds[1]);
    // wait for child to receive ping and write to pipe
    wait((int *) 0);
    char c;
    if (read(fds[0], &c, 1) != 1){
      printf("pingpong: parent pipe read failed\n");
      exit(-1);
    }
    printf("%d: received pong\n", getpid());
    close(fds[0]);
  } else { // fork error
    fprintf(2, "pingpong: fork error\n");
    exit(1);
  }

  exit(0);
}
