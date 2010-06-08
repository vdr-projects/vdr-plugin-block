/**
 * block.c: A plugin for the Video Disk Recorder
 *
 * based on taste.c v 1.1.1.1 2006/02/26 14:11:02 by lordjaxom
 *
 * See the README file for copyright and contact information.
 *
 * version by Midas
 *
 */

#include <vdr/plugin.h>

#include "status.h"
#include "event.h"
#include "setup.h"
#include "config.h"
#include "i18n.h"
#include "control.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = trNOOP("Block unwanted shows by EPG title");
static const char *MAINMENUENTRY  = trNOOP("Block broadcast");

static int channelnumber=0;
static char *temptitle=NULL;


class cPluginBlock : public cPlugin {
private:
  cStatusBlock *mStatus;

public:
  cPluginBlock(void);
  virtual ~cPluginBlock();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual const char *MainMenuEntry(void) { return SetupBlock.HideMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual void MainThreadHook(void);
  };

cPluginBlock::cPluginBlock(void):
    cPlugin(),
    mStatus(NULL)
{
  temptitle=(char*)"";
}

cPluginBlock::~cPluginBlock()
{
  delete mStatus;
}

bool cPluginBlock::Initialize(void)
{
  return EventsBlock.Load(AddDirectory(cPlugin::ConfigDirectory(), "block.conf"), true, false);
}

bool cPluginBlock::Start(void)
{
#if VDRVERSNUM < 10507 
  RegisterI18n(Phrases);
#endif 
  mStatus = new cStatusBlock();
  return true;
}

cOsdObject *cPluginBlock::MainMenuAction(void)
{
  const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel != NULL && !channel->GroupSep()) { 
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
    if (scheds == NULL)
      return NULL;

    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
      return NULL;

    const cEvent *present = sched->GetPresentEvent();
    EventsBlock.Add(new cEventBlock(present->Title()));
    EventsBlock.Save();
  }
  return NULL;
}

cMenuSetupPage *cPluginBlock::SetupMenu(void)
{
  return new cMenuSetupBlock();
}

bool cPluginBlock::SetupParse(const char *Name, const char *Value)
{
  return SetupBlock.Parse(Name, Value);
}

void cPluginBlock::MainThreadHook()
{
  if (cSetupBlock::DetectionMethod!=1) return;//other detection method active in setup
  channelnumber=cDevice::PrimaryDevice()->CurrentChannel();

  if (channelnumber==0) return; //switch in progress

  const cChannel *channel=Channels.GetByNumber(channelnumber);

  if (channel != NULL && !channel->GroupSep())
  {
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
            
    if (scheds == NULL)
    {
      return;
    }
                                    
    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
    {
      return;
    }
                                                                
    const cEvent *present = sched->GetPresentEvent();
    const cEvent *follow  = sched->GetFollowingEvent();
                                                                        
    if (present == NULL)
    {
      return;
    }
                        
    //TODO: check if isrequested is still necessary
//    if (!cControlBlock::IsRequested() && !EventsBlock.Acceptable(present->Title()))
    const char* title=present->Title();
//    dsyslog("plugin-block-DEV: comparing '%s' with '%s'.",title,temptitle);
    if (strcmp(title,temptitle)==0) return; //current show has already been checked
    temptitle=(char*)title;
    if (!EventsBlock.Acceptable(title))
    {
    isyslog("plugin-block: channel %d is not acceptable at present", channelnumber);
    cControl::Launch(new cControlBlock(channel, present, follow));
    }
  }
}                                                                                                                                                



                                                                                                                                                                                                                                                                                                
VDRPLUGINCREATOR(cPluginBlock); // Don't touch this!
