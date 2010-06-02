/**
 * based on config.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_CONFIG_H
#define VDR_BLOCK_CONFIG_H

class cSetupBlock {
public:
  int  HideMenuEntry;
  int  MessageTimeout;
  int  DetectionMethod;
  
  cSetupBlock(void);
  bool Parse(const char *Name, const char *Value);
};

extern cSetupBlock SetupBlock;

#endif // VDR_BLOCK_CONFIG_H
