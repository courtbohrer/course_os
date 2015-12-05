#include <thread.h>
#include <assert.h>
#include <stdio.h>

void* f(void* arg)
{
   return NULL;
}

void main(void)
{
   int rc = thread_create(NULL, f, (void*) 1);

   // if ( rc == 0 ) printf("rc is indeed zero\n");

   // for (int i = -20; i <=20; i++)
   // {
   // 		if (rc != i) continue;
   // 		switch (i) 
   // 		{
   // 		case -20: printf("rc=-20\n"); break; case -19: printf("rc=-19\n"); break;
   // 		case -18: printf("rc=-18\n"); break; case -17: printf("rc=-17\n"); break;
   // 		case -16: printf("rc=-16\n"); break; case -15: printf("rc=-15\n"); break;
   // 		case -14: printf("rc=-14\n"); break; case -13: printf("rc=-13\n"); break;
   // 		case -12: printf("rc=-12\n"); break; case -11: printf("rc=-11\n"); break;
   // 		case -10: printf("rc=-10\n"); break; case -9:  printf("rc=-9\n");  break;
   // 		case -8:  printf("rc=-8\n");  break; case -7:  printf("rc=-7\n");  break;
   // 		case -6:  printf("rc=-6\n");  break; case -5:  printf("rc=-5\n");  break;
   // 		case -4:  printf("rc=-4\n");  break; case -3:  printf("rc=-3\n");  break;
   // 		case -2:  printf("rc=-2\n");  break; 
   // 		case -1:  printf("rc=-1\n");  break; case 0:   printf("rc=0\n");   break;
   // 		case 20:  printf("rc=20\n");  break; case 19:  printf("rc=19\n");  break;
   // 		case 18:  printf("rc=18\n");  break; case 17:  printf("rc=17\n");  break;
   // 		case 16:  printf("rc=16\n");  break; case 15:  printf("rc=15\n");  break;
   // 		case 14:  printf("rc=14\n");  break; case 13:  printf("rc=13\n");  break;
   // 		case 12:  printf("rc=12\n");  break; case 11:  printf("rc=11\n");  break;
   // 		case 10:  printf("rc=10\n");  break; case 9:   printf("rc=9\n");   break;
   // 		case 8:   printf("rc=8\n");   break; case 7:   printf("rc=7\n");   break;
   // 		case 6:   printf("rc=6\n");   break; case 5:   printf("rc=5\n");   break;
   // 		case 4:   printf("rc=4\n");   break; case 3:   printf("rc=3\n");   break;
   // 		case 2:   printf("rc=2\n");   break; case 1:   printf("rc=1\n");   break;
   // 		}

   // }

   assert(rc == ERR_INVALID_ARGUMENTS);

   printf("passed\n");
}
