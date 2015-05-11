/*
** nw.gateplus~.c
**
** MSP object
** open and close gate to allow a signal through but
** starts & stops only after zero crossing occurs
** 2015/05/10 started
**
** Copyright © 2015 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
**
*/

#include "ext.h"		// required for all MAX external objects
#include "ext_obex.h"   // required for new style MAX objects
#include "z_dsp.h"		// required for all MSP external objects
#include <string.h>

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.gateplus~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for gate stage flag */
#define GATE_CLOSED			0
#define MONITOR_OPEN		1
#define GATE_OPEN			2
#define MONITOR_CLOSED		3

static t_class *gateplus_class;		// required global pointing to this class

typedef struct _gateplus
{

} t_gateplus;
