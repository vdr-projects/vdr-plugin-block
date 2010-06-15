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

#include <getopt.h>
#include <vdr/plugin.h>

#include "status.h"
#include "event.h"
#include "setup.h"
#include "config.h"
#include "i18n.h"
#include "control.h"

static const char *VERSION        = "0.0.3";
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
  //virtual bool ProcessArgs(int argc, char *argv[]);
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
    if (strcmp(title,temptitle)==0) return; //current show has already been checked
    temptitle=(char*)title;
    if (!EventsBlock.Acceptable(title))
    {
    isyslog("plugin-block: channel %d blocked", channelnumber);
    cControl::Launch(new cControlBlock(channel, present, follow));
    }
  }
}                                                                                                                                                

/*
bool cPluginBlock::ProcessArgs (int argc, char *argv[])
{
  static const char short_options[] = "p:";
  
  static const struct option long_options[] =
  {
    { "ParentalGuidance", required_argument, NULL, 'p' },
    {NULL}
  };
  
  int c;
  for (c=0;c<argc;c++)
  {
    dsyslog("plugin-block-DEV: argv[%d]=%s",c,argv[c]);
  }
  c=0;
  while ((c = getopt_long(argc,argv,short_options,long_options,NULL))!=-1)
  {
    switch (c)
    {
      case 'p': cSetupBlock::ParentalGuidance=atoi(optarg);
                //dsyslog("plugin-block-DEV: ParentalGuidance set to %s by cl argument.",optarg);
                break;
      default:  //dsyslog("plugin-block-DEV: unrecognized command line option %s",optarg );
                break;//usually return false, but parental guidance could also be set in setup.conf or not at all!
    }
  }
  return true;
}
*/
                                                                                                                                                                                                                                                                                                
VDRPLUGINCREATOR(cPluginBlock); // Don't touch this!
