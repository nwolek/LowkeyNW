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
    long sample_count;      // non-zero during stages 1,2,3
    
    //history
    double last_ctrl_in;
    double last_sig_in;
    
} t_gateplus;

/* method definitions for this object */
void *gateplus_new(long outlets);
void gateplus_dsp64(t_gateplus *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void gateplus_perform64(t_gateplus *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void gateplus_int(t_gateplus *x, long l);
void gateplus_assist(t_gateplus *x, t_object *b, long msg, long arg, char *s);
void gateplus_getinfo(t_gateplus *x);


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
                  (short)sizeof(t_gateplus), 0L, A_DEFLONG, 0);
    class_dspinit(c); // add standard functions to class
    
    /* bind method "gateplus_int" to the int message */
    class_addmethod(c, (method)gateplus_int, "int", A_LONG, 0);
    
    /* bind method "gateplus_assist" to the assistance message */
    class_addmethod(c, (method)gateplus_assist, "assist", A_CANT, 0);
    
    /* bind method "gateplus_getinfo" to the getinfo message */
    class_addmethod(c, (method)gateplus_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "gateplus_dsp64" to the dsp64 message */
    class_addmethod(c, (method)gateplus_dsp64, "dsp64", A_CANT, 0);
    
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
    outlet_new((t_pxobject *)x, "signal");			// outlet for signal to pass through
    outlet_new((t_pxobject *)x, "signal");			// outlet for sample count
    
    /* setup variables */
    x->last_ctrl_in = 0.0;
    x->last_sig_in = 0.0;
    x->sample_count = 0;
    
    /* set flags to defaults */
    x->gate_stage = GATE_CLOSED;
    
    x->x_obj.z_misc = Z_NO_INPLACE;
    
    #ifdef DEBUG
        post("%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    /* return a pointer to the new object */
    return (x);
}

/********************************************************************************
 void gateplus_dsp64()
 
 inputs:     x		-- pointer to this object
 dsp64		-- signal chain to which object belongs
 count	-- array detailing number of signals attached to each inlet
 samplerate -- number of samples per second
 maxvectorsize -- sample frames per vector of audio
 flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void gateplus_dsp64(t_gateplus *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
#ifdef DEBUG
    post("%s: adding 64 bit perform method", OBJECT_NAME);
#endif /* DEBUG */
    
    if (count[1]) { // if signal input is connected
        #ifdef DEBUG
            post("%s: output is being computed", OBJECT_NAME);
        #endif /* DEBUG */
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)gateplus_perform64, 0, NULL);
    } else {
        #ifdef DEBUG
            post("%s: no output computed", OBJECT_NAME);
        #endif /* DEBUG */
    }
    
}

/********************************************************************************
 void *gateplus_perform64()
 
 inputs:	x		--
 dsp64   --
 ins     --
 numins  --
 outs    --
 numouts --
 vectorsize --
 flags   --
 userparam  --
 description:	called at interrupt level to compute object's output at 64-bit,
 writes zeros to every outlet
 returns:		nothing
 ********************************************************************************/
void gateplus_perform64(t_gateplus *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars outlets and inlets
    t_double *in_ctrl = ins[0];
    t_double *in_signal = ins[1];
    t_double *out_signal = outs[0];
    t_double *out_count = outs[1];
    
    // local vars for object vars and while loop
    long n, count_samp;
    short g_stage;
    double lc_in, ls_in;
    
    // check to make sure object is enabled
    if (x->x_obj.z_disabled) goto out; // if not, skip ahead
    
    // assign values to local vars
    g_stage = x->gate_stage;
    lc_in = x->last_ctrl_in;
    ls_in = x->last_sig_in;
    count_samp = x->sample_count;
    
    n = vectorsize;
    while (n--) {
        
        // test control input for change
        if ((lc_in == 0.) != (*in_ctrl == 0.)) {
            
            // what we do depends on the gate stage
            switch (g_stage)
            {
                case GATE_CLOSED:
                    ++g_stage; // change to MONITOR_OPEN
                    break;
                case MONITOR_OPEN:
                    --g_stage; // change to GATE_CLOSED
                    break;
                case GATE_OPEN:
                    ++g_stage; // change to MONITOR_CLOSED
                    break;
                case MONITOR_CLOSED:
                    --g_stage; // change to GATE_OPEN
                    break;
            }
            
        }
        
        // if we are monitoring...
        if (g_stage % 2)
        {
            // look for positive zero-crossing
            if (ls_in < 0. && *in_signal >= 0.)
            {
                // and change gate stage
                switch (g_stage)
                {
                    case MONITOR_OPEN:
                        ++g_stage; // change to GATE_OPEN
                        break;
                    case MONITOR_CLOSED:
                        g_stage = GATE_CLOSED;
                        break;
                }
            }
        }
        
        // let sound through under right conditions
        if (g_stage > MONITOR_OPEN) // if GATE_OPEN or MONITOR_CLOSED
        {
            *out_signal = *in_signal;
        } else {
            *out_signal = 0.;
        }
        
        // if gate isn't open or monitoring...
        if (g_stage == 0)
        {
            // don't count the samples
            count_samp = 0;
        } else {
            // otherwise count the samples
            count_samp++;
        }
        
        // write sample count output
        *out_count = (double)count_samp;
        
        // update history
        lc_in = *in_ctrl;
        ls_in = *in_signal;
        
        // advance pointers
        ++in_ctrl, ++in_signal, ++out_signal;
    }
    
    // update global vars
    x->gate_stage = g_stage;
    x->last_ctrl_in = lc_in;
    x->last_sig_in = ls_in;
    x->sample_count = count_samp;
    
out:
    return;
    
}

/********************************************************************************
 void gateplus_int(t_gateplus *x, long l)
 
 inputs:			x		-- pointer to our object
 l		-- value of int input
 description:	handles ints sent to inlets
 returns:		nothing
 ********************************************************************************/
void gateplus_int(t_gateplus *x, long l)
{
    if (x->x_obj.z_in == 0) // if first inlet
    {
        // do something with the gate stage
        post("%s: ints not implemented yet", OBJECT_NAME);
    }
    else if (x->x_obj.z_in == 1) // if second inlet
    {
        post("%s: this inlet does not accept ints", OBJECT_NAME);
    }
}

/********************************************************************************
 void gateplus_assist(t_gateplus *x, t_object *b, long msg, long arg, char *s)
 
 inputs:			x		-- pointer to our object
 b		--
 msg		--
 arg		--
 s		--
 description:	method called when "assist" message is received; allows inlets
 and outlets to display assist messages as the mouse passes over them
 returns:		nothing
 ********************************************************************************/
void gateplus_assist(t_gateplus *x, t_object *b, long msg, long arg, char *s)
{
    if (msg==ASSIST_INLET) {
        switch (arg) {
            case 0:
                strcpy(s, "(signal/int) control of gate");
                break;
            case 1:
                strcpy(s, "(signal) input");
                break;
        }
    } else if (msg==ASSIST_OUTLET) {
        switch (arg) {
            case 0:
                strcpy(s, "(signal) output");
                break;
            case 1:
                strcpy(s, "(signal) sample count while active");
                break;
        }
    }
    
#ifdef DEBUG
    post("%s: assist message displayed", OBJECT_NAME);
#endif /* DEBUG */
}

/********************************************************************************
 void gateplus_getinfo(t_gateplus *x)
 
 inputs:			x		-- pointer to our object
 
 description:	method called when "getinfo" message is received; displays info
 about object and last update
 returns:		nothing
 ********************************************************************************/
void gateplus_getinfo(t_gateplus *x)
{
    post("%s object by Nathan Wolek", OBJECT_NAME);
    post("Last updated on %s - www.nathanwolek.com", __DATE__);
}

