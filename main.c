#include <stdio.h>
#include "math/math.h"
#include "log/log.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>



void *th1(void *p)
{
    time_t t;
    struct tm *tm;
    char str[200];

    while(1) {
        sleep(1);
        time(&t);
        tm = localtime(&t);
        if (tm) {
            memset(str,0,sizeof(str));
            asctime_r(tm,str);
            if (strlen(str)) {
                //printf("th1, cur time: %s",str);
                printf("th1, tmaddr[%08x], cur time: %s",(unsigned long)tm, str);
            }
        }
    }
}

void *th2(void *p)
{
    time_t t;
    struct tm *tm;
    char str[200];

    while(1) {
        
        time(&t);
        tm = localtime(&t);
        if (tm) {
            memset(str,0,sizeof(str));

            asctime_r(tm,str);
            if (strlen(str)) {
                //              printf("th1, tmaddr[%08x], cur time: %s",(unsigned long)tm, str);
                printf("th1,%d-%d-%d ", tm->tm_year,tm->tm_mon,tm->tm_mday);
//                sleep(1);
                printf("%d:%d:%d\n",tm->tm_hour,tm->tm_min,tm->tm_sec);

                printf("th2,%d-%d-%d ", tm->tm_year,tm->tm_mon,tm->tm_mday);
                sleep(1);
                printf("%d:%d:%d\n",tm->tm_hour,tm->tm_min,tm->tm_sec);
            }
        }


        if (tm) {
            memset(str,0,sizeof(str));

            asctime_r(tm,str);
            if (strlen(str)) {
                // printf("th2, tmaddr[%08x], cur time: %s",(unsigned long)tm, str);
            }
        }
    }
}

//int parse_filename(const char *filename, time_t *t);
int main(int argc, char **argv)
{
    int a,b,c;
    pthread_t tid1,tid2;
    //pthread_create(&tid1, NULL, th1,NULL);
    //pthread_create(&tid2, NULL, th2,NULL);
    /* { */
/*         int r; */
/*         time_t t; */
/*     r = parse_filename(argv[1],&t); */
/*     printf("r:%d\n",r); */
/* return 1; */

/*     } */

    log_init();

  

  while(1) {
sleep(5);
        sync();
        
        //printf("sync\n");
    }

    return 0;
}
