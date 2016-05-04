#include "Stdafx.h"
#include "ConsolePrinter.h"

ConsolePrinter* ConsolePrinter::_instance = 0;

ConsolePrinter::ConsolePrinter(bool in)
{
    // create a new console to the process
    AllocConsole();
	pFile = NULL;
	if (in == true)
	{
		hConsoleMode = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
		pFile  = _fdopen(hConsoleMode, "r");

		// use default stream buffer
		setvbuf(pFile, NULL, _IONBF, 0);
		*stdin = *pFile;
	}
	else
	{
		hConsoleMode = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		pFile  = _fdopen(hConsoleMode, "w");

		// use default stream buffer
		setvbuf(pFile, NULL, _IONBF, 0);
		*stdout = *pFile;
	}
}

ConsolePrinter::~ConsolePrinter()
{
    FreeConsole();
	if (pFile != NULL)
	{
		fclose(pFile);
	}
}

ConsolePrinter* ConsolePrinter::Instance(bool in)
{
    if (_instance == 0)
    {
        _instance = new ConsolePrinter(in);
    }
    return _instance;
}

void ConsolePrinter::Destroy()
{
    if (_instance)
    {
        delete _instance;
    }
    _instance = 0;
}