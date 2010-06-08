/**
 * based on i18n.c,v 1.2 2006/07/05 20:19:56 thomas
 *
 * version by Midas
 */

#include "i18n.h"

#if VDRVERSNUM < 10507

const tI18nPhrase Phrases[] = {
	/*
  { "English",
    "Deutsch",
    "Slovenski",
    "Italiano",
    "Nederlands",
    "Portugu�s",
    "Fran�ais",
    "Norsk",
    "suomi", // this is not a typo - it's really lowercase!
    "Polski",
    "Espa�ol",
    "��������", // Greek
    "Svenska",
    "Rom�n�",
    "Magyar",
    "Catal�",
    "�������", // Russian
    "Hrvatski",
    "Eesti",
    "Dansk",
  },
	*/
  { "Block broadcast",
    "Sendung sperren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Block unwanted shows by EPG title",
    "Sendung/EPG-Titel sperren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Channel not acceptable",
    "Aktuelle Sendung unzumutbar",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "New Entry",
    "Neuer Eintrag",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Hide Mainmenu Entry",
    "Hauptmen�eintrag verstecken",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Detection Method",
    "Methode",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Ok deblocks temporarily",
    "Ok entsperrt tempor�r",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "On Switch",
    "Beim Umschalten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Channel EPG",
    "Kanal EPG",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Edit",
    "Editieren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Delete",
    "L�schen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "New",
    "Neu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Delete keyword?",
    "Schlagwort l�schen?",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Pattern",
    "Suchmuster",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "--- Keywords -------------------------------------------------------------------",
    "--- Schlagworte ----------------------------------------------------------------",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Regular Expression",
    "Regul�rer Ausdruck",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Ignore Case",
    "Gro�/Kleinschreibung ignorieren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Malformed regular expression!",
    "Ung�ltiger regul�rer Ausdruck!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ "Message Timeout [s]",
    "Wartezeit bis Umschalten [s]",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ NULL }
};

#endif
