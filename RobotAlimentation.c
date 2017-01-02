#include "RobotAlimentation.h"
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Affichage.h"

#define cle 314



void p(int semid) //prologue
{
	char MessageAfficher[200];
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1)
	{
		sprintf(MessageAfficher,"[Erreur] : Opération prologue sur sémaphore");
		affichageConsole(LigneErreur,MessageAfficher);
	}
}

void v(int semid) //epilogue
{
	char MessageAfficher[200];
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1)
	{
		sprintf(MessageAfficher,"[Erreur] : Opération épilogue sur sémaphore");
		affichageConsole(LigneErreur,MessageAfficher);
	}
}


//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotAlimentation(void* arg)
{
	char MessageAfficher[200];
  int i=0;
  messageOperateur msg;

  struct convoyeur* myConvoyeur = arg;



  piece nouvellePiece;
	while(1)
	{
		sprintf(MessageAfficher,"[Robot Alimentation] : en attente de pièces...");
    affichageConsole(LigneRobotAlim,MessageAfficher);
		if((msgrcv(msgid, &msg, (sizeof(msg)-sizeof(long)), 1, 1)) == -1)
		{
			sprintf(MessageAfficher,"[Erreur] : Reception de message");
			affichageConsole(LigneErreur,MessageAfficher);
		}
		nouvellePiece.typePiece=msg.operation;
		for (i=0; i<msg.nbrPiece; i++)
		{
			alimente_convoyeur(nouvellePiece, myConvoyeur);
			v(semid);
		}
		sprintf(MessageAfficher,"[Robot Alimentation] : %d pièces mises sur le convoyeur, opération : %d\n",msg.nbrPiece, nouvellePiece.typePiece);
		affichageConsole(LigneRobotAlim,MessageAfficher);
	}

	sprintf(MessageAfficher,"[Robot Alimentation] : Terminaison du thread");
	affichageConsole(LigneRobotAlim,MessageAfficher);
	pthread_exit(NULL);
}
