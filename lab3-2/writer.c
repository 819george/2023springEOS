#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
*argc 应用程序参数个数
*argv 具体的参数内容,字符串形式
*命令调用: ./writer  <student_ID>    
*./writer 310512052 
*/
int main(int argc, char *argv[]) 
{
    int fd = 0;
    int ret = 0;
    char *data = malloc(10*sizeof(char));
	
    if (argc != 2){
        printf("error usage!! Plese use : ./writer  <student_ID> ");
        return -1;
    }

    /*open*/
    fd = open("/dev/etx_device", O_RDWR);
    if (fd < 0){
        printf("open /dev/etx_device failed!!\r\n");
        return -1;
    }

    /*write*/
    data = (argv[1]);    
    ret = write(fd, data, strlen(data)+1);
    if (ret < 0){
        printf("led control failed!\r\n");
        close(fd);
        return -1;
    }

    /*close*/
    ret = close(fd);
    if (ret < 0){
        printf("close /dev/etx_device failed!!");
        return -1;
    }

    return 0;
}
