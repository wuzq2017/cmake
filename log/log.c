#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define LOG_path "./log_dir"
static FILE *fp1 = NULL;
static pthread_mutex_t mutex_lock1 = PTHREAD_MUTEX_INITIALIZER;
static int time_set_ok = 1;
#define MAX_LOG_FILE_SIZE 10240
static int flag1;
static int num = 1;

static key_t key1;

static int msgid;

typedef struct
{
    long mtype;
    union {
        struct {
            unsigned int len; // include len num tv ...
            unsigned int num;
            unsigned int a;
            struct timeval tv;
            char dat[2000];
        }/*__attribute__((packed))*/ st;
        char mtext[4096];
    }mtext;
    /* unsigned int num; */
    /* //time_t tm; */
    /* struct timeval tv; */
    /* char dat[4096]; */
} /*__attribute__((packed)) */msg_t;
#define DAT_LEN (msg->mtext.st.len - ((unsigned long)(&msg->mtext.st.dat[0])-(unsigned long)(msg)))

typedef enum {
    MTYPE_invalid = 0,
    MTYPE_1,
    MTYPE_2,
    MTYPE_restart,
    MTYPE_test
}msg_type_t;

typedef struct {
    msg_type_t mtype;
    char *type_str;
    void (*p)(const void *param,int index, char *type_str);
}msg_proc_tbl_t;






static void write_file(char *str, unsigned int len)
{
    pthread_mutex_lock(&mutex_lock1);
            
    if (fp1 != NULL) {
        int ret;
        ret = fwrite(str,len, 10, fp1);
        printf("wrlen:%d, len:%d, strlen:%d, %s\n",ret,len,strlen(str),str);
        //printf("====================\n");
    }
                
    pthread_mutex_unlock(&mutex_lock1);
} 
static void log_save_test(const void *param, int index, char *type_str)
{

}

static int format_time(const struct timeval *tv, char *str, unsigned str_size)
{
    return -1;
}
static void log_save_restart(const void *param, int index, char *type_str)
{
#if 1
    const msg_t *msg = (const msg_t *)param;
    int r;
    struct timeval tv;
    char str[sizeof(msg_t) * 3 + 100];
    char str1[100];
    unsigned int datlen;
    if (type_str)
        sprintf(str, "num:%05d, type:%s, ",msg->mtext.st.num,type_str);
    else
        sprintf(str, "num:%05d, type:%s, ",msg->mtext.st.num,"NULL");

    
    r = format_time(&msg->mtext.st.tv,str1,sizeof(str1));

    strcat(str,"rcv_time:");
    if (r==0) {
        
        strcat(str,str1);
    }
    strcat(str,", ");

    strcat(str,"save_time:");
    gettimeofday(&tv,NULL);
    r = format_time(&tv,str1,sizeof(str1));
    if (r==0) {
        strcat(str,str1);
    }
    strcat(str,", ");

    datlen = DAT_LEN;
    sprintf(str1,"len:%d, ",datlen);
    strcat(str,str1);

    str[strlen(str)+datlen] = 0;
    memcpy(&str[strlen(str)],&msg->mtext.st.dat[0],datlen);
    //write_file(str, strlen(str));
#endif

}


msg_proc_tbl_t msg_proc_tbl[] = {

    {MTYPE_test, "MTYPE_test",log_save_test},
    {MTYPE_restart, "MTYPE_restart",log_save_restart},
    {MTYPE_invalid, "",NULL},
};



int init_file(void)
{
    int file_num = 0;
    DIR *dirp;
    time_t tm;
    char file[100];
    struct dirent *dp;
    struct tm *tm_now;
    time(&tm);
    tm_now = localtime(&tm);
    sprintf(file, "%s/%04d-%02d-%02d", LOG_path,tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday);
    
    if (access(LOG_path, 0) == -1)
    {
        if (mkdir(LOG_path, 0777))
        {
            perror("mkdir fail Error 1");
            return -1;
        }
    }
    
    if (access(file, 0) == -1)
    {
        if (mkdir(file, 0777))
        {
            perror("mkdir fail Error 2");
            return -1;
        }
    }
    
    dirp = opendir(file);
    
    if (NULL == dirp)
    {
        // if(mkdir("/sddisk/Log_txt",0777))
        {
            perror("mkdir fail");
            return -1;
        }
    }
    
    while ((dp = readdir(dirp))!=NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        else
            file_num = file_num + 1;
    }
    
    return file_num;
}


void *make_file_thread(void *param)
{
    long i = 0;
    time_t tm;
    struct tm *tm_now;
    int file_num = 0;
    char file[100];
    
    while (1)
    {
        pthread_mutex_lock(&mutex_lock1);
        
        if (time_set_ok)
        {
            if (fp1 != NULL)
            {
                {
                    msg_t msg;
                    msg.mtype = MTYPE_restart;
                    {
                        struct timeval tv;
                        struct timezone tz;
                        gettimeofday(&tv,&tz);
                        msg.mtext.st.tv = tv;
                    }
                    
                    msg.mtext.st.num  = num++;
                        
                    sprintf(msg.mtext.st.dat, "%s\n", "---------check file size-----");
                    msg.mtext.st.len = sizeof(msg.mtext.st)-1 + strlen(msg.mtext.st.dat)+1;
                    msgsnd(msgid, &msg, msg.mtext.st.len, 0);
                }

                i = ftell(fp1);
                
                //printf("*************************  %ld\n",i);
                if (i >= MAX_LOG_FILE_SIZE)
                {
                    fclose(fp1);
                    fp1 = NULL;
                    //sleep(5);
                    flag1 = 0;
                    i = 0;
                }


            }
            
            if (fp1 == NULL)
            {
                int n = init_file();
                if (n < 0) exit(1);//==================
                file_num = n + 1;
                time(&tm);
                tm_now = localtime(&tm);
                sprintf(file, "%s/%04d-%02d-%02d/%05d_%04d-%02d-%02d_%02d_%02d_%02d.txt", 
                        LOG_path, tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                        file_num,tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, 
                        tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

/* sprintf(file, "%s/%04d-%02d-%02d/%05d_%04d-%02d-%02d_02d_%02d_%02d.txt",  */
/*                         LOG_path, tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,  */
/*                         file_num, tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec); */
                printf("name :%s\n", file);
                
                do{
                    fp1 = fopen(file, "a+");
                    
                    if (NULL == fp1)
                    {
                        printf("\n  fopen fail\n");
                        //exit(-1);
                        usleep(1000 * 10);
                    }
                    else
                    {
                        flag1 = 1;
                    }
                } while (NULL == fp1);
                {
                    static int restart_f = 1;
                    if (restart_f == 1)
                    {
                        msg_t msg;
                        time(&tm);
                        msg.mtype = MTYPE_restart;
                        {
                            struct timeval tv;
                            struct timezone tz;
                            gettimeofday(&tv,&tz);
                            msg.mtext.st.tv = tv;
                        }
                    
                        msg.mtext.st.num  = num++;
                        //memset(msg.dat, 0, sizeof(msg.dat));
                        
                        sprintf(msg.mtext.st.dat, "%s\n", "###########start success##########");
                        msg.mtext.st.len = sizeof(msg.mtext.st)-1 + strlen(msg.mtext.st.dat)+1;

                        msgsnd(msgid, &msg, msg.mtext.st.len, 0);
                        restart_f  = 0;
                    }
                }
            }
        }
        
        pthread_mutex_unlock(&mutex_lock1);
        sleep(1);
    }
}

static void *log_save_proc(void *param)
{
    msg_t msg;

    while (1)
    {
        ssize_t ret;
        msg_t msg;
        ret = msgrcv(msgid, &msg, sizeof(msg) - sizeof(msg.mtype), 0, 0);
        
        if (-1 == ret)
        {
            perror("msgrcv fail\n");
            exit(-1);
        }
        
        if (ret < sizeof(msg.mtext.st)-1)
            continue;
        if (msg.mtype <=0)
            continue;
        printf("rcv ret[%d], len[%d]\n", ret,msg.mtext.st.len);

        msg.mtext.st.len = ret;

        {
            int i = 0;
            while(i < sizeof(msg_proc_tbl)/sizeof(msg_proc_tbl_t)) {
                if (msg_proc_tbl[i].mtype == msg.mtype) {
                    if (msg_proc_tbl[i].p) {
                        msg_proc_tbl[i].p((void *)&msg,i,msg_proc_tbl[i].type_str);
                    }
                    break;
                }
                i++;
            }
        }
    }
}

static void msg_q_create(void)
{
    struct msqid_ds tmpbuf;
    key1 = ftok("112358", 'b');
    msgid = msgget(key1, 0);
    
    if (msgid == -1)
    {
        msgid = msgget(key1, 0644 | IPC_CREAT);
        
        if (msgid == -1)
        {
            perror("msggest fail");
            exit(-1);
        }
    }
    else
    {
        int i = msgctl(msgid, IPC_RMID, &tmpbuf);
        printf("success msgctl     %d\n", i);
        msgid = msgget(key1, 0644 | IPC_CREAT);
        
        if (msgid == -1)
        {
            perror("msggest fail");
            exit(-1);
        }
    }
    
    printf("file[%s],function[%s] ok! msgid = %d\n", __FILE__,__FUNCTION__,msgid);
}

static void thread_init(void)
{
    pthread_t tid,tid1;
    pthread_create(&tid1,NULL,make_file_thread,NULL);
    pthread_create(&tid,NULL,log_save_proc,NULL);
}
void log_init(void)
{
    msg_t msg;

    printf("sizeof(msg_t) = %d,sizeof(msg.mtext.st)=%d,sizeof(long)=%d, sizeof(timeval)=%d,sizeof(int)=%d\n\n",sizeof(msg_t),sizeof(msg.mtext.st),sizeof(long),sizeof(struct timeval),sizeof(int));
    printf("add diff = %lu\n",(unsigned long)(&msg.mtext.st.dat[0])-(unsigned long)(&msg));

    msg_q_create();
    thread_init();

    
    
}
