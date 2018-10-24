#include <stdio.h>
#include <unistd.h>

int main (void)
{
    int x = 0;

    for(long i = 0; i < 1990000000; i++) {
	x += 1;
    }

    return 0;
}
