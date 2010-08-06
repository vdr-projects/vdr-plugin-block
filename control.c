/**
 * based on control.c,v 1.4 2007/06/18 17:58:52 lordjaxom
 *
 * version by Midas
 *
 */

#include "control.h"
#include "config.h"
#include "common.h"

#include <vdr/remote.h>

inline uint64_t BlockTimeout() { return SetupBlock.MessageTimeout * 1000; }

bool cControlBlock::mRequested = false;

cControlBlock::cControlBlock(const cChannel *Channel, const cEvent *Present, const cEvent *Following):
    cControl(new cPlayer),
		mChannel(Channel),
		mPresent(Present),
		mFollowing(Following),
		mStart(0),
		mSwitch(true),
		mOsd(NULL),
		direction(0)
{
#if APIVERSNUM >= 10500
	SetNeedsFastResponse(true);
#else
	needsFastResponse = true;
#endif

  cRemote::Put(kBack,true); //Hide OSD new version
}

cControlBlock::~cControlBlock()
{
	if (mOsd != NULL)
		delete mOsd;
        if (mRequested)
        {
#ifdef LOGGING
          dsyslog("plugin-block: userint user requested to watch blocked channel");
#endif  
          //if (mChannel!=0)
          if (mChannel != NULL)
          {
            cDevice::PrimaryDevice()->SwitchChannel(mChannel, true);
          }
          else
          {
             //don't know if this is necessary - just taken from plugin taste
             //seems that mChannel cannot be 0 because it is an object reference
             //changed that to NULL
             dsyslog("plugin-block: userint Cannot switch - channel unknown!");
          }
          mRequested=false;
        }

	if (mSwitch) {
	        int lastchannel=cSetupBlock::LastChannel;
		// possibly first or last available channel, fall back to old channel

		if (direction == 0)
			direction = 1;
		if (!cDevice::SwitchChannel(direction) && (lastchannel != 0))
			Channels.SwitchTo(lastchannel);

	}
}

void cControlBlock::Show(void)
{
	if (mOsd == NULL)
		mOsd = Skins.Current()->DisplayChannel(true);

	mOsd->SetChannel(mChannel, 0);
	mOsd->SetEvents(mPresent, mFollowing);
	mOsd->SetMessage(mtError, tr("Current show blocked!"));
	mOsd->Flush();

	mStart = time_ms();
}

eOSState cControlBlock::ProcessKey(eKeys Key)
{
#ifdef LOGGING
	dsyslog("plugin-block: userint cControlBlock::ProcessKey(%d) this = %p", Key, this);
#endif

  switch (Key) {

	case kOk:
#ifdef LOGGING
		dsyslog("plugin-block: userint Processing 'Ok' event");
#endif		
		if (cSetupBlock::OkAllowed==1)
		{
		  mRequested = true;
		  mSwitch = false;
		  return osEnd;
                }
                else
                {
                  mRequested = false;
                  mSwitch = true;
                  if (mOsd != NULL)
                  {
                    mOsd->SetMessage(mtError, tr("Permission denied!"));
                    mOsd->Flush();
                  }
                }
                return osContinue;
                
	case kNone:
#ifdef LOGGING
dsyslog("plugin-block: userint Processing kNone (no user interaction)");
#endif
		if (mStart == 0)
		{
			Show();
                }
		else if (time_ms() - mStart > BlockTimeout()) {
		direction = mChannel->Number() - cSetupBlock::LastChannel;
			mSwitch = true;
			return osEnd;
		}
	  return osContinue;

	case kChanUp:
                //workaround cause 'normal' code wont work here; vdr does not react on code/destructor here o.O
                //this is because in case of chan +/- keys have a chance to get eaten by vdr itself
                cRemote::Put(kUp,true);
                return osContinue;
                break;
        
	case kUp:                
#ifdef LOGGING
dsyslog("plugin-block: userint Processing up event (userrequest)");
#endif
		if (mStart == 0)
			Show();
		else 
		{
		  mRequested=false;//TODO:necessary? as above
		  direction = 1;
		  mSwitch = true;
		  return osEnd;
		}
	        break;

	case kChanDn:
                //workaround cause 'normal' code wont work here; vdr does not react on code/destructor here o.O
                //this is because in case of chan +/- keys have a chance to get eaten by vdr itself
                cRemote::Put(kDown,true);
                return osContinue;
                break;
        
	case kDown:
#ifdef LOGGING
dsyslog("plugin-block: userint Processing down event (userrequest)");
#endif
		if (mStart == 0)
			Show();
		else 
		{
		  mRequested=false;//TODO:necessary? as above
		  direction = -1;
		  mSwitch = true;
		  return osEnd;
		}
	        break;

        default:
                break;
  }
  return osContinue;
}

