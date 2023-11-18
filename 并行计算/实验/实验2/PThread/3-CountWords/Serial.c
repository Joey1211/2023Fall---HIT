#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

FILE *fd; 
int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;

int GetNextLine(FILE *f, char *Line)
{ 
	 
	 if (fgets(Line, 132, f)==NULL) if (feof(f))return EOF; else return 1;
}

int GetWordAndLetterCount(char *Line)
{
	int Word_Count = 0, Letter_Count = 0;
	for (int i=0;i<132;i++)
	{
		if ((Line[i]!=' ')&&(Line[i]!=0)&&(Line[i]!='\n')) Letter_Count++; 
		else {
			if (Letter_Count % 2) {
				TotalOddWords++; 
                Word_Count++;
				Letter_Count = 0;
			}
			else {
				TotalEvenWords++;
				Word_Count++;
				Letter_Count = 0;
			}
			if (Line[i]==0) break;
		}
	}
	return (Word_Count);     // encode two return values
}

int CountWords() 
{ 
   bool bDone = false;
   char inLine[132];

	while (!bDone)
	{
      bDone = (GetNextLine(fd, inLine) == EOF);
	  if (!bDone){
           TotalWords += GetWordAndLetterCount(inLine) ;
         }
	}
	return 0;
}

int main()
{
	fd = fopen("./InFile1.txt", "r"); // Open file for read

	struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    gettimeofday(&TimeStampStart, NULL);
	CountWords();
	gettimeofday(&TimeStampStop, NULL);
	ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

	fclose(fd);

    printf("Total Words = %8d\n", TotalWords);
    printf("Total Even Words = %7d\nTotal Odd Words = %7d\n", TotalEvenWords, TotalOddWords);
	printf("The time to read words was %f seconds\n", (ExeTime));

	return 0;
}
