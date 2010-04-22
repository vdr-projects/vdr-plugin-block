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

cControlBlock::cControlBlock(int LastChannel, const cChannel *Channel, const cEvent *Present, const cEvent *Following):
    cControl(new cPlayer),
    mLastChannel(LastChannel),
		mChannel(Channel),
		mPresent(Present),
		mFollowing(Following),
		mStart(0),
		mSwitch(true),
		mOsd(NULL)
{
#if APIVERSNUM >= 10500
	SetNeedsFastResponse(true);
#else
	needsFastResponse = true;
#endif
	cRemote::Put(kOk, true); // Hide OSD
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
        }

	if (mSwitch) {
		// possibly first or last available channel, fall back to old channel
		int direction = mChannel->Number() - mLastChannel;
		if (direction == 0)
			direction = 1;
		if (!cDevice::SwitchChannel(direction) && (mLastChannel != 0))
			Channels.SwitchTo(mLastChannel);
	}
}

void cControlBlock::Show(void)
{
	if (mOsd == NULL)
		mOsd = Skins.Current()->DisplayChannel(true);

	mOsd->SetChannel(mChannel, 0);
	mOsd->SetEvents(mPresent, mFollowing);
	mOsd->SetMessage(mtError, tr("Channel not acceptable!"));
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
		mRequested = true;
		mSwitch = false;
		return osEnd;

	case kNone:
#ifdef LOGGING
dsyslog("plugin-block: userint Processing kNone (no user interaction)");
#endif
		if (mStart == 0)
			Show();
		else if (time_ms() - mStart > BlockTimeout()) {
			mSwitch = true;
			return osEnd;
		}
	  return osContinue;

		

	case kUp:
	case kDown:
	case kChanUp:
	case kChanDn:
#ifdef LOGGING
dsyslog("plugin-block: userint Processing k(Ch)Up/(Ch)Down event.");
#endif
		mRequested = false;
		mSwitch = true;
		return osEnd;
		break;

  default:
    break;
  }
  return osContinue;
}
