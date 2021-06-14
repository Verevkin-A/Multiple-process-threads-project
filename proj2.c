#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#define mg 01777

// VUT FIT 2020/2021
// IOS Project 2
// Aleksandr Verevkin (xverev00)


//project definings
int elfs_cnt, rd_cnt, setting, printing, deers_ready, full_queue, santa_go_sleep, get_help, elfs_queue, christmas_started, state, christmas_str, flush;
int *state_value, *christmas_str_value, *flush_value, *rd_cnt_value, *elfs_cnt_value;
sem_t *s_setting, *s_printing, *s_deers_ready, *s_full_queue, *s_get_help, *s_santa_go_sleep, *s_elfs_queue, *s_christmas_started;

//define file
FILE *file;

//memory/semaphores initialization
int initialization(int deer_cnt) {

    setting = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(setting == -1) {
      return 1;
    }
    printing = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(printing == -1) {
      return 2;
    }
    deers_ready = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(deers_ready == -1) {
      return 3;
    }
    full_queue = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(full_queue == -1) {
      return 4;
    }
    get_help = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(get_help == -1) {
      return 5;
    }
    christmas_started = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(christmas_started == -1) {
      return 6;
    }
    elfs_queue = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(elfs_queue == -1) {
      return 7;
    }
    santa_go_sleep = shmget(IPC_PRIVATE, sizeof(sem_t), mg);
    if(santa_go_sleep == -1) {
      return 8;
    }

    flush = shmget(IPC_PRIVATE, sizeof(int), mg);
    if(flush == -1) {
      return 9;
    }
    christmas_str = shmget(IPC_PRIVATE, sizeof(int), mg);
    if(christmas_str == -1) {
      return 10;
    }
    elfs_cnt = shmget(IPC_PRIVATE, sizeof(int), mg);
    if(elfs_cnt == -1) {
      return 11;
    }
    rd_cnt = shmget(IPC_PRIVATE, sizeof(int), mg);
    if(rd_cnt == -1) {
      return 12;
    }
    state = shmget(IPC_PRIVATE, sizeof(int), mg);
    if(state == -1) {
      return 13;
    }

    s_setting = shmat(setting, NULL, 0);
    s_printing = shmat(printing, NULL, 0);
    s_deers_ready = shmat(deers_ready, NULL, 0);
    s_full_queue = shmat(full_queue, NULL, 0);
    s_get_help = shmat(get_help, NULL, 0);
    s_christmas_started = shmat(christmas_started, NULL, 0);
    s_elfs_queue = shmat(elfs_queue, NULL, 0);
    s_santa_go_sleep = shmat(santa_go_sleep, NULL, 0);
    flush_value = shmat(flush, NULL, 0);
    christmas_str_value = shmat(christmas_str, NULL, 0);
    elfs_cnt_value = shmat(elfs_cnt, NULL, 0);
    rd_cnt_value = shmat(rd_cnt, NULL, 0);
    state_value = shmat(state, NULL, 0);

    if(sem_init(s_setting, 1, 1) == -1) {
      return 14;
    }
    if(sem_init(s_printing, 1, 1) == -1) {
      return 15;
    }
    if(sem_init(s_deers_ready, 1, 0) == -1) {
      return 16;
    }
    if(sem_init(s_full_queue, 1, 0) == -1) {
      return 17;
    }
    if(sem_init(s_get_help, 1, 0) == -1) {
      return 18;
    }
    if(sem_init(s_christmas_started, 1, 0) == -1) {
      return 19;
    }
    if(sem_init(s_elfs_queue, 1, 3) == -1) {
      return 20;
    }
    if(sem_init(s_santa_go_sleep, 1, 0) == -1) {
      return 21;
    }

    *flush_value = 1;
    *christmas_str_value = 0;
    *elfs_cnt_value = 0;
    *rd_cnt_value = deer_cnt;
    *state_value = 0;

    return 0;
}

//help function
void memctl(int name) {
  shmctl(name, IPC_RMID, NULL);
}

//main function
int main(int argc, char *argv[]) {

  //arguments count check
  if(argc != 5) {
       printf("Error: there must be 4 additional arguments\n");
       return 1;
     }

  //arguments assigment
  int elfs = atoi(argv[1]);
  int deers = atoi(argv[2]);
  int elfs_time = atoi(argv[3]);
  int deers_time = atoi(argv[4]);

  //arguments check
  if(elfs < 1 || elfs > 999 || deers < 1 || deers > 19 || elfs_time < 0 || elfs_time > 1000 || deers_time < 0 || deers_time > 1000) {
    printf("Error: not supported arguments\n");
    return 2;
  }

  //file opening
  file = fopen("proj2.out", "w");

  //file opening error
  if(file == NULL){
    printf("Error: can't open .out file\n");
    return 3;
  }

  //calling of semaphore initialization function
  int sem_allocation = initialization(deers);
  if(sem_allocation > 0) {
    printf("Error: initialization\n");
    return 7;
  }

  //santa process
  pid_t id = fork();

  //error in case of negative value
  if(id < 0){
    printf("Error: santa forking\n");
    return 4;
  }

  //if process is child, execute
  if(id == 0) {

    while(true) {

      sem_wait(s_printing);
      fprintf(file, "%d: Santa: going to sleep\n", (*flush_value)++);
      fflush(NULL);
      sem_post(s_printing);

      sem_wait(s_full_queue);
      sem_wait(s_setting);
      if(*state_value == 1) {
        break;
      }

      sem_wait(s_printing);
      fprintf(file, "%d: Santa: helping elves\n", (*flush_value)++);
      fflush(NULL);
      sem_post(s_printing);

      sem_post(s_get_help);
      sem_post(s_setting);
      sem_wait(s_santa_go_sleep);
    }

      sem_wait(s_printing);
      fprintf(file, "%d: Santa: closing workshop\n", (*flush_value)++);
      fflush(NULL);
      sem_post(s_printing);

      sem_post(s_deers_ready);
      sem_wait(s_christmas_started);

      sem_wait(s_printing);
      fprintf(file, "%d: Santa: Christmas started\n", (*flush_value)++);
      fflush(NULL);
      sem_post(s_printing);
      *christmas_str_value = 1;

      sem_post(s_setting);
      sem_post(s_get_help);

      //file closing
      fclose(file);

      return 0;
}

  //elfs processes
  for(int i = 1; i <= elfs; i++) {

    id = fork();

    //error in case of negative value
    if(id < 0){

      printf("Error: elfs forking\n");
      return 5;
    }

    //if process is child, execute
    if(id == 0) {

      sem_wait(s_printing);
      fprintf(file, "%d: Elf %d: started\n", (*flush_value)++, i);
      fflush(NULL);
      sem_post(s_printing);

      while(true) {
          if(elfs_time != 0) {
            srand(time(NULL));
            int sleepTime = 1000 * (rand() % elfs_time);
            usleep(sleepTime);
          }

          sem_wait(s_printing);
          fprintf(file, "%d: Elf %d: need help\n", (*flush_value)++, i);
          fflush(NULL);
          sem_post(s_printing);

          sem_wait(s_elfs_queue);
          sem_wait(s_setting);
          if(*christmas_str_value == 1) {

              sem_post(s_setting);
              break;
          }

          *elfs_cnt_value = *elfs_cnt_value + 1;
          if(*elfs_cnt_value == 3) {

              sem_post(s_full_queue);
          }
          sem_post(s_setting);
          sem_wait(s_get_help);

          if(*christmas_str_value == 1) {

              break;
          }
          *elfs_cnt_value = *elfs_cnt_value - 1;

          sem_wait(s_printing);
          fprintf(file, "%d: Elf %d: get help\n", (*flush_value)++, i);
          fflush(NULL);
          sem_post(s_printing);

          if(*elfs_cnt_value == 0) {

              sem_post(s_santa_go_sleep);
              for (int i = 0; i < 3; i++) {

                sem_post(s_elfs_queue);
              }
          } else {

              sem_post(s_get_help);
          }
      }
      sem_wait(s_deers_ready);

      sem_wait(s_printing);
      fprintf(file, "%d: Elf %d: taking holidays\n", (*flush_value)++, i);
      fflush(NULL);
      sem_post(s_printing);

      sem_post(s_deers_ready);
      sem_post(s_elfs_queue);
      sem_post(s_get_help);

      //file closing
      fclose(file);

      return 0;
    }
  }

  //deers processes
  for(int i = 1; i <= deers; i++) {
    id = fork();

    //error in case of negative value
    if(id < 0){
      printf("Error: deers forking\n");
      return 6;
    }

    //if process is child, execute
    if(id == 0) {

      sem_wait(s_printing);
      fprintf(file, "%d: RD %d: rstarted\n", (*flush_value)++, i);
      fflush(NULL);
      sem_post(s_printing);

      if(deers_time != 0) {
        srand(time(NULL));
        int sleepTime = 1000 * ((deers_time / 2) + (rand() % (deers_time / 2)));
        usleep(sleepTime);
      }

        sem_wait(s_printing);
        fprintf(file, "%d: RD %d: return home\n", (*flush_value)++, i);
        fflush(NULL);
        sem_post(s_printing);

        sem_wait(s_setting);
        *rd_cnt_value = *rd_cnt_value - 1;

        if(*rd_cnt_value == 0) {
            *state_value = 1;
            sem_post(s_full_queue);
        }
        sem_post(s_setting);

        sem_wait(s_deers_ready);

        sem_wait(s_printing);
        fprintf(file, "%d: RD %d: get hitched\n", (*flush_value)++, i);
        fflush(NULL);
        sem_post(s_printing);

        *rd_cnt_value = *rd_cnt_value + 1;
        if(*rd_cnt_value == deers) {
            sem_post(s_christmas_started);
        }
        sem_post(s_deers_ready);

      //file closing
      fclose(file);

      return 0;
    }
  }

  //wait for all other processes to end
  while(wait(NULL)) {
    if(errno == 10) {
    break;
  }
}

    //cleaning
    sem_destroy(s_setting);
    sem_destroy(s_printing);
    sem_destroy(s_deers_ready);
    sem_destroy(s_full_queue);
    sem_destroy(s_get_help);
    sem_destroy(s_christmas_started);
    sem_destroy(s_elfs_queue);
    sem_destroy(s_santa_go_sleep);

    memctl(setting);
    memctl(printing);
    memctl(deers_ready);
    memctl(full_queue);
    memctl(get_help);
    memctl(christmas_started);
    memctl(elfs_queue);
    memctl(santa_go_sleep);
    memctl(flush);
    memctl(elfs_cnt);
    memctl(rd_cnt);
    memctl(state);
    memctl(christmas_str);

    //file closing
    fclose(file);

    return 0;
}
