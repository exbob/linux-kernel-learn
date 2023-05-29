#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd, ret;
    int i = 0;
    char *data;

    fd = open("/dev/chardev", O_RDWR);

    data = malloc(0x100);
    for(i=0; i<0x100; i++)
    {
        data[i] = i;
    }

    ret = write(fd, data, 10);
    printf("write %d bytes\n", ret);

    ret =read(fd, data, 8);
    printf("read %d bytes\n", ret);
    for(i=0; i<8; i++)
    {
        printf("0x%02x ", *(data+i));
        if((i+1)%8 == 0)
            printf("\n");
        
        if(*(data+i) != i)
        {
            printf("error: data[%d] should be 0x%02x!", i, i);
            break;
        }
    }

    free(data);
    
    close(fd);

    return 0;
}
