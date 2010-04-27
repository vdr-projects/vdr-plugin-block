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

static const char *VERSION        = "0.0.1b+bf1";
static const char *DESCRIPTION    = trNOOP("Block unwanted shows by EPG title");
static const char *MAINMENUENTRY  = trNOOP("Block broadcast");

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
  };

cPluginBlock::cPluginBlock(void):
    cPlugin(),
    mStatus(NULL)
{
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

VDRPLUGINCREATOR(cPluginBlock); // Don't touch this!
