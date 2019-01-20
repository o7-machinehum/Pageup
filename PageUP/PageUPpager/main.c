//Dependecies
#include <my_global.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wiringPi.h>
#include <math.h>
#include <mysql.h>

//Defines
#define DB0 0
#define DB1 1
#define DB2 2
#define DB3 3
#define RST 4 //"Upper nibble"
#define DR 5 //"Data ready"
#define DP 6 //"Data processed"
#define UTM 1000 //uS to mS

#define TABLE 0
#define TIME 1
#define DEBUG 2

//Funk Protos
void Print_Table(int);
void init(void);
int Read_Byte(void);
void UpdateServer(int table[][3], int pointer);

int main(void)
{
  int table[100][3], temp = 0, pointer = 0, i = 0;

  wiringPiSetup();
  init();

  delayMicroseconds(10*UTM); //Delay 10ms
  digitalWrite(RST,1); //On plz

  while(1)
  {

    if(digitalRead(DR))
    {
      table[pointer][TABLE] = Read_Byte();
      table[pointer][TIME] = millis();
      table[pointer][DEBUG] = 1;

      printf("%d", table[pointer][TABLE]); //Debug purposes
      printf("\n"); //Debug

      UpdateServer(table, pointer);
      pointer++;

      if(pointer == 100)
      {
        i = 0;
        while(i < 50)
        {
          table[i][TABLE] = table[i + 50][TABLE];
          table[i][TIME] = table[i + 50][TIME];
          table[i][DEBUG] = table[i + 50][DEBUG];
          i++;
        }
        pointer = 50;

      }
    }

  }

  return 0;
}

void finish_with_error(MYSQL *con)
{
	printf("%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);        
}

/*Move the table into the SQL database*/
void Print_Table(int Table)
{

  FILE *fp;
  fp = fopen("/var/www/index.html","a"); //a == append
  fprintf(fp,"%d", Table);
  fprintf(fp,"\n");
  fclose(fp);

}

void init(void)
{
  pinMode(DB0,INPUT);
  pinMode(DB1,INPUT);
  pinMode(DB2,INPUT);
  pinMode(DB3,INPUT);

  pinMode(DR,INPUT);

  pinMode(RST,OUTPUT);
  pinMode(DP,OUTPUT);

  digitalWrite(DP,LOW);
  digitalWrite(RST,0); //Lets poll a reset
}

int Read_Byte(void)
{
  int Data = 0;

  Data |= digitalRead(DB0); //Bit Zero
  Data |= (digitalRead(DB1) << 1); //Bit One
  Data |= (digitalRead(DB2) << 2); //Bit Two
  Data |= (digitalRead(DB3) << 3); //Bit Three

  digitalWrite(DP,HIGH); //Lower nibble processed

  while(digitalRead(DR)); //Wait for falling edge

  Data |= (digitalRead(DB0) << 4); //Bit four
  Data |= (digitalRead(DB1) << 5); //Bit five
  Data |= (digitalRead(DB2) << 6); //Bit Six
  Data |= (digitalRead(DB3) << 7); //Last bit

  digitalWrite(DP,LOW);

  return(Data);
}

void UpdateServer(int table[][3], int pointer)
{
  FILE *fp = NULL;
	char output[50] = {};
	
  fp = fopen("/var/www/index.html", "w");
	sprintf(output, "insert into Pages values('NULL',%d)", table[pointer][TABLE]);
	
  while(pointer >= 0)
  {
    fprintf(fp, "%d", table[pointer][TABLE]);
    fprintf(fp, ";");
    fprintf(fp, "%d", table[pointer][TIME]);
    fprintf(fp, ";");
    fprintf(fp, "%d", table[pointer][DEBUG]);
    fprintf(fp, "<br>");
    fprintf(fp,"\n");
    pointer--;
  }

  fclose(fp);

  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
		fprintf(stderr, "mysql_init() failed\n");
		exit(1);
  }  

	if (mysql_real_connect(con, "localhost", "Upageup", "pageup", 
          "pageup", 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  } 

	if (mysql_query(con, output)) {
      finish_with_error(con);
  }  

  mysql_close(con);

}

