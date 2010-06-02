/**
 * based on status.c,v 1.2 2007/05/08 10:16:23 thomas
 *
 * version by Midas
 *
 */

#include <vdr/channels.h>

#include "status.h"
#include "control.h"
#include "event.h"
#include "config.h"

cStatusBlock::cStatusBlock(void):
    cStatus(),
    mLastChannel(0) // int
{
}

void cStatusBlock::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{
  if (SetupBlock.DetectionMethod!=0)
  {
//    dsyslog("plugin-block-DEV: StatusBlock::ChannelSwitch did nothing: Detection Method didn't match");
    return;
  }
  //printf("cStatusBlock::ChannelSwitch(%p, %d)\n", Device, ChannelNumber);

#ifdef LOGGING
  dsyslog("plugin-block: cStatusBlock was informed about channel switch at device %d, channel no %d",Device->DeviceNumber(),ChannelNumber);
  dsyslog("plugin-block: cDevice::CurrentChannel %d",cDevice::CurrentChannel());
  dsyslog("plugin-block: Device %d, ActualDevice %d, primary Device %d",Device->DeviceNumber(),cDevice::ActualDevice()->DeviceNumber(),cDevice::PrimaryDevice()->DeviceNumber());
  for (int ii=0;ii<cDevice::NumDevices();ii++)
  {
    cDevice* tmpdev=cDevice::GetDevice(ii);
    dsyslog("plugin-block: cDevice %d replaying: %d, transferring %d",ii,tmpdev->Replaying(),tmpdev->Transferring());
  }
#endif


  if (Device->DeviceNumber()!=cDevice::PrimaryDevice()->DeviceNumber())
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing cause ChannelSwitch not on active livedevice.");
#endif
    return;
  }

  if (mLastChannel==0)
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing cause mLastChannel=0 (set to %d)",cDevice::CurrentChannel());
#endif
    mLastChannel=cDevice::CurrentChannel();
    return;
  }
  
  if (ChannelNumber==0) 
  {
    mLastChannel=cDevice::CurrentChannel(); 
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing because ChannelNumber=0 (some switch is in progress)"); 
#endif
    return; //seems that switching is in progress
  }
  
  if (ChannelNumber!=cDevice::CurrentChannel())
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing because ChannelNumber!=CurrentChannel (switch still in progress)");
#endif
    return;
  }
  
  if (Device->Replaying()) 
  {
#ifdef LOGGING
    dsyslog("plugin-block: Did nothing because Device is Replaying");
#endif
    return; 
  }

  const cChannel *channel = Channels.GetByNumber(ChannelNumber);
  if (channel != NULL && !channel->GroupSep()) 
  {
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
    if (scheds == NULL) return;

    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL) return;

    const cEvent *present = sched->GetPresentEvent();
    const cEvent *follow  = sched->GetFollowingEvent();
    if (present == NULL) return;
          		
    if (!cControlBlock::IsRequested() && !EventsBlock.Acceptable(present->Title())) 
    {
      isyslog("plugin-block: channel %d is not acceptable at present", ChannelNumber);
      cControl::Launch(new cControlBlock(mLastChannel, channel, present, follow));
      mLastChannel=0;
    }
  }
}
  
