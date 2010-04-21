/**
 * based on event.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_EVENT_H
#define VDR_BLOCK_EVENT_H

#include <sys/types.h>
#include <regex.h>
							
#include <vdr/tools.h>
#include <vdr/config.h>

#define EVLINELENGTH 256

class cEventBlock : public cListObject {
  friend class cMenuSetupEditBlock;

private:
  char mPattern[EVLINELENGTH];
	int  mRegularExp;
	int  mIgnoreCase;
	bool mCompiled;

	regex_t mExpression;

public:
  cEventBlock(void);
  cEventBlock(const char *Pattern);
	cEventBlock(const cEventBlock &Src);
	~cEventBlock();

	cEventBlock &operator=(const cEventBlock &Src);

  bool Acceptable(const char *Event) const ;

  bool Parse(char *s);
	bool Compile(void);
  bool Save(FILE *f);

  const char *Pattern(void) const { return mPattern; }
};

class cEventsBlock : public cConfig<cEventBlock> {
public:
  bool Acceptable(const char *Event);
  cEventsBlock &operator=(const cEventsBlock &Source);
};

extern cEventsBlock EventsBlock;

#endif // VDR_BLOCK_EVENT_H
