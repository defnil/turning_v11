#pragma once
#include <iostream>
#include <io.h>
#include <fcntl.h>

#define PR(x) std::cout << #x " = " << x << "\n"; // convenient for printing info
#define PRS(s) std::cout << s << "\n";

class ConsolePrinter
{
public:
    static void Destroy();
    static ConsolePrinter* Instance(bool in);
    virtual ~ConsolePrinter();

protected:
    ConsolePrinter(bool in);

private:
    static ConsolePrinter* _instance;
    FILE *pFile;
	int hConsoleMode;
};