#include <iostream>

void adjustReturn(int &stat);
int main(int argc, char *argv[])
{
    int returnStat;
    adjustReturn(returnStat);
    std::cout << "Hello World" << std::endl;
    return returnStat;
}

void adjustReturn(int &stat)
{
    stat = 0;
}

// Unused function
bool isGtZero(int v)
{
    return (v > 0);
}
