/**
 * based on status.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_STATUS_H
#define VDR_BLOCK_STATUS_H

#include <vdr/status.h>

class cStatusBlock : public cStatus {
private:

protected:
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
  virtual void Replaying(const cControl *Control,
                         const char *Name,
                         const char *FileName, bool On);
  
public:
  cStatusBlock(void);
};

#endif // VDR_BLOCK_STATUS_H
