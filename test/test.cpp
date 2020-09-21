#ifdef NEED_IOSTREAM
#include <iostrean>
#endif
#include "test.h"

void adjustReturn(int &stat);
int main(int argc, char *argv[])
{
    int returnStat;
    adjustReturn(returnStat);
    outEmitter e;
    e.emit_out();
    FILE *f = fopen("test.txt","w");
    if (f)
    {
        fwrite((void *)main,128,1,f);
        fclose(f);
    } else {
        printf("Could not open file!\n");
    }
    return returnStat;
}

void adjustReturn(int &stat)
{
    stat = 0;
}

void outEmitter::emit_out()
{
	std::cout << "Hello World!" << std::endl;
}

outEmitter::outEmitter() {}

// Unused function
bool isGtZero(int v)
{
    return (v > 0);
}

