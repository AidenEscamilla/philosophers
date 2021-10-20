#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

sem_t forks[5] = {1};
sem_t room = {4};


//int pthread_create(pthread_t *thread, void *(*start_routine) (void *), void *arg);


struct thread_info{

    pthread_t   thread_id;
    int		thread_num;
    int		eatNum;
    int		thinkNum;
    int		timeEating;
    int		timeThinking;

};

static void * philosopher(void *arg){				//this is the "thread_start" function

    struct thread_info *tinfo = arg;
    srand(time(0));
    int randTime;

    printf("philosopher %d is working\n", tinfo->thread_num);
    while(1){
    //think
    tinfo->thinkNum++;
    randTime = (1000 * (rand() % 24 + 25));
    tinfo->timeThinking += randTime;
//    printf("philosopher %d is thinking\n", tinfo->thread_num);
    usleep(randTime);


    sem_wait(&room);
    sem_wait(&forks[tinfo->thread_num]);
    sem_wait(&forks[(tinfo->thread_num + 1) % 5]);

    //eat
    tinfo->eatNum++;
    randTime = (1000 * (rand() % 24 + 25));
    tinfo->timeEating += randTime;
//    printf("philosopher %d is eating\n", tinfo->thread_num);
    usleep(randTime);

    sem_post(&forks[(tinfo->thread_num + 1) % 5]);
    sem_post(&forks[tinfo->thread_num]);
    sem_post(&room);
    }


}



int main (int argc, char *argv[]){
	int s, tnum, opt, num_threads, startTime;
	struct thread_info *tinfo;
	pthread_attr_t attr;


	sem_init(&room, 0, 4);
	for(int i = 0; i < 5; i++){
	sem_init(&forks[i], 0, 1);
	}

	num_threads = argc;	
	s = pthread_attr_init(&attr);
	tinfo = calloc(num_threads, sizeof(struct thread_info));

	startTime = time(NULL);
	for(tnum = 0; tnum < num_threads; tnum++){
	    tinfo[tnum].thread_num = tnum;
	    
	    s = pthread_create(&tinfo[tnum].thread_id, &attr, &philosopher, &tinfo[tnum]);
	    
	 }
	    s = pthread_attr_destroy(&attr);
	while(1){
	    if(difftime(time(NULL), startTime) >= 3){
	   	 for(int j = 0; j < num_threads; j++){
			printf("Philosofer %d ate %d times for %d milliseconds\n", tinfo[j].thread_num, tinfo[j].eatNum, tinfo[j].timeEating/1000);
			printf("Philosofer %d was thinking %d times for %d milliseconds\n", tinfo[j].thread_num, tinfo[j].thinkNum, tinfo[j].timeThinking/1000);
	    		pthread_detach(tinfo[j].thread_id);
	    	}
		break;
	    }
	}

	free(tinfo);
	exit(EXIT_SUCCESS);
}


