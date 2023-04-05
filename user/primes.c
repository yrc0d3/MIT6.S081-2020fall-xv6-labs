#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn))
int
pipeline(int left_fds[2])
{
  close(left_fds[1]);

  int prime;
  if(read(left_fds[0], &prime, 4) != 4){
    exit(0);
  }
  printf("prime %d\n", prime);

  int right_fds[2];
  if (pipe(right_fds) < 0){
    printf("primes: pipe failed\n");
    exit(0);
  }
  if (fork() == 0){ // child
    close(right_fds[1]);
    close(left_fds[0]);
    pipeline(right_fds);
  } else{ // parent
    close(right_fds[0]);
    int next;
    while(read(left_fds[0], &next, 4) == 4){
      if(next%prime == 0){
        continue;
      }
      write(right_fds[1], &next, 4);
    } 
    close(right_fds[1]);
    wait(0);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int fds[2];
  if (pipe(fds) < 0){
    printf("primes: pipe failed\n");
    exit(-1);
  }
  if (fork() == 0){ // child
    pipeline(fds);
  } else { // parent
    close(fds[0]);
    for (int cnt=2; cnt<=35; cnt++){
      write(fds[1], &cnt, 4);
    }
    close(fds[1]);
    wait(0);
  }
  exit(0);
}
