#ifndef COMMON_H
#define COMMON_H

/**
* @file common.h
* Betriebssysteme common code for receiver and sender
* Beispiel 3
*
* @author Dominic Schebeck 			<ic17b049@technikum-wien.at>
* @author Thomas  Neugschwandtner 	<ic17b082@technikum-wien.at>
* @author Dominik Rychly 			<ic17b052@technikum-wien.at>
*
* @date 06/11/2018
*
* @version 1.0
*
*/

/*
* -----------------------------------------includes----------------------------------
*/


enum { SENDER, RECEIVER };

//char sem_SENDER[] = "/shm_2337000";
//char sem_RECEIVER[] = "/shm_2337001";

int run (const int role, const int argc, char* argv[]);








#endif // COMMON_H

//------------------------------------eof--------------------------------------------------