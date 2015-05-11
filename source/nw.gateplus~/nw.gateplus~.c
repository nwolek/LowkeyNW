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

#define DEBUG			//enable debugging messages

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
    t_pxobject x_obj;					// <--
    
    // current gate info
    short gate_stage;       // see flags in header
    
    //history
    double last_ctrl_in;
    double last_sig_in;
    
} t_gateplus;

/* method definitions for this object */
void *gateplus_new(long outlets);


/********************************************************************************
 void main(void)
 
 inputs:			nothing
 description:	called the first time the object is used in MAX environment;
 defines inlets, outlets and accepted messages
 returns:		int
 ********************************************************************************/
int C74_EXPORT main(void)
{
    t_class *c;
    
    c = class_new(OBJECT_NAME, (method)gateplus_new, (method)dsp_free,
                  (short)sizeof(t_gateplus), 0L, 1, 0);
    class_dspinit(c); // add standard functions to class
    
    class_register(CLASS_BOX, c); // register the class w max
    gateplus_class = c;
    
    #ifndef DEBUG
        
    #endif /* DEBUG */
}

/********************************************************************************
 void *gateplus_new(long outlets)
 
 inputs:			outlets		-- NOT USED YET`
 description:	called for each new instance of object in the MAX environment;
 defines inlets and outlets; sets argument for number of outlets
 returns:		nothing
 ********************************************************************************/
void *gateplus_new(long outlets)
{
    t_gateplus *x = (t_gateplus *) object_alloc((t_class*) gateplus_class);
    
    dsp_setup((t_pxobject *)x, 2);					// two inlets
    outlet_new((t_pxobject *)x, "signal");			// one outlet
    
    /* setup variables */
    x->last_ctrl_in = 0.0;
    x->last_sig_in = 0.0;
    
    /* set flags to defaults */
    x->gate_stage = GATE_CLOSED;
    
    x->x_obj.z_misc = Z_NO_INPLACE;
    
    #ifdef DEBUG
        post("%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    /* return a pointer to the new object */
    return (x);
}

                  
