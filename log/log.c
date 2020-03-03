#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static key_t key1;

static int msgid;

void log_init(void)
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
