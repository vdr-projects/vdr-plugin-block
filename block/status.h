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
  int mLastChannel;

protected:
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);

public:
  cStatusBlock(void);
};

#endif // VDR_BLOCK_STATUS_H
