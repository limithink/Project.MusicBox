//Copyright 2016 buildworld.net. All rights reserved.
#pragma once
#include "Global.h"

int CommandInterpreter(pOPT opt);
int VerifyString(CHAR *poi, int limit);
int getString(CHAR *src, CHAR *obj, int num, int limit);
int str2num(CHAR *src);