#pragma once
#include <stdio.h>
#include <Windows.h>

bool FileExists(const char* fileName)
{
	FILE* fin = fopen(fileName, "rb");
	if (fin == NULL)
	{
		return false;
	}

	fclose(fin);
	return true;
}

bool ConsoleExists(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return false;

	return true;
}

void InitConsole()
{
	if (!ConsoleExists())
	{
		AllocConsole();

		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stderr);
	}
}

int __cdecl cusprintf(const char* Format, ...)
{
	va_list ArgList;
	int Result = 0;

	if (ConsoleExists())
	{
		__crt_va_start(ArgList, Format);
		Result = vprintf(Format, ArgList);
		__crt_va_end(ArgList);
	}

	return Result;
}

int __cdecl cus_puts(char* buf)
{
	if (ConsoleExists())
	{
		return puts(buf);
	}

	return 0;
}

