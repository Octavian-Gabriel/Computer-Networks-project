#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <termios.h>
#include<string.h>

extern int errno;
int port;
struct client_logat{ char username[25],parola[25];};
struct client_logat cli_On;
int creare_Socket(int nr, char* x[])
{
  int sd=0;
   if(nr!=3){
		printf ("Sintaxa: %s <adresa_server> <port>\n", x[0]);
      return -1;
	}
    port = atoi(x[2]);
     sd = socket(AF_INET,SOCK_STREAM,0) ;
     if(sd==-1 )
   {
   		perror("eroare la socket client\n");
   		return errno;
   }
   return sd;
}

//LOGARE CLIENT
void preluare_si_trimitere_info_logare(int *y){//paramatru = socket
   struct logare_client{
    char  username[25];
  char  parola [25]; 
 };
   struct logare_client c;
   
 
     printf("Va rugam sa introduceti urmatoarele informatii: \n");
       printf("Username: ");
      scanf("%s", c.username);
      strcpy(cli_On.username,c.username);
       printf("Parola:");
       scanf("%s", c.parola);
       strcpy(cli_On.parola,c.parola);
       int cod=2;
       if(send(*y,&cod,sizeof(cod),0)<=0){
         printf("client -> eroare la trimitere cod logare \n");
         printf("%s",(char*)errno);
       }
       if(send(*y,&c,sizeof(struct logare_client),0)<=0){
         printf("client -> eroare la trimitere info logare \n ");
         printf("%s",(char*)errno);
       }
}

//INREGISTRARE CLIENT
void preluare_si_trimitere_info_inregistrare(int * y)
{
  struct inregistrare_client {
  char username[25];char parola[25] ;char nume[25];
 char  prenume[25]; char  tip[6] ;
 
 };
  
  struct inregistrare_client c;char* p2;p2 = malloc(sizeof(char));
  
      printf("Va rugam sa introduceti urmatoarele informatii: \n");
      printf("Numele dvs: ");
      scanf("%s", c.nume);
      printf("Prenumele dvs: ");
      scanf("%s", c.prenume);
      printf("Username: ");
      scanf("%s", c.username);
       
      int ok=0;
      while (!ok)
      {
      printf("Parola:   ");
      scanf("%s", c.parola);
      printf("Introduceti din nou parola:");
      scanf("%s", p2);
        if(strcmp(c.parola,p2)!=0)
      {
        printf("Parolele nu sunt identice! \n Incercati din nou \n");
        ok=0;
      }
      else {printf("Parolele sunt identice! \n");ok=1;}
      }
      ok=0;
      while(!ok){
      printf("Tipul de client: 1-obisnuit sau 2-administrator: ");
      int n;
      scanf("%d",&n);
       if(n==1) {strcpy(c.tip,"comun");ok=1;}
       else if(n==2){strcpy(c.tip,"admin");ok=1;}
       else printf("Valoare gresita \n.Introduceti o valoare corecta \n");
      }
      printf("\n");
   int cod=1;
  if(send(*y,&cod,sizeof(cod),0)<=0)
      {
        perror ("[client] Eroare la trimitere cod 1 spre server");
       printf("%s \n",(char* )errno);
      }
  if (send (*y,&c,sizeof(struct inregistrare_client),0) <= 0)
    {
      perror ("[client]Eroare la write() inregistrare spre server.\n");
     printf("%s \n",(char* )errno);
    }
  if(read(*y,&ok,sizeof(ok))<=0){
    printf("client -> eroare la primire da/nu inregistrare \n");
    printf("%s \n",(char*)errno);
  }
  if(ok==1)printf("Ati fost inregistrat! \n");
  else printf("Nu ati fost inregistrat.Incercati din nou! \n");  
    
} 
// ADAUGARE MELODIE
void preluare_trimitere_info_adaugare_melodie(int* a) //param:socket
{
  int cod=3;
    if(send(*a,&cod,sizeof(int),0)<=0){
    printf("client-> eroare la trimitere cod  adaugare melodie \n");
    printf("%s \n",(char*) errno);
  }
  struct melodie{char artist[25],nume[25],gen[15],link[150],descriere[200];};
  struct melodie m;
  printf("Va rugam sa dati urmatoarele informatii: \n");
  printf("Numele melodiei: \n");scanf("%s", m.nume);
  printf("Artist sau formatie: \n");scanf("%s", m.artist);
  printf("Genul in care se incadreaza melodia: \n");scanf("%s", m.gen); 
  printf("Dati un link la care se poate gasi melodia \n");scanf("%s",m.link);
   int d;
  printf("Doriti sa adaugati o descriere a melodiei? 1.Da sau 2.Nu");scanf("%d",&d);
  switch(d){
    case 1:{printf("Descriere: ");scanf("%[^\n]%*c", m.descriere); break;}
    default: break;
         }
    
  if(send(*a,&m,sizeof(struct melodie),0)<=0){
    printf("client-> eroare la trimitere info adaugare melodie \n");
    printf("%s \n",(char*) errno);
  }
}

//TOP MUZICAL GENERAL
 void cerere_top_general(int *a){
    int cod=4;
    if(write(*a,&cod,sizeof(int))<=0){
      printf("client -> eroare la trimitere cod top general \n");
      printf("%s\n",(char*)errno);
    }
   //primesc mai intai nr de melodii din top
   int nr_m;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez topul efectivn
printf("Topul contine %d melodii: \n",nr_m);
 for(int i=0;i<nr_m;++i){
   char m[100];
      if(recv(*a,m,sizeof(m),0)<=0){
        printf("client -> eroare la primire melodie top general \n");
       printf("%s \n",(char*) errno);
      }
      else { 
         printf("Melodia de pe locul %d: %s\n",i+1,m);
         bzero(m,100);
      }
    }
    printf("\n\n");
 }
 
 //TOP PENTRU GEN MUZICAL
 void cerere_top_gen(int * a,char* g){
   int cod=5;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod top pe gen \n");
      printf("%s\n",(char*)errno);
    }
    if(write(*a,g,sizeof(25))<=0){
      printf("client -> eroare la trimitere cod top pe gen \n");
      printf("%s\n",(char*)errno);
    }
    //primesc mai intai nr de melodii din top
   int nr_m;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez topul 
printf("Topul contine %d melodii: \n",nr_m);
 for(int i=0;i<nr_m;++i){
   char m[100];
      if(recv(*a,m,sizeof(m),0)<=0){
        printf("client -> eroare la primire melodie top general \n");
       printf("%s \n",(char*) errno);
      }
      else { 
         printf("Melodia de pe locul %d: %s\n",i+1,m);
         bzero(m,100);
      }
    }
    printf("\n\n");
 }

// GENURI MUZICALE
void  cerere_genuri(int * a){
  int cod=6;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod top pe gen \n");
      printf("%s\n",(char*)errno);
    }
    int nr_m;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez genuri
printf("Puteti alege dintre urmatoarele genuri muzicale\n");
 for(int i=0;i<nr_m;++i){
   char m[100];
      if(recv(*a,m,sizeof(m),0)<=0){
        printf("client -> eroare la primire gen muzical \n");
       printf("%s \n",(char*) errno);
      }
      else { 
         printf("%d. %s\n",i+1,m);
         bzero(m,100);
      }
    }
    char* gen=malloc(sizeof(char));
    printf("Numele genului dorit: \n");scanf("%s",gen);
    cerere_top_gen(a,gen);
}


//  VOTARE MELODIE
void vot_melodie(int *a){
   /* 1.verific daca user are drept de vot */
  int cod=7;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod drept vot user \n");
      printf("%s\n",(char*)errno);
    }
   if(write(*a,&cli_On,sizeof(struct client_logat))<=0){
      printf("client -> eroare la trimitere info client on \n");
      printf("%s\n",(char*)errno);
    }
    int vot;
    if(read(*a,&vot,sizeof(vot))<=0){
      printf("client ->eroare la primire calitate vot \n");
      printf("%s \n",(char*)errno);
    }
    if(vot==1){  /*pun toate melodiile 
      ca la topul general si cer id-ul melodiei */ 
    int cod=8;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod lista melodii\n");
      printf("%s\n",(char*)errno);
    }
    //primesc mai intai nr de melodii din top
   int nr_m=0;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez lista de melodii
printf("Lista completa de melodii:");printf("\n");
 for(int i=0;i<nr_m;i++){
   char m[100],arti[100];
      if(read(*a,m,sizeof(m))<=0){
        printf("client -> eroare la primire nume melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
     if(read(*a,arti,sizeof(arti))<=0){
        printf("client -> eroare la primire artist melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
      printf("%d.%s-%s",i+1,arti,m);printf("\n");
     
    } 
  cod=9;
  if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere  cod votare melodie \n");
      printf("%s\n",(char*)errno);
    }
    printf("Numarul  melodiei pe care doriti sa o votati: ");
    int nr;
    scanf("%d",&nr);
   
  if(write(*a,&nr,sizeof(nr))<=0){
      printf("client -> eroare la trimitere  id melodie de votat\n");
      printf("%s\n",(char*)errno);
    }
    //astept raspuns 1-da 0-nu
    int raspuns;
    if(read(*a,&raspuns,sizeof(raspuns))<=0){
        printf("client -> eroare la primire raspuns rezultat vot \n");
       printf("%s \n",(char*) errno);
      }
    if(raspuns ==1)printf("Vot dvs a fost inregistrat cu succes! \n");
    else printf("Votul dvs nu a fost inregistrat! \n");
    printf("\n\n");
    }
    else printf("Dreptul de vot v-a fost retras! \n\n");
}

//ADAUGARE COMENTARIU MELODIE
void com_melodie(int *a){
     int cod=8;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod lista melodii\n");
      printf("%s\n",(char*)errno);
    }
    //primesc mai intai nr de melodii din top
   int nr_m=0;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez lista de melodii
printf("Lista completa de melodii:");printf("\n");
 for(int i=0;i<nr_m;i++){
   char m[100],arti[100];
      if(read(*a,m,sizeof(m))<=0){
        printf("client -> eroare la primire nume melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
     if(read(*a,arti,sizeof(arti))<=0){
        printf("client -> eroare la primire artist melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
      printf("%d.%s-%s",i+1,arti,m);printf("\n");
     
    } 
    cod=10;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod lista melodii\n");
      printf("%s\n",(char*)errno);
    }
    printf("Numarul  melodiei la care doriti sa adaugati un comentariu: ");
    int nr;
    scanf("%d",&nr);
    if(write(*a,&nr,sizeof(nr))<=0){
      printf("client -> eroare la trimitere  id melodie de comentat\n");
      printf("%s\n",(char*)errno);
    }
    else {
      char com[100];
      printf("Comentariul dvs :");
    char x; scanf("%c",&x);
     fgets(com, 100, stdin);
    if(write(*a,com,sizeof(com))<=0){
      printf("client -> eroare la trimitere comentariu melodie \n");
      printf("%s \n",(char *)errno);
    }
    //astept raspuns 1-ok 0-!ok
    int raspuns ;
    if(read(*a,&raspuns,sizeof(raspuns))<=0){
      printf("client ->eroare la primire raspuns adaugare comentariu melodie \n");
      printf("%s \n",(char *)errno);
    }
    else if(raspuns==1)printf("Comentariul a fost adaugat cu succes. ");
            else printf("Comentariul nu a fost adaugat. ");
    }

    printf("\n\n");
}

//STERGERE MELODIE
void sterge_melodie(int *a){
   int cod=8;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod lista melodii\n");
      printf("%s\n",(char*)errno);
    }
    //primesc mai intai nr de melodii din top
   int nr_m=0;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire nr melodii top general de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez lista de melodii
printf("Lista completa de melodii:");printf("\n");
 for(int i=0;i<nr_m;i++){
   char m[100],arti[100];
      if(read(*a,m,sizeof(m))<=0){
        printf("client -> eroare la primire nume melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
     if(read(*a,arti,sizeof(arti))<=0){
        printf("client -> eroare la primire artist melodie %d lista generala \n",i+1);
       printf("%s \n",(char*) errno);
      }
      printf("%d.%s-%s",i+1,arti,m);printf("\n");
     
    } 
    cod=11;
    if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod stergere melodie\n");
      printf("%s\n",(char*)errno);
    }
    printf("Numarul  melodiei pe care doriti sa o stergeti: ");
    int nr;
    scanf("%d",&nr);
    if(write(*a,&nr,sizeof(nr))<=0){
      printf("client -> eroare la trimitere  id melodie de sters\n");
      printf("%s\n",(char*)errno);
    }
    //astept raspuns 1-ok 0-!ok
    int raspuns ;
    if(read(*a,&raspuns,sizeof(raspuns))<=0){
      printf("client ->eroare la primire raspuns stergere melodie \n");
      printf("%s \n",(char *)errno);
    }
    else if(raspuns==1)printf("Melodia a fost stearsa cu succes. ");
            else printf("Melodia nu a fost stearsa. ");
    

    printf("\n\n");
}
//BANNARE MEMBRU
void bannare_membru(int *a ){
  //toti clientii  ordonati alfabetic 
  //dau inapoi id
  int cod=12;
      if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod lista clienti\n");
      printf("%s\n",(char*)errno);
    }
   int nr_m=0;
    if(read(*a,&nr_m,sizeof(int))<=0)
  {
    printf("client ->eroare la primire clienti de la server \n");
    printf("%s \n",(char *)errno);
  }
//primesc si afisez lista de melodii

printf("Lista a utilizatorilor ordonata alfabetic:");printf("\n");
 for(int i=0;i<nr_m;i++){
   char u[25];
    if(read(*a,u,sizeof(u))<=0){
        printf("client -> eroare la primire username pentru clientul nr %d  \n",i+1);
       printf("%s \n",(char*) errno);
      }
      else printf("%d.%s\n",i+1,u);
 }
 cod=13;
 if(write(*a,&cod,sizeof(cod))<=0){
      printf("client -> eroare la trimitere cod membru de bannat\n");
      printf("%s\n",(char*)errno);
    }
 printf("Numarul clientului pe care doriti sa-l bannati: ");
    int nr;
    scanf("%d",&nr);
    if(write(*a,&nr,sizeof(nr))<=0){
      printf("client -> eroare la trimitere  id membru de bannat \n");
      printf("%s\n",(char*)errno);
    }
    //astept raspuns 1-ok 0-!ok
    int raspuns ;
    if(read(*a,&raspuns,sizeof(raspuns))<=0){
      printf("client ->eroare la primire raspuns bannare membru \n");
      printf("%s \n",(char *)errno);
    }
    else if(raspuns==1)printf("Membrul a fost bannat cu succes. ");
            else printf("Membrul nu a fost bannat. ");
    

    printf("\n\n");


}


 void iesire()
 { 
   printf("sesiunea va fi inchisa \n");
   exit(1);
 }
int main(int argc, char* argv[])
{
	int sd; 
  sd=creare_Socket(argc, argv);
 
	
  struct sockaddr_in my_server;
  my_server.sin_family = AF_INET;
   my_server.sin_addr.s_addr = inet_addr(argv[1]);
   my_server.sin_port= htons(port);
  if (connect (sd, (struct sockaddr *) &my_server,sizeof (struct sockaddr)) == -1)
    {
      perror ("eroare la connect().\n");
      return errno;
    }
  while(1){
   /*inregistrare si logare*/ 
    int opt;
    int a;
    int ok=0;
    
    printf("Introduceti optiunea dumneavoastra: \n 1.inregistrare \n 2.logare \n");
    printf("Optiunea dvs: \n");
    scanf("%d",&opt);
     switch(opt){
     case 1: { printf("Ati selectat Inregistrarea. \n");
                   preluare_si_trimitere_info_inregistrare(&sd);
                  break;
                }
    case 2:
     {
       ok=1;
      printf("Ati selectat Logarea. \n"); 
       preluare_si_trimitere_info_logare(&sd);
      int accept;
       recv(sd,&accept,sizeof(int),0);
       char  drepturi[6]="\0";/*drepturi[0]='\0';*/
       printf("%d \n",accept);
       if(accept==1){ 
                  recv(sd,drepturi,sizeof(char),0);
         if(*drepturi=='c'){
            printf("Login reusit! \n");
                while(1){
             printf("Selectati o comanda dintre urmatoarele : \n");
              printf("1.Adaugati o melodie. \n2.Top general melodii. \n3.Top melodii pentru un singur gen. \n");
              printf("4.Votati o melodie. \n5.Adaugati un comentariu la o melodie.\n6.Iesire\n");
              int o;
              printf("Comanda dvs: ");scanf("%d",&o);
              switch(o){
                case 1:{ preluare_trimitere_info_adaugare_melodie(&sd);
                  break;
                }
                case 2:{
                  cerere_top_general(&sd);
                  break;
                }
                /*mai intai afisez genuri apoi fac top daca genul e valid*/
               case 3:{ cerere_genuri(&sd);
                break;
              }
              case 4:{vot_melodie(&sd);
                break;
              }
              case 5:{
                com_melodie(&sd);
                break;
              }
              
              case 6:{ iesire();
                break;
              }
              default:printf("Optiune invalida! \n");
              }
          }
         }
          else if(*drepturi =='a'){
                  printf("Login reusit! \n");
                  while(10){
              printf("Selectati o comanda dintre urmatoarele : \n");
              printf("1.Adaugati o melodie. \n2.Top general melodii. \n3.Top melodii pentru un singur gen. \n");
              printf("4.Votati o melodie. \n5.Adaugati un comentariu la o melodie.\n");
              printf("6.Stergeti o melodie \n7.Bannare membru\n8.Iesire\n");
              int o;
              printf("Comanda dvs: ");scanf("%d",&o);
              switch(o){
                case 1:{ preluare_trimitere_info_adaugare_melodie(&sd);
                  break;
                }
                case 2:{
                  cerere_top_general(&sd);
                  break;
                }
                case 3:{ 
                  cerere_genuri(&sd);
                break;
              }
              case 4:{ vot_melodie(&sd);
                break;
              }
              case 5:{
                com_melodie(&sd);
                break;
              }
              case 6:{sterge_melodie(&sd);
                break;
              }
              case 7:{ bannare_membru(&sd);
                break;
              }
              
              case 8:{
                iesire();
                break;
              }
              default:printf("Optiune invalida! \n");
              }
          }
          }
       }
       else {
         printf("Login nereusit.Incercati din nou \n\n");
       }
       break;
     }
     default:printf("Optiune gresita \n Incercati din nou \n\n");
    }
  }
    
	return 0;
}   