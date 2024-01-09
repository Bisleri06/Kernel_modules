#include<iostream>
#include<unistd.h>
#include<fcntl.h>
using namespace std;

int main()
{
    int s=open("/dev/device",O_RDWR);
    char buf[1024]="ENCRYPTED";

    //encrypt
    write(s,buf,1024);
    read(s,buf,1024);
    printf("%s\n",buf);

    //decrypt
    write(s,buf,1024);
    read(s,buf,1024);
    printf("%s\n",buf);
    
    close(s);
}