#include <termios.h>
#include <unistd.h>
#include <stdio.h>

void main()
{
    char buf[100];
    int count;

    while(count = read(fileno(stdin), buf, 100))
    {
        printf("%s", buf);
    }
}

