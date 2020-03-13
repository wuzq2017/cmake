#include <stdio.h>
#include "math/math.h"
#include "log/log.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    int a,b,c;

    log_init();

    a = 10;
    b =11;
    c = add(a,b);
    printf("sum is %d, diff is %d\n",c,dec(a,b));
    while(1) {
sleep(5);
        sync();
        
        printf("sync\n");
    }

    return 0;
}
