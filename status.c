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
    cStatus()
{
}

void cStatusBlock::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{
  if (cSetupBlock::LastChannel==0)
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because LastChannel=0");
#endif    
    cSetupBlock::LastChannel=cDevice::CurrentChannel();
    return;
  }
  if (ChannelNumber==0)
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because ChannelNumber=0 (switch in progess)");
#endif
    cSetupBlock::LastChannel=cDevice::CurrentChannel();
    return; //Switch in progress;
  }
    
  if (cSetupBlock::DetectionMethod!=0) 
  {
#ifdef LOGGING
    dsyslog("plugin-block: ChannelSwitch returned because other detection method active");
#endif
    return;
  }

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
    if (scheds == NULL) 
    {
#ifdef LOGGING
      dsyslog("plugin-block: ChannelSwitch: scheds=null caused return");
#endif      
      return;
    }

    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: ChannelSwitch: sched=null caused return");
#endif      
      return;
    }
    const cEvent *present = sched->GetPresentEvent();
    const cEvent *follow  = sched->GetFollowingEvent();
    if (present == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: present=null return");
#endif
      return;
    }
    if (!cControlBlock::IsRequested() && !EventsBlock.Acceptable(present->Title())) 
    {
      dsyslog("plugin-block: channel %d blocked", ChannelNumber);
      cControl::Launch(new cControlBlock(channel, present, follow));
    }
  }
}
  
