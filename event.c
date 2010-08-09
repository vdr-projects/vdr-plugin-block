/**
 * based on event.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include "event.h"

#include <ctype.h>

char* cEventBlock::duptolower(const char *s) {
	char *c = strdup(s);
	char *p = c;
	for (; *p; ++p)
		*p = tolower(*p);
	return c;
}

const char* cEventBlock::LastTitle="block_dummy_title3";
const bool* cEventBlock::ReplayingRecording=false;

cEventsBlock EventsBlock;

cEventBlock::cEventBlock(void):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false)
{
  strncpy(mPattern, tr("New Entry"), sizeof(mPattern));
}

cEventBlock::cEventBlock(const char *Pattern):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false)
{
  strncpy(mPattern, Pattern, sizeof(mPattern));
}

cEventBlock::cEventBlock(const cEventBlock &Src)
{
	operator=(Src);
}

cEventBlock &cEventBlock::operator=(const cEventBlock &Src)
{
	printf("copy construct\n");
	strcpy(mPattern, Src.mPattern);
	mRegularExp = Src.mRegularExp;
	mIgnoreCase = Src.mIgnoreCase;
	mCompiled   = false;
	Compile();
	return *this;
}

cEventBlock::~cEventBlock()
{
	if (mRegularExp)
		regfree(&mExpression);
}

bool cEventBlock::Acceptable(const char *Event) const 
{
	if (mRegularExp)
		return regexec(&mExpression, Event, 0, NULL, 0) != 0;
	else if (mIgnoreCase) {
		char *ev = cEventBlock::duptolower(Event);
		char *pa = cEventBlock::duptolower(mPattern);
		printf("check for %s in %s\n", pa, ev);
		bool res = strstr(ev, pa) == NULL;
		free(ev); free(pa);
		return res;
	} else
		return strstr(Event, mPattern) == NULL;
}

bool cEventBlock::Parse(char *s) {
	char *patternbuf = NULL;
	int fields = sscanf(s, "%d:%d:%a[^\n]", &mRegularExp, &mIgnoreCase, &patternbuf);

	if (fields == 3) {
		strncpy(mPattern, skipspace(stripspace(patternbuf)), sizeof(mPattern));
		free(patternbuf);
	} else { // backward compatibility
		strncpy(mPattern, skipspace(stripspace(s)), sizeof(mPattern));
		mRegularExp = false;
		mIgnoreCase = false;
	}

  return Compile();
}

bool cEventBlock::Compile(void) {
	mCompiled = false;
	if (mRegularExp) {
		if (regcomp(&mExpression, mPattern, REG_EXTENDED | (mIgnoreCase ? REG_ICASE : 0)) != 0) {
			esyslog("ERROR: malformed regular expression: %s", mPattern);
			return false;
		} else
			mCompiled = true;
	}
	return true;
}

bool cEventBlock::Save(FILE *f) {
  return fprintf(f, "%d:%d:%s\n", mRegularExp, mIgnoreCase, mPattern) > 0;
}

bool cEventsBlock::Acceptable(const char *Event) {
  const cEventBlock *event = First();
  while (event != NULL) {
    if (!event->Acceptable(Event))
      return false;
    event = Next(event);
  }
  return true;
}

cEventsBlock &cEventsBlock::operator=(const cEventsBlock &Source) {
  cList<cEventBlock>::Clear();
  
  const cEventBlock *event = Source.First();
  while (event != NULL) {
    printf("transfering %p\n", event);
    Add(new cEventBlock(*event));
    event = Source.Next(event);
  }
  return *this;
}
