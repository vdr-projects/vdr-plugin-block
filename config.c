/**
 * based on config.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

cSetupBlock SetupBlock;

cSetupBlock::cSetupBlock(void):
	HideMenuEntry(0),
	MessageTimeout(2),
	DetectionMethod(0)
{
}

bool cSetupBlock::Parse(const char *Name, const char *Value)
{
  if (strcmp(Name, "HideMenuEntry")  == 0) HideMenuEntry  = atoi(Value);
  else if (strcmp(Name, "MessageTimeout") == 0) MessageTimeout = atoi(Value);
  else if (strcmp(Name, "DetectionMethod") == 0) DetectionMethod = atoi(Value);
  else return false;
  return true;
}
