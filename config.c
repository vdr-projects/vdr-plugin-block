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

int cSetupBlock::LastChannel=0;
int cSetupBlock::DetectionMethod=0;
int cSetupBlock::OkAllowed=1;

cSetupBlock::cSetupBlock(void):
	HideMenuEntry(0),
	MessageTimeout(2),
	ExtraOptionsVisible(1)
{
}

bool cSetupBlock::Parse(const char *Name, const char *Value)
{
  if (strcmp(Name, "HideMenuEntry")  == 0) HideMenuEntry  = atoi(Value);
    else if (strcmp(Name, "MessageTimeout") == 0) MessageTimeout = atoi(Value);
      else if (strcmp(Name, "DetectionMethod") == 0) DetectionMethod = atoi(Value);

        //if you want to make use of the following you have to manually add Block.ExtraOptionsVisible to your setup.conf
        //0 means options 'Detection Method' and 'Ok temporarily deblocks' are not visible (eg for parental guidance)
        //1 means the options are visible in the setup of vdr and everybody could alter the values
        else if (strcmp(Name, "ExtraOptionsVisible") == 0) ExtraOptionsVisible = atoi(Value);

          else if (strcmp(Name, "OkAllowed")==0) OkAllowed = atoi(Value);
            else return false;
  return true;
}
