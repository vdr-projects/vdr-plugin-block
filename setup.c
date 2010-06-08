/**
 * based on setup.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include "setup.h"

#include <vdr/interface.h>

cMenuSetupBlock::cMenuSetupBlock():
    cMenuSetupPage()
{
  mEventsData = EventsBlock; // need to use copy constructor
  mSetupData  = SetupBlock;
  Set();
}

void cMenuSetupBlock::Set(void) {
  int current = Current();
  cOsdItem *item;

  Clear();

  Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"), &mSetupData.HideMenuEntry));
  Add(new cMenuEditIntItem(tr("Message Timeout [s]"),  &mSetupData.MessageTimeout, 0, 10));

  DetectionMethods[0] = tr("On Switch");
  DetectionMethods[1] = tr("Channel EPG");

  if(mSetupData.ExtraOptionsVisible==1)
  {
    Add(new cMenuEditStraItem(tr("Detection Method"), &mSetupData.DetectionMethod, 2, DetectionMethods));
    Add(new cMenuEditBoolItem(tr("Ok deblocks temporarily"), &mSetupData.OkAllowed));
  }
  else
  {
    cMenuEditStraItem *methoditem = new cMenuEditStraItem(tr("Detection Method"), &mSetupData.DetectionMethod, 2, DetectionMethods);
    cMenuEditBoolItem *okitem = new cMenuEditBoolItem(tr("Ok deblocks temporarily"), &mSetupData.OkAllowed);
    methoditem->SetSelectable(false);
    okitem->SetSelectable(false);
    Add(methoditem);
    Add(okitem);
//    Add((new cMenuEditStraItem(tr("Detection Method"), &mSetupData.DetectionMethod, 2, DetectionMethods))::SetSelectable(false));
//    Add(new cMenuEditBoolItem(tr("Ok deblocks temporarily"), &mSetupData.ExtraOptionsVisible));
  }  

  
  item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);

  item = new cOsdItem(tr("--- Keywords -------------------------------------------------------------------"));
  item->SetSelectable(false);
  Add(item);

#define NONKEYWORDITEMS 6

  int index = 0;
  cEventBlock *event = mEventsData.First();
  while (event != NULL) {
    Add(new cOsdItem(event->Pattern()));
    event = mEventsData.Next(event);
    ++index;
  }

  SetCurrent(Get(current));
  SetHelpKeys();
  Display();
}

void cMenuSetupBlock::SetHelpKeys(void)
{
  const char *red    = NULL;
  const char *yellow = NULL;

  printf("sethelpkeys, current = %d\n", Current());

  if (Current() >= NONKEYWORDITEMS) {
	red    = trVDR("Button$Edit");
	yellow = trVDR("Button$Delete");
  }
  SetHelp(red, trVDR("Button$New"), yellow, NULL);
}

void cMenuSetupBlock::Store(void) 
{
  EventsBlock = mEventsData;
  EventsBlock.Save();

  SetupBlock  = mSetupData;
  SetupStore("HideMenuEntry",  SetupBlock.HideMenuEntry);
  SetupStore("MessageTimeout", SetupBlock.MessageTimeout);
  SetupStore("DetectionMethod", SetupBlock.DetectionMethod);
  SetupStore("OkAllowed", SetupBlock.OkAllowed);
}

eOSState cMenuSetupBlock::Edit(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;

  cEventBlock *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL)
    return AddSubMenu(new cMenuSetupEditBlock(event));
  return osContinue;
}

eOSState cMenuSetupBlock::New(void)
{
  if (HasSubMenu())
    return osContinue;

  mEventsData.Add(new cEventBlock());
  Set();
  return osContinue;
}

eOSState cMenuSetupBlock::Delete(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;
        
  cEventBlock *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL) {
	if (Interface->Confirm(tr("Delete keyword?")))
	  mEventsData.Del(event);
  }
  Set();
  return osContinue;
}

eOSState cMenuSetupBlock::ProcessKey(eKeys Key) {
  bool hadSubMenu = HasSubMenu();
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (hadSubMenu && !HasSubMenu()) {
		Set();
		return state;
  }

  switch (state) {
  case osUnknown: // normal key handling
    switch (Key) {
    case kRed:    if (mSetupData.ExtraOptionsVisible==1) return Edit();
    case kGreen:  return New();
    case kYellow: if (mSetupData.ExtraOptionsVisible==1) return Delete();

    default:
      break;
    }
    break;

  default:
    break;
  }
	
	if (!HasSubMenu())
		Set();

  return state;
}

static const char *ALLOWEDCHARS = "$ abcdefghijklmnopqrstuvwxyz0123456789-_.#~/[]{}()+*^$";

cMenuSetupEditBlock::cMenuSetupEditBlock(cEventBlock *Event):
    cOsdMenu("", 33),
    mEvent(Event),
    mData(*Event)
{
  char buf[80];
  snprintf(buf, sizeof(buf), "%s - %s '%s'", trVDR("Setup"), trVDR("Plugin"), "block");
  SetTitle(buf);
  Add(new cMenuEditStrItem(tr("Pattern"), mData.mPattern, sizeof(mData.mPattern), tr(ALLOWEDCHARS)));
	Add(new cMenuEditBoolItem(tr("Regular Expression"), &mData.mRegularExp));
	Add(new cMenuEditBoolItem(tr("Ignore Case"), &mData.mIgnoreCase));
}

eOSState cMenuSetupEditBlock::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
    case kOk:
			if (!mData.Compile()) {
				Skins.Message(mtError, tr("Malformed regular expression!"));
				state = osContinue;
			} else {
				*mEvent = mData;
				state = osBack;
			}
      break;

    default:
      break;
    }
  }
  return state;
}
