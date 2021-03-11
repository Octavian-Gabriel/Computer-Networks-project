#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sqlite3.h>
#include <signal.h>
#include <pthread.h>  

#define PORT 1001
extern int errno ;
typedef struct info_Th{
	pthread_t idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
sqlite3 *d;//poiter baza de date
}info_Th;
static int idn=4;
static int idm=6;
int  creare_Socket()
{
  int sd=0;
if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
    
      exit(0);
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  int time=10;
   setsockopt(sd,IPPROTO_TCP,SO_REUSEADDR,&on,sizeof(on));
 return sd;
}

 sqlite3 * deschidere_BD()
{
sqlite3 *db;
char *zErrMsg = 0; 
   int open_db;
   /* Deschidere baza de date */
   open_db = sqlite3_open("TopMusic.db", &db);
   
   if( open_db ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(1);
   } else {
      fprintf(stdout, "Baza de date a fost deschisa.\n");
   }
   return db;
}

int Bind(int a)
{
struct sockaddr_in my_server;	// structura folosita de server
 
/* pregatirea structurilor de date */
  bzero (&my_server, sizeof (my_server));
 
  
  /* umplem structura folosita de server */
  
    my_server.sin_family = AF_INET;	
    my_server.sin_addr.s_addr = htonl (INADDR_ANY);
    my_server.sin_port = htons (PORT);
/* atasam socketul */
  if (bind (a, (struct sockaddr *) &my_server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      exit(0);
    }
return 1;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
} 
 
 // THREAD FUNCTIE INREGISTRARE
 void *inregistrare(void* a){
   printf("Thread inregistreaza client \n");
 struct inregistrare_client {
  char username[25];char parola[25] ;char nume[25];
 char  prenume[25]; char  tip[6] ;
 
 };
    struct argumente{ struct inregistrare_client c_arg;
                    sqlite3  *datab;   int n;  };

    struct argumente * agt=(struct argumente *) a;
    struct inregistrare_client C;
    C=agt->c_arg;
   int nr=agt->n;
  char *zErrMsg = 0;
sqlite3 *db;
db=agt->datab;
char id[10];
sprintf(id,"%d",idn);
char sql[300]; 
  sql[0]='\0';
 char ap[2]="\'\0";
  char ap1[6]="\', \'\0";char vot[3]="da";
  char ins_form[80]="INSERT INTO users(ID,NUME,PRENUME,USERNAME,PAROLA,DREPTURI,VOT) VALUES (";
  ins_form[strlen(ins_form)+1]='\0';
  strcat(sql,ins_form);strcat(sql,ap);strcat(sql,id);
  strcat(sql,ap1);strcat(sql,C.nume);strcat(sql,ap1);
  strcat(sql,C.prenume); strcat(sql,ap1);strcat(sql,C.username);strcat(sql,ap1);
  strcat(sql,C.parola);   strcat(sql,ap1);strcat(sql,C.tip);strcat(sql,ap1);strcat(sql,vot);
  strcat(sql,"\' ); ");
++idn;
int rc;
printf("%s \n",sql);
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   int ok=1;
   if( rc != SQLITE_OK ){//trimit client inregistrare fail
              ok=0;
          if(write(nr,&ok,sizeof(ok))<=0)
         {
           printf("server -> eroare la trimitere succesc inregistrare \n");
           printf("%s \n",(char*)errno);
         }
         
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } 
   else {//trimit client inregistrare cu succes
         if(write(nr,&ok,sizeof(ok))<=0)
         {
           printf("server -> eroare la trimitere succesc inregistrare \n");
           printf("%s \n",(char*)errno);
         }
         
   }

  pthread_detach(pthread_self());  
  return(NULL);
}

//THREAD FUNCTIE LOGARE
void *logare(void *a){
  printf("Thread executa logare client \n");
  //pregatire informatii
   struct logare_client{
    char  username[25];
  char  parola [25]; 
 };
     struct argumente_log {
              struct logare_client c_info_log;int fd_c,t;//t mod functie 0-normal,1-intoarce drept de vot;
              sqlite3 *datab1;};
    struct argumente_log * agl=(struct argumente_log *) a;
    struct logare_client c_log=agl->c_info_log;
    sqlite3 * db = agl->datab1;
    int fd=agl->fd_c;int tip=agl->t;
    
    int rc;
     char *err_msg = 0;
    sqlite3_stmt *res;
     char sql[200];sql[0]='\0';char ap[2]="\'\0";
     //split 0-normal,1-drept de vot
     if(tip==0){

    char select[50]="select * from users where username ='";select[strlen(select)+1]='\0';
    strcat(sql,select);strcat(sql,c_log.username);strcat(sql,ap);
    strcat(sql, " and parola ='\0");strcat(sql,c_log.parola);strcat(sql,ap);
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) ;
    else {
       fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    int step = sqlite3_step(res);int accept=1;
    if (step == SQLITE_ROW) {
      printf("Thread a logat client \n");
        //spun clientului ca se afla in baza de date
      if(send(fd,&accept,sizeof(int),0)<=0)
        {
          printf("server -> eroare la trimitere mesaj de accept login \n");
          printf("%s \n",(char *)errno);
        }
        char drepturi[6]="\0";/*drepturi[0]='\0';*/
        strcpy(drepturi, sqlite3_column_text(res, 5));
        send(fd,&drepturi,sizeof(char),0);
        } 
    else { accept=0;
      printf("Thread nu a logat client. \n");
     //sintpun clientului ca nu se afla in baza de date 
   
       if(send(fd,&accept,sizeof(int),0)<=0)
        {
          printf("server -> eroare la trimitere mesaj de accept login \n");
          printf("%s \n",(char *)errno);
        }
    }
     }
     else if(tip==1){
        char select[50]="select vot from users where username ='";select[strlen(select)+1]='\0';
    strcat(sql,select);strcat(sql,c_log.username);strcat(sql,ap);
    strcat(sql, " and parola ='\0");strcat(sql,c_log.parola);strcat(sql,ap);
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) ;
    else {
       fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    int step = sqlite3_step(res);
    if (step == SQLITE_ROW) {
      printf("Thread a obtinut drept de vot pentru user \n");
        //scot dreptul de vot si trimit int 1-da 0-nu
        char vot[3]="\0"; int v;
         printf("%s \n", sqlite3_column_text(res, 0));
        strcpy(vot, sqlite3_column_text(res, 0));
        if(strcmp(vot,"da")==0){
          printf("User are drept de vot \n");
          v=1;
          if(send(fd,&v,sizeof(v),0)<0){
            printf("server -> eroare la trimitere vot=da \n");
            printf("%s \n",(char*) errno);
          }
        }
        else {
          printf("User nu  are drept de vot \n");
          v=0;
          if(send(fd,&v,sizeof(v),0)<0){
            printf("server -> eroare la trimitere vot=nu \n");
            printf("%s \n",(char*) errno);
          }
        }
        } 
     }
    sqlite3_finalize(res);
    pthread_detach(pthread_self());
    return(NULL);
}

//THREAD FUNCTIE ADAUGARE MELODIE
void *adaugare_melodie(void * a){
  //pregatire informatii de adaugat si alte structuri de date necesare
  struct melodie{char artist[25],nume[25],gen[15],link[150],descriere[200];};
  struct arg_mel{ struct melodie arg_m;sqlite3 *datab2;};
  struct arg_mel * melodie_noua=(struct arg_mel *) a;
  struct melodie m= melodie_noua->arg_m;
  sqlite3 *db =melodie_noua->datab2;
   char id[10];sprintf(id,"%d",idm);char sql[200]; 
  sql[0]='\0';char ap[2]="\'";ap[2]='\0';
  char ap1[6]="\', \'";ap1[5]='\0';char * zErrMsg;
  char ins_form[80]="INSERT INTO songs (id,nume,artist,gen,link) values('";
  ins_form[strlen(ins_form)+1]='\0';
  char zero[2]="\0";int nul =0; sprintf(zero,"%d",nul);
  //pregatire query insert
  strcat(sql,ins_form);strcat(sql,id);strcat(sql,ap1);strcat(sql,m.nume);
  strcat(sql,ap1);strcat(sql,m.artist);strcat(sql,ap1);
  strcat(sql,m.gen);strcat(sql,ap1);strcat(sql,m.link);strcat(sql,ap1);strcat(sql,zero);
  strcat(sql,"\' ); ");
  printf("queri final %s \n",sql);
  ++idm;
int rc;
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Thread a adaugat melodie \n");
   }
 pthread_detach(pthread_self());
 return(NULL);
}

void distribuire_threaduri(void *);
static void * Thread(void * arg)
{		
		struct info_Th th_nou; 
		th_nou= *((struct info_Th*)arg);	
		printf ("[thread]- %d - Asteptam comanda...\n",(int) th_nou.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		distribuire_threaduri((struct info_Th*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
}

 //FUNCTIE TOP GENERAL
 void * top_general(void * d){//tip : 0-normal 1-fara sortare
  //interogare
   struct top_m{ sqlite3 * db;int a,t;}; struct top_m *top_Arg=(struct top_m *)d;
  sqlite3 *db =top_Arg->db;int client=top_Arg->a; int tip=top_Arg->t;
	sqlite3_stmt *stmt;
  if(tip==0){
    int d_top0=0;
     sqlite3_prepare_v2(db, "select nume from songs order by voturi desc", -1, &stmt, NULL);
   
    // generare si trimitere nr melodii
    while (sqlite3_step(stmt) != SQLITE_DONE) d_top0++;
 if(write(client,&d_top0,sizeof(int))<=0){
      printf("server->eroare la trimitere nr melodii top general spre client \n");
      printf("%s \n",(char*)errno);
    }
      //trimitere melodii secvential
         while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
    char m[100];
    int c=sqlite3_column_count(stmt);
		for (i = 0; i < c; i++){
		strcpy(m,sqlite3_column_text(stmt, i));
    
    if(write(client,m,sizeof(m))<=0){
      printf("server->eroare la trimitere melodie din top general spre client \n");
      printf("%s \n",(char*)errno);
    }} }
  }
  // lista fara sortare 
  else if(tip==1){
    int d_top1=0;
    printf("Trimitere melodii\n");
sqlite3_prepare_v2(db, "select nume,artist from songs;", -1, &stmt, NULL);
   
    // generare si trimitere nr melodii
    while (sqlite3_step(stmt) != SQLITE_DONE) d_top1++;
    
 if(write(client,&d_top1,sizeof(int))<=0){
      printf("server->eroare la trimitere nr melodii top general spre client \n");
      printf("%s \n",(char*)errno);
    }
      //trimitere melodii secvential
         while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
    char m[100],ar[100];
    int c=sqlite3_column_count(stmt);
		for (i = 0; i < c; i++){
     if(i==0)strcpy(m,sqlite3_column_text(stmt, i));
     else strcpy(ar,sqlite3_column_text(stmt, i));
     }
    if(write(client,m,sizeof(m))<=0){
      printf("server->eroare la trimitere melodie din lista general spre client \n");
      printf("%s \n",(char*)errno);
    }
    
    if(send(client,ar,sizeof(ar),0)<=0){
      printf("server->eroare la trimitere artist  din lista generala spre client \n");
      printf("%s \n",(char*)errno);
    }
    bzero(ar,100);bzero(m,100);
    }
     }
  printf("Thread a executat top general .Elibereaza resursele primite \n");
    sqlite3_finalize(stmt);
      pthread_detach(pthread_self());
 return(NULL);
 }
 

//TOP PENTRU GEN MUZICAL
void * top_gen(void * d ){
  //interogare
   struct top_m{ sqlite3 * db;int a;char gen[15];}; struct top_m *top_Arg=(struct top_m *)d;
  sqlite3 *db =top_Arg->db;int client=top_Arg->a; char g[15];strcpy(g,top_Arg->gen);
	sqlite3_stmt *stmt;
      char sql[200];sql[0]='\0';
    char select[40]="select nume from songs where gen like '";select[strlen(select)+1]='\0';
    char order[30]="%' order by voturi desc";order[strlen(order)+1]='\0';
	int d_top=0;
    strcat(sql,select);strcat(sql,g);strcat(sql,order);
    sqlite3_prepare_v2(db,sql, -1, &stmt, NULL);
    // generare si trimitere nr melodii
    while (sqlite3_step(stmt) != SQLITE_DONE) d_top++;
    //nu exista acest gen
    if(d_top==0){
        char mesaj[3]="nu";
        if(write(client,mesaj,sizeof(mesaj))<=0){
          printf("server->eroare la trimitere mesaj negativ top pentru gen");
          printf("%s\n",(char *)errno);
        }
    }

   else if(write(client,&d_top,sizeof(int))<=0){
      printf("server->eroare la trimitere nr melodii top general spre client \n");
      printf("%s \n",(char*)errno);
    }
      //trimitere melodii secvential
         while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
    char m[100];
    int c=sqlite3_column_count(stmt);
		for (i = 0; i < c; i++)
		strcpy(m,sqlite3_column_text(stmt, i));
   
    if(send(client,m,sizeof(m),0)<=0){
      printf("server->eroare la trimitere melodie din top general spre client \n");
      printf("%s \n",(char*)errno);
    } }
    sqlite3_finalize(stmt);
      pthread_detach(pthread_self());
 return(NULL);
}

//GENURI
void * cerere_genuri(void *a){
  struct top{ sqlite3 * db;int a;char gen[15];};
  struct top * gen=(struct top *)a;
  sqlite3 *db =gen->db;int client=gen->a;
	sqlite3_stmt *stmt;
     sqlite3_prepare_v2(db, "select distinct gen from songs ", -1, &stmt, NULL);
   int d_top=0;
    // generare si trimitere nr melodii
    while (sqlite3_step(stmt) != SQLITE_DONE) d_top++;
 if(write(client,&d_top,sizeof(int))<=0){
      printf("server->eroare la trimitere nr melodii top general spre client \n");
      printf("%s \n",(char*)errno);
    }
    else 
    printf("Thread  trimite genuri\n");
      //trimitere melodii secvential
         while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
    char m[100];
    int c=sqlite3_column_count(stmt);
		for (i = 0; i < c; i++){
		strcpy(m,sqlite3_column_text(stmt, i));
    if(send(client,m,sizeof(m),0)<=0){
      printf("server->eroare la trimitere melodie din top general spre client \n");
      printf("%s \n",(char*)errno);
                                    }
              } 
                  }
    printf("Thread a trimis genuri \n");
    sqlite3_finalize(stmt);
      pthread_detach(pthread_self());
 return(NULL);
}

//VOT MELODIE
void *vot_melodie(void * a ){
  struct vot_melodie{sqlite3 *dab;int i,sok;};
  struct vot_melodie *vot=(struct vot_melodie *)a;
  sqlite3 * db=vot->dab;int id=vot->i;
  //extrag nr de voturi cu column_int incrementez si fac update
  	sqlite3_stmt *stmt;printf("Thread adauga vot \n");
      char sql[200];sql[0]='\0';char id_c[5]="\0";sprintf(id_c,"%d",id);char ap[3]="\';\0";
      char select[40]="select voturi from songs where id='";select[strlen(select)+1]='\0';
      strcat(sql,select);strcat(sql,id_c);strcat(sql,ap);
      sqlite3_prepare_v2(db,sql, -1, &stmt, NULL); int nr_vot,nr_c;
      //obtinere nr de voturi curent
      while (sqlite3_step(stmt) != SQLITE_DONE)nr_vot=sqlite3_column_int(stmt,0);
     sqlite3_finalize(stmt);
      // incrementare nr voturi
   nr_vot++; char v_plus[3]="\0";sprintf(v_plus,"%d",nr_vot);strcpy(ap,"\'");
      strcpy(sql,"\0");char update[30]="update songs set voturi='";char c[15]=" where id='";
    strcat(sql,update);strcat(sql,v_plus);strcat(sql,ap);strcat(sql,c);strcat(sql,id_c);
    strcat(sql,ap);
  int cl=vot->sok; sqlite3_stmt *expr;
  //update bd
    sqlite3_prepare_v2(db, sql, -1, &expr, NULL);
	if( (sqlite3_step(stmt) == SQLITE_DONE)){
    int r=1;
   if(write(cl,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns afirmativ votare spre client \n");
     printf("%s \n",(char*)errno);
   
   }  
   else printf("Thread a adaugat vot.A eliberat resursele\n.");
  }
  else {
     int r=0;
      if(write(cl,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns negativ votare spre client \n");
     printf("%s \n",(char*)errno);
      printf("Thread nu a adaugat vot.A eliberat resursele\n.");
   }
   else printf("Thread a adaugat vot.A eliberat resursele\n."); 
  }
      sqlite3_finalize(expr);
pthread_detach(pthread_self());
 return(NULL);
}
// ADAUGA COMENTARIU MELODIE
void * com_melodie(void * a){
 struct com_ml{sqlite3 *d;int i,s;char co[100];};
 struct com_ml* m =(struct com_ml* )a;
 sqlite3 *db=m->d;int id=m->i,sk=m->s;char com[100]="\0";strcpy(com,m->co);com[strlen(com)+1]='\0';printf("com primit%s\n",com);
 //comentarii inainte 
 	sqlite3_stmt *stmt;printf("Thread adauga comentariu\n");
      char sql[400]="\0", ap[3]="\'\0", id_c[5]="\0", com_actual[200]="\0";sprintf(id_c,"%d",id);
      char select[40]="select comentarii from songs where id='";select[strlen(select)+1]='\0';
      strcat(sql,select);strcat(sql,id_c);strcat(sql,ap);
      sqlite3_prepare_v2(db,sql, -1, &stmt, NULL);
      //obtinere comentarii curente
      while (sqlite3_step(stmt) != SQLITE_DONE)strcpy(com_actual,sqlite3_column_text(stmt,0));
     sqlite3_finalize(stmt);
     com_actual[strlen(com_actual)+1]='\0';
     printf("com: %s\n",com_actual);
     //pana aici e bine
     //acum construiesc update pt com 
 strcpy(sql,"\0");char update[30]="update songs set comentarii='";char c[15]=" where id='";
 strcat(sql,update);strcat(sql,com_actual);strcat(sql," \0");strcat(sql,com);strcat(sql,ap);
 strcat(sql,c);strcat(sql,id_c);strcat(sql,"\'");printf("%s\n",sql);
 sqlite3_stmt *expr;
 sqlite3_prepare_v2(db,sql,-1,&expr,NULL);
 if(sqlite3_step(expr)==SQLITE_DONE){
    int r=1;
   if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns afirmativ com adaugat spre client \n");
     printf("%s \n",(char*)errno);
   
   }  
   else printf("Thread a adaugat comentariu.A eliberat resursele\n.");
  }
  else {
     int r=0;
      if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns negativ com adaugat spre client \n");
     printf("%s \n",(char*)errno);
      printf("Thread nu a adaugat comentariu.A eliberat resursele\n.");
   }
   else printf("Thread a adaugat comentariu.A eliberat resursele\n."); 
  }
      sqlite3_finalize(expr);
pthread_detach(pthread_self());
 return(NULL);
}

//STERGERE MELODIE
void * stergere_melodie(void *a){
  struct com_ml{sqlite3 *d;int i,s;};
  struct com_ml *m=(struct com_ml *)a;
  sqlite3 *db =m->d;int id=m->i,sk=m->s;
  sqlite3_stmt *expr;printf("Thread sterge melodie\n");
      char sql[100]="\0", ap[3]="\'\0", id_c[5]="\0";sprintf(id_c,"%d",id);
      char select[40]="delete from songs where id='";select[strlen(select)+1]='\0';
      strcat(sql,select);strcat(sql,id_c);strcat(sql,ap);
      sqlite3_prepare_v2(db,sql, -1, &expr, NULL);
      if(sqlite3_step(expr)==SQLITE_DONE){
        int r=1;
   if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns afirmativ stergere melodie client \n");
     printf("%s \n",(char*)errno);
   
   }  
   else printf("Thread a sters melodie.A eliberat resursele\n.");
  }
  else {
     int r=0;
      if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns negativ stergere melodie client \n");
     printf("%s \n",(char*)errno);
      printf("Threada nu sters melodie.A eliberat resursele\n.");
   }
   else printf("Thread a sters melodie.A eliberat resursele\n."); 
  }
      sqlite3_finalize(expr);
pthread_detach(pthread_self());
 return(NULL);
      }

//LISTA CLIENTI ALFABETIC
void * lista_clienti(void *a){
        struct l_cli{sqlite3 * d;int s;};
        struct l_cli *l=(struct l_cli*)a;
        sqlite3* db=l->d;int client=l->s;
        sqlite3_stmt *expr;
  
    int d_top0=0;
     sqlite3_prepare_v2(db, "select username from users order by 1", -1, &expr, NULL);
   printf("Thread trimit username-uri \n");
    // generare si trimitere nr utilizatori
    while (sqlite3_step(expr) != SQLITE_DONE) d_top0++;
 if(write(client,&d_top0,sizeof(int))<=0){
      printf("server->eroare la trimitere nr clienti spre client \n");
      printf("%s \n",(char*)errno);
    }
      //trimitere usernameuri secvential
         while (sqlite3_step(expr) != SQLITE_DONE) {
		int i;
    char u[25];
    int c=sqlite3_column_count(expr);
		for (i = 0; i < c; i++){
		strcpy(u,sqlite3_column_text(expr, i));
    printf("user %s",u);
    if(write(client,u,sizeof(u))<=0){
      printf("server->eroare la trimitere username spre client \n");
      printf("%s \n",(char*)errno);
    }} }
    printf("Thread a trimis toate username-urile \n");
  sqlite3_finalize(expr);
pthread_detach(pthread_self());
 return(NULL);
  }
//BANNARE USER
  void * bannare_user(void * a){
    struct usr_ban{sqlite3 * d;int i,s;};
    struct usr_ban * ub;ub=(struct usr_ban *)a;
    sqlite3 * db=ub->d;int id=ub->i,sk=ub->s;
    char id_c[3]="\0";sprintf(id_c,"%d",id);
    char sql[100]="update users set vot='nu' where id='";
    strcat(sql,id_c);strcat(sql,"\'\0");
     sqlite3_stmt *expr;
 sqlite3_prepare_v2(db,sql,-1,&expr,NULL);
 if(sqlite3_step(expr)==SQLITE_DONE){
    int r=1;
   if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns afirmativ bannare membru spre client \n");
     printf("%s \n",(char*)errno);
   
   }  
   else printf("Thread a bannat membru .A eliberat resursele\n.");
  }
  else {
     int r=0;
      if(write(sk,&r,sizeof(r))<=0){
     printf("server->eroare la trimitere raspuns negativ bannare membru spre client \n");
     printf("%s \n",(char*)errno);
      printf("Thread nu a bannat membru .A eliberat resursele\n.");
   }
   else printf("Thread a bannat membru .A eliberat resursele\n."); 
  }
      sqlite3_finalize(expr);
pthread_detach(pthread_self());
 return(NULL);
  }
      


// DISTRIBUIREA COMENZILOR PE THREADURI
void distribuire_threaduri(void *arg){
// primeste mai intai un cod 
//dupa valoarea codului functia va crea un thread 
pthread_attr_t atribute;
  pthread_attr_init(&atribute); 
        int nr, i=0;
	struct info_Th *t; 
	t= (struct info_Th*)arg;int cod_primit;
	 int client;pthread_t idthread;
   idthread=t->idThread;
   client=t->cl; sqlite3 *db=t->d;
  while(recv(client,&cod_primit,sizeof(cod_primit),0)>0){//cod de la client
  printf(" serverul a primit codul:%d \n",cod_primit);
   switch(cod_primit){
//cazuri de utilizare
  case 1: 
  { printf("Threadul  pregateste inregistrare client \n"); 
  struct inregistrare_client {
  char username[25];char parola[25] ;char nume[25];
 char  prenume[25]; char  tip[6] ;
 
 };struct inregistrare_client  c;
  if(recv(client,&c,sizeof( struct inregistrare_client ),0)<=0)
  {
    printf(" server-eroare la citit info inregistrare \n");
    printf("%s",(char *) errno);
  }
  struct argumente{ struct inregistrare_client c_arg;
                    sqlite3  *datab; int n;    };
      struct argumente ag;
      ag.c_arg=c;
      ag.datab=db;ag.n=client;
      pthread_create(&idthread,&atribute,inregistrare,&ag);
      ++i;
      break;
  }
  case 2://parametru logare =0-normal
    {printf("Threadul pregateste logare client \n");
        struct logare_client{
          char username[25];char parola[25];};
          struct logare_client c_log;
          if((recv(client,&c_log,sizeof(struct logare_client),0))<=0){
              printf("server-eroare la primire info logare \n ");
               printf("%s",(char *) errno);
          }
                    struct argumente_log {
              struct logare_client c_info_log;
              int fd_c,t;
              sqlite3 *datab1;};
              struct argumente_log ag_log;
              ag_log.c_info_log = c_log;
              ag_log.datab1=db;
              ag_log.fd_c=client;ag_log.t=0;
              pthread_create(&idthread,&atribute,logare,&ag_log);
              ++i;
         break;
        }
   case 3:{printf("Threadul  pregateste adaugare melodie \n ");
          struct melodie{char artist[25],nume[25],gen[15],link[150],descriere[200];};
          struct arg_mel{ struct melodie arg_m;sqlite3 *datab2;};
          struct melodie m;
          if(recv(client,&m,sizeof(struct melodie),0)<=0){
            printf("server-> eroare la primire info melodie\n");
            printf("%s",(char*)errno);
          }
          struct arg_mel melodie_arg;
          melodie_arg.arg_m=m;
          melodie_arg.datab2=db;
          pthread_create(&idthread,&atribute,adaugare_melodie,&melodie_arg);
          ++i;
            break;
   }
   case 4:{ //parametru 0-normal
     printf("Threadul  pregateste top general\n");
     struct top{ sqlite3 * db1;int a,t;}; struct top top_Arg;
     top_Arg.db1=db;top_Arg.a=client;top_Arg.t=0;
     pthread_create(&idthread,&atribute,top_general,&top_Arg);

     break;
   }
   case 5:{
     printf("Threadul pregateste top pentru gen\n");
     char gen_T[15]="\0";
     if(read(client,gen_T,sizeof(gen_T))<=0){
       printf("server ->eroare la primire gen muzical \n");
       printf("%s \n",(char*)errno);
     }
     printf("Server gen primit %s \n",gen_T);
      struct top{ sqlite3 * db2;int a;char gen[15];}; struct top top_Arg;
     top_Arg.db2=db;top_Arg.a=client;strcpy(top_Arg.gen,gen_T);
     pthread_create(&idthread,&atribute,top_gen,&top_Arg);

     break;
   }
   case 6:{
     printf("Serverul pregateste multimea de  genuri muzicale \n");
     struct top{ sqlite3 * db3;int a;}; struct top top_Arg;
     top_Arg.db3=db;top_Arg.a=client;
     pthread_create(&idthread,&atribute,cerere_genuri,&top_Arg);
     break;
   }
   case 7:{//primesc username si parola client intorc 1/0 are sau nu drept de vot
   // mai pun un paramentru la functia de logare 0-normal 1-drept de vot
     printf("Serverul pregateste drept vot user\n");
      struct logare_client{
          char username[25];char parola[25];};
          struct logare_client c_log;
          if((recv(client,&c_log,sizeof(struct logare_client),0))<=0){
              printf("server-eroare la primire info logare \n ");
               printf("%s",(char *) errno);
          }
                    struct argumente_log {
              struct logare_client c_info_log;
              int fd_c,t;
              sqlite3 *datab1;};
              struct argumente_log ag_log;
              ag_log.c_info_log = c_log;
              ag_log.datab1=db;
              ag_log.fd_c=client;ag_log.t=1;
              pthread_create(&idthread,&atribute,logare,&ag_log);
              ++i;
         
     break;
   }
   case 8:{
     printf("Threadul  pregateste lista melodii\n");
     struct top{ sqlite3 * db4;int a,t;}; struct top top_Arg;
     top_Arg.db4=db;top_Arg.a=client;top_Arg.t=1;
     pthread_create(&idthread,&atribute,top_general,&top_Arg);
     break;
   }

    case 9: {printf("Threadul pregateste votare melodie\n");
     //fac o functie de update vot pentru o melodie 
     struct vot_melodie{sqlite3 *dab;int i,s;};
     struct vot_melodie vot;vot.dab=db; vot.s=client;
     int id_mel;
     if(read(client,&id_mel,sizeof(id_mel))<=0){
       printf("server->eroare la primire id melodie pentru vot\n");
       printf("%s \n",(char *)errno);
     }
     printf("Am primit id %d \n",id_mel);
     vot.i=id_mel;
     pthread_create(&idthread,&atribute,vot_melodie,&vot);
     break;
    }
    case 10: {
      printf("Threadul pregateste adaugare comentariu melodie \n");
      int id;
      if(read(client,&id,sizeof(id))<=0){
        printf("server->eroare la primire id melodie de comentat\n");
        printf("%s \n",(char *)errno);
        
      }
      struct com_ml{sqlite3 *d;int i,s;char com[100];};
      struct com_ml m;
      m.d=db;m.i=id;m.s=client;
      if(read(client,&m.com,sizeof(m.com))<=0){
        printf("server -> eroare la primire comentariu \n");
        printf("%s \n",(char *) errno);
      }
      pthread_create(&idthread,&atribute,com_melodie,&m);
    break;
    }
    case 11:{
        printf("Threadul pregateste stergere melodie\n");
        int id;
      if(read(client,&id,sizeof(id))<=0){
        printf("server->eroare la primire id melodie de sters\n");
        printf("%s \n",(char *)errno);
        
      }
      struct com_ml{sqlite3 *d;int i,s;};
      struct com_ml m;
      m.d=db;m.i=id;m.s=client;
      pthread_create(&idthread,&atribute,stergere_melodie,&m);
  break;

    }
    case 12:{
      printf("Threadul pregateste lista clienti\n");
      struct l_cli{sqlite3 * d;int s;};
      struct l_cli l;l.d=db;l.s=client;
      pthread_create(&idthread,&atribute,lista_clienti,&l);
      break;
    }
    case 13: {
     printf("Thread pregateste bannare user\n");
     struct usr_ban{sqlite3 * d;int i,s;};
     struct usr_ban u; u.d=db; u.s=client;
     int id;
     if(read(client,&id,sizeof(id))<=0){
        printf("server->eroare la primire id user de bannat\n");
        printf("%s \n",(char *)errno);
      }
      u.i=id;
      pthread_create(&idthread,&atribute,bannare_user,&u);
      break;
    }
    
   } 
   }	
//case ? exit(1)S
}

int main()
{  
  
    int sd;	
   int i=1;

pthread_t th[100]; 

   sd = creare_Socket();
   Bind(sd);
     struct sockaddr_in client;
   	  bzero (&client, sizeof (client));
  	
  
    if (listen (sd,5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    } 
    else printf("Asteptam clienti \n");
  
   
       while(1){
         info_Th * td;int utilizator;int length = sizeof (client); fflush (stdout);
       if ( (utilizator = accept (sd, (struct sockaddr *) &client, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
   else printf("Client acceptat.  Totul este pregatit! \n");
  td=(struct info_Th*)malloc(sizeof(struct info_Th));	
	td->idThread=i++;
	td->cl=utilizator;
  td->d=deschidere_BD();
 
	
  pthread_create(&th[i], NULL, &Thread, td);	 
       }
}
 
  