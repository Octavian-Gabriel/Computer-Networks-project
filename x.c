#include <stdio.h>
#include<stdlib.h>
#include <sqlite3.h>
#include<string.h>
int main(void)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	
	sqlite3_open("TopMusic.db", &db);

	if (db == NULL)
	{
		printf("Failed to open DB\n");
		return 1;
	}

	printf("Performing query...\n");
	sqlite3_prepare_v2(db, "select link from songs where id='1'", -1, &stmt, NULL);
	char link[25];
	printf("Got results:\n");
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		int num_cols = sqlite3_column_count(stmt);
		
		for (i = 0; i < num_cols; i++)
		{
			
				strcpy(link, sqlite3_column_text(stmt, i));
				char s[110]="start ";    printf("%s \n",s);
   if(system(link)<0){
     printf("linkul nu este acceptat.\n");
     
   }
		}
		

	}
	printf("%s\n",link);

	sqlite3_finalize(stmt);

	sqlite3_close(db);

	getc(stdin);
	return 0;
}