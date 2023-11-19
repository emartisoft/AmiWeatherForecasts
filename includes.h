#ifndef INCLUDES_H
#define INCLUDES_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/memory.h>
#include <exec/types.h>
#include <libraries/gadtools.h>
#include <intuition/intuition.h>
#include <exec/execbase.h>
#include <intuition/intuitionbase.h>
#include <libraries/gadtools.h>

#include <classes/window.h>
#include <gadgets/layout.h>
#include <proto/layout.h>
#include <proto/window.h>

#include <gadgets/button.h>
#include <proto/integer.h>
#include <gadgets/integer.h>
#include <proto/checkbox.h>
#include <gadgets/checkbox.h>
#include <proto/label.h>
#include <images/label.h>
#include <proto/chooser.h>
#include <gadgets/chooser.h>
#include <proto/string.h>
#include <gadgets/string.h>
#include <proto/palette.h>
#include <gadgets/palette.h>
#include <proto/space.h>
#include <gadgets/space.h>

#include <workbench/workbench.h>
#include <proto/dos.h>

#define SORRY           0xFF 
#define WAITONESECOND   0x32
#define PERIOD4UPDATE   0x0F
#define BUFFER          0x0400
#define PARAMETERCOUNT  0x0C

#define PATH_MAX    0x200
#define NAME_MAX    0x020

#define STREQUAL(s1, s2) (strcmp(s1, s2) == 0)

#define  ALL_REACTION_MACROS
#define  ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#endif
