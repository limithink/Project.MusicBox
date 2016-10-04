//Copyright 2016 buildworld.net. All rights reserved.

#include "CommandInterpreter.h"

int CommandInterpreter(pOPT opt)
{
	CHAR raw[RAWCOMMAND], opera[MAX_OPERA], temp[MAX_NUM_BIT];
	fgets(raw, RAWCOMMAND, stdin);
	if (!VerifyString(raw, RAWCOMMAND)) return -1;
	if (getString(raw, opera, 1, RAWCOMMAND) < MIN_OPERA) return -2;
	//
	//param+path
	if (strcmp(opera, "create") == 0 || strcmp(opera, "wavgen") == 0)
	{
		if (strcmp(opera, "create") == 0) opt->opera = OPERA_CREATE;
		else opt->opera = OPERA_WAVGENERATION;
		//
		if (!getString(raw, temp, 2, RAWCOMMAND)) return -3;
		opt->param1 = str2num(temp);
		if (!getString(raw, temp, 3, RAWCOMMAND)) return -3;
		opt->param2 = str2num(temp);
		if (!getString(raw, temp, 4, RAWCOMMAND)) return -3;
		opt->param3 = str2num(temp);
		//
		if (!getString(raw, opt->path, 5, RAWCOMMAND)) return -3;
	}
	//path only
	else if (strcmp(opera, "open") == 0 || strcmp(opera, "saveas") == 0)
	{
		if (strcmp(opera, "open") == 0) opt->opera = OPERA_OPEN;
		else opt->opera = OPERA_SAVEAS;
		//
		if (!getString(raw, opt->path, 2, RAWCOMMAND)) return -3;
	}
	//param only
	else if (strcmp(opera, "play") == 0 || strcmp(opera, "set") == 0 ||
		strcmp(opera, "insert") == 0 || strcmp(opera, "format") == 0 ||
		strcmp(opera, "echo") == 0 || strcmp(opera, "rehead") == 0)
	{
		if (strcmp(opera, "play") == 0) opt->opera = OPERA_PLAY;
		else if (strcmp(opera, "set") == 0) opt->opera = OPERA_SET;
		else if (strcmp(opera, "insert") == 0) opt->opera = OPERA_INSERT;
		else if (strcmp(opera, "format") == 0) opt->opera = OPERA_FORMAT;
		else if (strcmp(opera, "echo") == 0)opt->opera = OPERA_ECHO;
		else opt->opera = OPERA_REHEAD;
		//
		if (!getString(raw, temp, 2, RAWCOMMAND)) return -3;
		opt->param1 = str2num(temp);
		if (!getString(raw, temp, 3, RAWCOMMAND)) return -3;
		opt->param2 = str2num(temp);
		if (strcmp(opera, "play") == 0 || strcmp(opera, "set") == 0 || strcmp(opera, "rehead") == 0)
		{
			if (!getString(raw, temp, 4, RAWCOMMAND)) return -3;
			opt->param3 = str2num(temp);
		}
	}
	else
	{
		if (strcmp(opera, "exit") == 0) opt->opera = OPERA_EXIT;
		else if (strcmp(opera, "save") == 0) opt->opera = OPERA_SAVE;
		else if (strcmp(opera, "abort") == 0) opt->opera = OPERA_ABORT;
		else if (strcmp(opera, "undo") == 0) opt->opera = OPERA_UNDO;
		else if (strcmp(opera, "help") == 0) opt->opera = OPERA_HELP;
		else
		{
			strcpy(opt->reserve, opera);
			return -4;
		}
	}
	return 0;
}

int VerifyString(CHAR *poi, int limit)
{
	int ctr;
	for (ctr = 0; ctr < limit; ctr++)
	{
		if (*(poi + ctr) == '\0')
			return 1;
	}
	return 0;
}

int getString(CHAR *src, CHAR *obj, int num, int limit)
{
	int ctr, cur;
	for (ctr = 0, cur = 0; ctr < limit; ctr++)
	{
		if (*(src + ctr) == ' ' || *(src + ctr) == '\n' || *(src + ctr) == '\0')
		{
			num--;
			ctr++;
			if (num == 0)
			{
				*(obj + cur) = '\0';
				return cur;
			}
		}
		if (num == 1)
		{
			*(obj + cur) = *(src + ctr);
			cur++;
		}
	}
	return 0;
}

int str2num(CHAR *src)
{
	int i, j, sum = 0;
	for (i = 0; *(src + i) != '\0'; i++)
	{
		if (*(src + i) < '0' || *(src + i) > '9') return 0;
	}
	for (i -= 1, j = 1; i >= 0; i--, j *= 10)
	{
		sum += (*(src + i) - '0')*j;
	}
	return sum;
}
