/**
* @file common.c
* Betriebssysteme common code for receiver and sender
* Beispiel 3
*
* @author Dominic Schebeck 			<ic17b049@technikum-wien.at>
* @author Thomas  Neugschwandtner 	<ic17b082@technikum-wien.at>
* @author Dominik Rychly 			<ic17b052@technikum-wien.at>
*
* @date 06/15/2018
*
* @version 1.0
*
*/

/*
* -----------------------------------------includes----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include <sys/mman.h>

#include "common.h"


static unsigned int _bufferSize = -1;
int fd = -1;

static sem_t *semCharsToRead = NULL;
static sem_t *semCharsFree = NULL;
static char *shmptr = NULL;
static int _role;

static char* sem000 = "/sem_2337000";
static char* sem001 = "/sem_2337001";
static char* shm003 = "/shm_2337003";


static unsigned int check_Parameter(const int argc, char* argv[]);
static int initSemShm(void);
static sem_t *create_semaphore(char* name, const int size);
static int unlink_semaphore(char* name);
static int closeSemShm(const int role);
static int create_sharedmemory(void);
static int sendChar(char var);
static int receiveChar(char* pchar);
static void gracefulExit(void);

enum {SUCCESS, FAILURE};


/**
* \brief Start Funktion für Sender Empfänger
*
* \param role enum SENDER, RECEIVER
* \param argc the number of arguments
* \param argv the arguments itselves (including the program name in argv[0])
*
* \return on success "success", exit on Failure with enum FAILURE (through other fx)
* \retval eunm success on success
*/
int run(const int role, const int argc, char* argv[]){
	_role = role;
	_bufferSize = check_Parameter(argc, argv);


	initSemShm();
	create_sharedmemory();
	
	if(role == RECEIVER){
		
		
		int ipcEnd = 0;
		while(ipcEnd == 0){
			char currentChar;
			receiveChar(&currentChar);
			if(currentChar == 0x0 || currentChar == 0xF){
				char asdf;
				receiveChar(&asdf);
				if(currentChar == ~asdf){
					fprintf(stdout,"%c",currentChar);
				}else{
					ipcEnd = 1;
				}
			}else{
				fprintf(stdout,"%c",currentChar);
			}
		}
		
		
		if (fflush(stdout) == EOF)
		{
			fprintf(stderr, "%s: Error flushing stdout", argv[0]);
		}
		
		


	}else{
		int toSend;
		
		while((toSend = getchar())!= EOF){
			char asdf = toSend;
			sendChar(asdf);
			
			if(toSend == 0x0){
				sendChar(0xF);
			}
			if(toSend == 0xF){
				sendChar(0x0);
			}
			
		}	
	
		sendChar(0x0);
		sendChar(0x0);
	}

	closeSemShm(role);
	

	return 0;
}


/**
* \brief versendet ein char über sharedmemmory
*
* \param var zu versendeter Char.
*
* \return on success 0, exit on Failure with enum FAILURE
* \retval on success 0
*/
static int sendChar(const char var){
	static int charPosition = 0;
	
	if(sem_wait(semCharsFree) == -1){
		gracefulExit();
	}

	shmptr[charPosition%_bufferSize] = var;
	
	charPosition++;
	
	if(sem_post(semCharsToRead) == -1){
		gracefulExit();
	}
	return 0;
}


/**
* \brief empfängt ein char über sharedmemmory
*
* \param pchar char pointer indem das empfangen char geschrieben wird.
*
* \return on success 0, exit on Failure with enum FAILURE
* \retval on success 0
*/
static int receiveChar(char* pchar){
	static int charPosition = 0;


	if(sem_wait(semCharsToRead) == -1){
		gracefulExit();
	}
	
	*pchar = shmptr[charPosition%_bufferSize];

	charPosition++;

	if(sem_post(semCharsFree) == -1){
		gracefulExit();
	}

	return 0;
}


/**
* \brief überprüft argc,argv
*
* \param argc the number of arguments
* \param argv the arguments itselves (including the program name in argv[0])
*
* \return on success size of sharedmemmory in char*x, exit on Failure with enum FAILURE
* \retval buffersize
*/
static unsigned int check_Parameter(const int argc, char* argv[])
{	
	int option = 0;
	long bufferSize = 0;
	char* tmp = NULL;
	int error = SUCCESS;
	if (argc < 2)
		error = FAILURE;
	
	
	
	while ((option = getopt(argc, argv, "m:")) != -1){	
		if (option == 'm'){
			bufferSize = strtol(optarg, &tmp, 10);

			if(errno == ERANGE && (bufferSize == LONG_MAX || bufferSize == LONG_MIN) ){
				error = FAILURE;
			}
			
			if (errno != 0 || *tmp != '\0' || bufferSize < 1){	
				error = FAILURE;
			}
		}else{	
			error = FAILURE;
		}
	}

	if (error == FAILURE || optind < argc){
		fprintf(stderr,"USAGE: -m <buffersize>\n");
		exit(FAILURE);
	}

	return bufferSize;
}

/**
* \brief initialisert Semaphore and Sharedmemmory
*
*
* \return on success SUCCESS, exit on Failure with enum FAILURE (through other fx)
* \retval on success SUCCESS
*/
static int initSemShm(){	

	semCharsFree = create_semaphore(sem000,_bufferSize);
	semCharsToRead = create_semaphore(sem001,0);
	

	return SUCCESS;
}

/**
* \brief schliesst Semaphore and Sharedmemmory
*
* \param role enum SENDER, RECEIVER
*
* \return on success SUCCESS, exit on Failure with enum FAILURE (through other fx)
* \retval on success SUCCESS
*/
static int closeSemShm(const int role){	

	if(sem_close(semCharsFree) == -1){
		gracefulExit();
	}
	if(sem_close(semCharsToRead) == -1){
		gracefulExit();
	}

	if(role == RECEIVER){
		unlink_semaphore(sem000);
		unlink_semaphore(sem001);
	}
	
	if(munmap(shmptr, sizeof(char)*_bufferSize) == -1){
		gracefulExit();
	}
	
	if(close(fd) == -1){
		gracefulExit();
	}


	if(role == RECEIVER){
		if(shm_unlink(shm003) == -1){	
			gracefulExit();
		}
	}

	return SUCCESS;
}

/**
* \brief erstellt einzelness Semaphor
*
* \param name char* auf Semaphore Name
* \param size initialwert vom Semaphor
*
* \return on success sem_t*, exit on Failure with enum FAILURE (through other fx)
* \retval on success sem_t*
*/
static sem_t *create_semaphore(char* name, const int size){
	sem_t *sem = sem_open(name,O_CREAT,S_IRWXU,size);
	
	if(sem == SEM_FAILED){
		gracefulExit();
	}

	return sem;
}



/**
* \brief löscht einzelness Semaphor
*
* \param name char* auf Semaphor Name

*
* \return on 0 , exit on Failure with enum FAILURE
* \retval on success sem_t*
*/
static int unlink_semaphore(char* name){
	if(sem_unlink(name) == -1){
		gracefulExit();
	}

	return 0;
}

/**
* \brief Erstellt Sharedmemmory für Programm
*
*
* \return on 0 , exit on Failure with enum FAILURE
* \retval on success 0
*/
static int create_sharedmemory(){
	
	fd = shm_open(shm003,O_CREAT|O_RDWR,S_IRWXU);

	if(fd == -1){
		gracefulExit();
	}

	if(ftruncate(fd, sizeof(char)*_bufferSize) == -1){
		gracefulExit();
	}
	
	shmptr = mmap(NULL, sizeof(char)*_bufferSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd,0);
	if(shmptr == (char* const) MAP_FAILED){
		gracefulExit();
	}
	return 0;

}

/**
* \brief Versuch alles aufzuräumen dann Exit with FAILURE
*
*
*/
static void gracefulExit(){
	if(semCharsToRead != NULL){
		sem_close(semCharsToRead);
	}
	if(semCharsFree != NULL){
		sem_close(semCharsFree);
	}
	
	if(shmptr != NULL&& _role == RECEIVER){
		munmap(shmptr, sizeof(char)*_bufferSize);
		shm_unlink(shm003);
		sem_unlink(sem000);
		sem_unlink(sem001);	
	}

	exit(FAILURE);
}
//------------------------------------eof--------------------------------------------------