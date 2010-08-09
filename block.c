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
#include "common.h"
#include <vdr/interface.h>

static const char *VERSION        = "0.0.4+201008090412";
static const char *DESCRIPTION    = trNOOP("Block unwanted shows by EPG title");
static const char *MAINMENUENTRY  = trNOOP("(De)Block broadcast");

static int channelnumber=0;
#ifdef LOGGING
  char *channel_groupsep_string=(char*)"";  
  char *has_been_checked_string=(char*)"";
  bool temp_replaying_recording=false;
#endif


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
  cEventBlock::LastTitle=(char*)"block_dummy_title1";
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
    
    if (EventsBlock.Acceptable(present->Title()))
    {
     EventsBlock.Add(new cEventBlock(present->Title()));
     EventsBlock.Sort();
     EventsBlock.Save();
    }
    else
    {
     int index=0;
     const cEventBlock* blockeventpointer = EventsBlock.First();
     while (blockeventpointer != NULL)
     {
      if (strcmp(blockeventpointer->Pattern(),present->Title())==0)
       break;
      index+=1;
      blockeventpointer=EventsBlock.Next(blockeventpointer);
     }      
     cEventBlock *event=EventsBlock.Get(index);
     if (event!=NULL)
     {
      if (Interface->Confirm(tr("Delete keyword?")))
       EventsBlock.Del(event);
       EventsBlock.Save();
     }
    }
    
    cEventBlock::LastTitle="block_dummy_title2";
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

  if (cEventBlock::ReplayingRecording) //no block events on the underlying channel processed - user watches recording
  {
#ifdef LOGGING
   if (!temp_replaying_recording)
   {
    temp_replaying_recording=true;
    dsyslog("plugin-block: doing nothing because user watches recording");
   }
#endif   
   return;
  }
#ifdef LOGGING
  else
  {
   if (temp_replaying_recording)
   {
    temp_replaying_recording=false;
    dsyslog("plugin-block: replay of recording ended. Resuming detection mode.");
   }
   return;
  }
#endif      

  channelnumber=cDevice::PrimaryDevice()->CurrentChannel();
  if (channelnumber==0) 
  {
#ifdef LOGGING
   dsyslog("plugin-block: Channel number is 0 => Channel switch on primary device");
#endif
   return; //switch in progress
  }

  const cChannel *channel=Channels.GetByNumber(channelnumber);

  if (channel != NULL && !channel->GroupSep())
  {
#ifdef LOGGING
    char *temp_string;
    asprintf(&temp_string,"channel: %d channel->GroupSep(): %d", channel->Number(), channel->GroupSep());
    if (strcmp(temp_string,channel_groupsep_string)!=0)
    {
     dsyslog("plugin-block: %s",temp_string);
     channel_groupsep_string=temp_string;
    }
#endif
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
            
    if (scheds == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: doing nothing because scheds==NULL");             
#endif      
      return;
    }
                                    
    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
    {
     char *dummy;
     asprintf(&dummy, "%jd", (intmax_t)time_ms());
     dsyslog("plugin-block: no EPG data - using dummy for LastTitle: %s",dummy);
     cEventBlock::LastTitle=(char*)dummy;
#ifdef LOGGING    
     dsyslog("plugin-block: doing nothing because sched==NULL");
#endif
     return;
    }
                                                                
    const cEvent *present = sched->GetPresentEvent();
    const cEvent *follow  = sched->GetFollowingEvent();
                                                                        
    if (present == NULL)
    {
#ifdef LOGGING
      dsyslog("plugin-block: doing nothing because present==NULL");
#endif
      return;
    }
                        
    //TODO: check if isrequested is still necessary
//    if (!cControlBlock::IsRequested() && !EventsBlock.Acceptable(present->Title()))
    const char* title=present->Title();
    if (strcmp(title,"")==0)
    {
     char *dummy;
     asprintf(&dummy, "%jd", (intmax_t)time_ms());
     dsyslog("plugin-block: no current EPG title - using dummy for LastTitle: %s",title);
    }
  
    if (strcmp(title,cEventBlock::LastTitle)==0) 
    {
#ifdef LOGGING
     char *temp_string;
     asprintf(&temp_string, "'%s' has already been checked",title);
     if (strcmp(temp_string,has_been_checked_string)!=0)
     {
      dsyslog("plugin-block: %s",temp_string);
     has_been_checked_string=temp_string;
     }
#endif     
     return; //current show has already been checked
    }
#ifdef LOGGING
    dsyslog("plugin-block: new EPG title detected: '%s' - comparing with '%s'",title, cEventBlock::LastTitle);
#endif    
    cEventBlock::LastTitle=(char*)title;
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
