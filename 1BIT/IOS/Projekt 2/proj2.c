/*
Projekt 2 do predmetu Operacni systemy na FIT VUT v Brne
Synchronizacny problem rivercrossing

Autor: Jan Jusko

Pouzitie: ./rivercrossing P H S R

P - pocet generovanych osob v kazdej skupine
H - interval generovania hackerov
S - interval generovania serfov
R - interval doby plavby

4.5.2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

//PROTOTYPY FUNKCII
int createSerf(int P, int S, int R);
int args(int P, int H, int S, int R);
int createHacker(int P, int H, int R);
int increment(int *num);
void uspi(int R);

//DEKLARACIA PREMENNYCH SEMAFOROV
sem_t *sem1;
sem_t *sem2;
sem_t *sem3;
sem_t *sem;
sem_t *sem4;
sem_t *sem5;

//DEKLARACIA PREMENNYCH ZDIELANEJ PAMETE
int *zapis;
int *serfmolo;
int *hackermolo;
int *hackercount;
int *serfcount;

FILE *vypis = NULL;

//OVERENIE ARGUMENTOV
int args(int P, int H, int S, int R)
{
    if(P>0 && P%2==0 && H>=0 && H<5001 && S>=0 && S<5001 && R>=0 && R<5001)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//VYTVARANIE HACKEROV
int createHacker(int P, int H, int R)
{
    pid_t id;
    int j;

    for (j=0;j<P;j++)
    {
        uspi(H);            //VYTVARANIE CASOVEHO POSUNU

        id=fork();
        if(id == 0)
        {
            sem_wait(sem3); //ZABEZPECENIE ZAPISU
            fprintf(vypis,"%d: hacker: %d: started\n",increment(zapis),increment(hackercount));
            fflush(vypis);
            sem_post(sem3);

            sem_wait(sem);  //NASTUP NA MOLO HODNOTA SEMAFORU = 4
            increment(hackermolo);
            fprintf(vypis,"%d: hacker: %d: waiting for boarding: %d:  %d\n",increment(zapis),(*hackercount-1),(*hackermolo),(*serfmolo));
            fflush(vypis);
            //OVER CI JE VYTVORENA VHODNA SKUPINA, AK NIE PUSTI DALSIEHO NA MOLO
            if ( (*hackermolo == 3 && (*serfmolo == 1)) || ((*hackermolo == 1) && (*serfmolo == 3)) )
            {
                sem_post(sem);
            }

            //ZISTI AKA SKUPINA SA DOSTALA NA MOLO A PODLA TOHO SPRAV VYPIS DO SUBORU
            if ( ((*hackermolo == 2) && (*serfmolo > 2)) )
            {
                int temph = *hackercount;
                int temps = *serfcount;
               for (int x=0;x<(*serfmolo-1);x++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 0: 1\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 0: 1\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: hacker: %d: captain\n",increment(zapis),temph-1);
               fflush(vypis);

               for (int x=0;x<(*serfmolo-1);x++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo-1);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-y-2);
                   fflush(vypis);
               }

                uspi(R);
                //landing
                for (int x=0;x<(*serfmolo-1);x++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 0: 1\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 0: 1\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }


                *serfmolo=1;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }

            else if(*hackermolo == 4 && *serfmolo == 1)
            {
                int temph = *hackercount;
               for (int y=0;y<*hackermolo;y++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 0: 1\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

                fprintf(vypis,"%d: hacker: %d: captain\n",increment(zapis),temph-1);
                fflush(vypis);

                for (int y=0;y<*hackermolo-1;y++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-y-2);
                   fflush(vypis);
               }

                uspi(R);
               for (int y=0;y<*hackermolo;y++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 0: 1\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

                *serfmolo = 1;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);

            }

            else if(*hackermolo == 4 && *serfmolo == 0)
            {
                int temph = *hackercount;
                for (int x=0;x<*hackermolo;x++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 0: 0\n",increment(zapis),temph-x-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: hacker: %d: captain\n",increment(zapis),temph-1);
               fflush(vypis);

               for (int x=0;x<*hackermolo-1;x++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-x-2);
                   fflush(vypis);
               }

                uspi(R);
               for (int x=0;x<*hackermolo;x++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 0: 0\n",increment(zapis),temph-x-1);
                   fflush(vypis);
               }

                *serfmolo=0;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }

            else if ( ((*hackermolo == 2) && (*serfmolo == 2)) )
            {
                int temph = *hackercount;
                int temps = *serfcount;
               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 0: 0\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: hacker: %d: captain\n",increment(zapis),temph-1);
               fflush(vypis);

               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo-1);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-y-2);
                   fflush(vypis);
               }

                uspi(R);

               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 0: 0\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }


                *serfmolo=0;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }

            exit(0);
        }
        else if (id > 0)
        {
            wait(NULL);
        }
    }
    return 0;
}

//VYTVARANIE SERFOV
int createSerf(int P, int S, int R)
{
    pid_t id2;
    int i;

    for (i=0;i<P;i++)
    {
        uspi(S);            //CASOVY POSUN
        id2=fork();
        if(id2 == 0)
        {
            sem_wait(sem3); //JEDNOZNACNY ZAPIS STARTED
            fprintf(vypis,"%d: serf: %d: started\n",increment(zapis),increment(serfcount));
            fflush(vypis);
            sem_post(sem3);

            sem_wait(sem);  //PUST 4 NA MOLO
            increment(serfmolo);
            fprintf(vypis,"%d: serf: %d: waiting for boarding: %d: %d\n",increment(zapis),(*serfcount)-1,(*hackermolo),(*serfmolo));
            fflush(vypis);

            //AK NEUTVORILI VHODNU SKUPINU TAK PUST DALSIEHO
            if ( (*hackermolo == 3 && (*serfmolo == 1)) || ((*hackermolo == 1) && (*serfmolo == 3)) )
            {
                sem_post(sem);
            }

            //ZISTI AKU SKUPINU UTVORILI A PODLA TOHO VYPIS
            //SKUPINA 3 HACKERS 2 SERFS
            if ( ((*hackermolo > 2) && (*serfmolo == 2)) )
            {
                int temph = *hackercount;
                int temps = *serfcount;
               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 1: 0\n",increment(zapis),*serfcount-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo-1);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 1: 0\n",increment(zapis),*hackercount-y-2);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: serf: %d: captain\n",increment(zapis),temps-1);
               fflush(vypis);

               for (int x=0;x<(*serfmolo-1);x++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo-1);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-y-2);
                   fflush(vypis);
               }

                uspi(R);

                for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 1: 0\n",increment(zapis),*serfcount-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo-1);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 1: 0\n",increment(zapis),*hackercount-y-2);
                   fflush(vypis);
               }

                *serfmolo=0;
                *hackermolo=1;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }
            //SKUPINA 4 HACKERS 1 SERF
            else if(*serfmolo == 4 && *hackermolo == 1)
            {
               int temps = *serfcount;
               for (int y=0;y<*serfmolo;y++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 1: 0\n",increment(zapis),temps-y-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: serf: %d: captain\n",increment(zapis),temps-1);
               fflush(vypis);

               for (int y=0;y<*serfmolo;y++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-y-2);
                   fflush(vypis);
               }

                uspi(R);

               for (int y=0;y<*serfmolo;y++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 1: 0\n",increment(zapis),temps-y-1);
                   fflush(vypis);
               }

                *hackermolo=1;
                *serfmolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }
            //SKUPINA 0 HACKER 4 SERF
            else if(*serfmolo == 4 && *hackermolo == 0)
            {
                int temps = *serfcount;
                for (int x=0;x<*serfmolo;x++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: serf: %d: captain\n",increment(zapis),temps-1);
               fflush(vypis);

               for (int x=0;x<*serfmolo-1;x++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               uspi(R);

               for (int x=0;x<*serfmolo;x++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

                *serfmolo=0;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }
            //SKUPINA 2 HACKER 2 SERF
            else if ( ((*hackermolo == 2) && (*serfmolo == 2)) )
            {
                int temph = *hackercount;
                int temps = *serfcount;
               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: boarding: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: boarding: 0: 0\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

               fprintf(vypis,"%d: serf: %d: captain\n",increment(zapis),temps-1);
               fflush(vypis);

               for (int x=0;x<(*serfmolo-1);x++)
               {
                   fprintf(vypis,"%d: serf: %d: member\n",increment(zapis),temps-x-2);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: member\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }
               uspi(R);

               for (int x=0;x<(*serfmolo);x++)
               {
                   fprintf(vypis,"%d: serf: %d: landing: 0: 0\n",increment(zapis),temps-x-1);
                   fflush(vypis);
               }

               for (int y=0;y<(*hackermolo);y++)
               {
                   fprintf(vypis,"%d: hacker: %d: landing: 0: 0\n",increment(zapis),temph-y-1);
                   fflush(vypis);
               }

                *serfmolo=0;
                *hackermolo=0;
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
                sem_post(sem);
            }

            exit(0);
        }
        else if (id2 > 0)
        {
            wait(NULL);
        }
    }
    return 0;
}

//BEZPECNA INKREMENTACIA ZDIELANEJ PREMENNEJ
int increment(int *num)
{
    int old=0;
    sem_wait(sem2);
    old = *num;
    (*num)++;
    sem_post(sem2);

    return old;
}

//FUNKCIA VYTVARA USPANIE PROCESU NA NAHODNU HODNOTU Z INTERVALU
void uspi(int R)
{
    int cas=0;

    if(R!=0)
    {
        cas = (rand() % (R + 1));
    }
    else
    {
        cas = 0;
    }
    usleep(cas*1000);
}


//***********************************************  MAIN  *************************************************


int main(int argc, char *argv[])
{

    int P,H,S,R;
    int err1=1;
    srand(time(NULL));         //RANDOM
    vypis = fopen("rivercrossing.out", "w+");   //OTVOR SUBOR


    if(argc == 5)           //OVER SPRAVNOST ARGUMENTOV
    {
        P=atoi(argv[1]);
        H=atoi(argv[2]);
        S=atoi(argv[3]);
        R=atoi(argv[4]);
        err1=args(P,H,S,R);
    }

    if(err1 == 1)
    {
        fprintf(stderr,"Chyba vstupu!\n");
        return 1;
    }

//SEMAPHORE INITIALIZATION
    sem = sem_open("/Sem", O_CREAT | O_EXCL, 0666, 4);
    sem1 = sem_open("/Sem1", O_CREAT | O_EXCL, 0666, 1);
    sem2 = sem_open("/Sem2", O_CREAT | O_EXCL, 0666, 1);
    sem3 = sem_open("/Sem3", O_CREAT | O_EXCL, 0666, 1);
    sem4 = sem_open("/Sem4", O_CREAT | O_EXCL, 0666, 1);
    sem5 = sem_open("/Sem5", O_CREAT | O_EXCL, 0666, 0);


//SHARED MEMORY VARIABLES
    int shmkey=1259;
    int shmid_zapis = shmget(shmkey, sizeof (int), 0644 | IPC_CREAT);
    zapis= (int *) shmat (shmid_zapis, NULL,0);
    *zapis=1;

    shmkey=1000;
    int shmid_serfmolo = shmget(shmkey, sizeof (int), 0644 | IPC_CREAT);
    serfmolo= (int *) shmat (shmid_serfmolo, NULL,0);
    *serfmolo=0;

    shmkey=1001;
    int shmid_hackermolo = shmget(shmkey, sizeof (int), 0644 | IPC_CREAT);
    hackermolo= (int *) shmat (shmid_hackermolo, NULL,0);
    *hackermolo=0;

    shmkey=1002;
    int shmid_hackercount = shmget(shmkey, sizeof (int), 0644 | IPC_CREAT);
    hackercount= (int *) shmat (shmid_hackercount, NULL,0);
    *hackercount=1;

    shmkey=1005;
    int shmid_serfcount = shmget(shmkey, sizeof (int), 0644 | IPC_CREAT);
    serfcount= (int *) shmat (shmid_serfcount, NULL,0);
    *serfcount=1;

    setbuf(vypis, NULL);

    pid_t pid1, pid2;           //VYTVORENIE HLAVNYCH PROCESOV


    pid1 = fork();
    if (pid1 == 0)  //HLAVNY HACKER PROCES
    {
        createHacker(P,H,R);
        exit(0);
    }
    else if (pid1>0) //HLAVNY PROCES
    {
        pid2 = fork();
        wait(NULL);
        if(pid2 == 0)//HLAVNY SERF PROCES
        {
            createSerf(P,S,R);
            exit(0);
        }
        else if(pid2 > 0)
        {
            wait(NULL);
        }
        else if(pid2 < 0)
        {
            fprintf(stderr,"Hlavny proces SERF sa nepodarilo vytvorit.");
        }

        wait(NULL);
    }
    else if (pid1<0)
    {
        fprintf(stderr,"Hlavny proces HACKER sa nepodarilo vytvorit.");
    }

    for(int i=1; i<*hackercount; i++)
    {
        fprintf(vypis,"%d: hacker: %d: finished\n",increment(zapis),i);
        fflush(vypis);
        fprintf(vypis,"%d: serf: %d: finished\n",increment(zapis),i);
        fflush(vypis);
    }

    //UVOLNENIE PAMETE A SEMAFOROV

    sem_close(sem);
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem3);
    sem_close(sem4);
    sem_close(sem5);

    sem_unlink("/Sem");
    sem_unlink("/Sem1");
    sem_unlink("/Sem2");
    sem_unlink("/Sem3");
    sem_unlink("/Sem4");
    sem_unlink("/Sem5");


    shm_unlink("/sharedMemory");
    close(shmid_hackercount);
    close(shmid_serfcount);
    close(shmid_hackermolo);
    close(shmid_serfmolo);
    close(shmid_zapis);

    shmdt(serfmolo);
    shmdt(zapis);
    shmdt(hackermolo);
    shmdt(hackercount);
    shmdt(serfcount);

    return 0;
}

