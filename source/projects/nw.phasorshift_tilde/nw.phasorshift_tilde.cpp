/*
** nw.phasorshift~.c
**
** MSP object
** generates evenly shifted phase signals
** 2001/08/09 started by Nathan Wolek
**
** Copyright © 2001,2014 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
** 
*/


#include "c74_msp.h"

using namespace c74::max;

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.phasorshift~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for interpolation flag */
#define INTERP_OFF			0
#define INTERP_ON			1

#define OUTLET_MAX		64					// maximum number of outlets specifiable
#define OUTLET_MIN		2					// minimum number of outlets specifiable
#define VEC_SIZE		OUTLET_MAX + 4		// size of vector array passed to perform method

static t_class *phasorshift_class;		// required global pointer to this class

/* structure definition for this object */
typedef struct _phasorShift
{
	t_pxobject 	ps_obj;
	long 		ps_outletcount;
	float 		ps_currIndex[OUTLET_MAX];
	float 		ps_freq;
	float		ps_stepsize;
	short		ps_inlet_connected;
    double      ps_samp_rate;
	
} t_phasorShift;	

/* method definitions for this object */
void phasorShift_setIndexArray(t_phasorShift *x);
void *phasorShift_new(long outlets);
void phasorShift_dsp64(t_phasorShift *x, t_object *dsp64, short *count, double samplerate,
                       long maxvectorsize, long flags);
void phasorShift_perform64(t_phasorShift *x, t_object *dsp64, double **ins, long numins, double **outs,
                            long numouts, long vectorsize, long flags, void *userparam);
void phasorShift_float(t_phasorShift *x, double f);
void phasorShift_int(t_phasorShift *x, long l);
void phasorShift_assist(t_phasorShift *x, t_object *b, long msg, long arg, char *s);
void phasorShift_getinfo(t_phasorShift *x);
float allpassInterp(float *in_array, float index, float last_out, long buf_length);

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
    
    c = class_new(OBJECT_NAME, (method)phasorShift_new, (method)dsp_free, (short)sizeof(t_phasorShift), 0L,
				A_DEFLONG, 0);
    class_dspinit(c); // add standard functions to class
	
	/* bind method "phasorShift_float" to the float message */
	class_addmethod(c, (method)phasorShift_float, "float", A_FLOAT, 0);
	
	/* bind method "phasorShift_int" to the int message */
	class_addmethod(c, (method)phasorShift_int, "int", A_LONG, 0);
	
	/* bind method "phasorShift_assist" to the assistance message */
	class_addmethod(c, (method)phasorShift_assist, "assist", A_CANT, 0);
	
	/* bind method "phasorShift_getinfo" to the getinfo message */
	class_addmethod(c, (method)phasorShift_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "phasorShift_dsp64" to the dsp64 message */
    class_addmethod(c, (method)phasorShift_dsp64, "dsp64", A_CANT, 0);
	
    class_register(CLASS_BOX, c); // register the class w max
    phasorshift_class = c;
    
    #ifdef DEBUG
        //object_post((t_object*)x, "%s: main function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    return 0;

}

/********************************************************************************
void phasorShift_setIndexArray(t_phasorShift *x)

inputs:			x		-- pointer to this object
description:	fills array with indexs for phasor output to outlets
returns:		nothing
********************************************************************************/
void phasorShift_setIndexArray(t_phasorShift *x)
{
	float *tab = (float *)(x->ps_currIndex);		// get pointer to ps_table
	float num_out = (float)(x->ps_outletcount);		// local var for number of outlets
	long n = OUTLET_MAX;				// set counter equal to ps_outletcount
	
	n += 1;
	while (--n) {	// fill indexs with zero first, to be safe
		tab[n] = 0.0;
	}
	
	n = x->ps_outletcount;			// set counter equal to ps_outletcount
	while (--n >= 0) {
		/* fill ps_table with pointer values */
		tab[n] = (float)n / num_out;
	}
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: pointers set", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void *phasorShift_new(double initial_pos)

inputs:			initial_pos		-- object argument 1; initial position of pan
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; fills ps_table; sets pointers
returns:		nothing
********************************************************************************/
void *phasorShift_new(long outlets)
{
	long i;
	
	t_phasorShift *x = (t_phasorShift *) object_alloc((t_class*) phasorshift_class);
	
	// set outlets within limits
	x->ps_outletcount = 
		outlets>OUTLET_MAX?OUTLET_MAX:outlets<OUTLET_MIN?OUTLET_MIN:outlets;
	
	dsp_setup((t_pxobject *)x, 1);					// one inlet
	for (i = 0; i < x->ps_outletcount; i++) {
		outlet_new((t_pxobject *)x, "signal");		// create outlets
	}
	
	phasorShift_setIndexArray(x);					// set the indexs
	
	x->ps_freq = 20.0;								// default freq to 20.0
	
	x->ps_obj.z_misc = Z_NO_INPLACE;
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
	
	/* return a pointer to the new object */
	return (x);
}


/********************************************************************************
 void phasorShift_dsp64()
 
 inputs:	x		-- pointer to this object
            dsp64		-- signal chain to which object belongs
            count	-- array detailing number of signals attached to each inlet
            samplerate -- number of samples per second
            maxvectorsize -- sample frames per vector of audio
            flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void phasorShift_dsp64(t_phasorShift *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    // check if inlets are connected at audio rate
    x->ps_inlet_connected = count[0];
    
    // save other info to object vars
    x->ps_samp_rate = samplerate;
    
    // add the perform routine to the signal chain
    dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)phasorShift_perform64, 0, NULL);
    
}


/********************************************************************************
 void *phasorShift_perform64(t_phasorShift *x, t_object *dsp64, double **ins, long numins, double **outs,
 long numouts, long vectorsize, long flags, void *userparam)
 
 inputs:			x		--
                    dsp64   --
                    ins     --
                    numins  --
                    outs    --
                    numouts --
                    vectorsize --
                    flags   --
                    userparam  --
 description:	called at interrupt level to compute object's output at 64-bit
 returns:		nothing
 ********************************************************************************/
void phasorShift_perform64(t_phasorShift *x, t_object *dsp64, double **ins, long numins, double **outs,
                            long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars for outlets, interval, width, step size and index
    double *curr_out[OUTLET_MAX];
    double curr_freq = x->ps_inlet_connected ?  *ins[0] : x->ps_freq;
    double curr_step_size;
    float *currIndex = x->ps_currIndex; // TODO: upgrade to double later
    
    // local vars used for while loop
    double temp;
    long n, m;
    
    // fill local pointer array for outlets
    m = numouts;
    while(m--)
    {
        curr_out[m] = outs[m];
    }
    
    // check constraints
    // NOT NECESSARY
    
    // then compute step size
    curr_step_size = curr_freq / x->ps_samp_rate;
    
    // update object variables
    x->ps_freq = curr_freq;
    x->ps_stepsize = curr_step_size;
    
    n = vectorsize;
    while (n--) {
        m = numouts;
        while (m--) {
            temp = (double)(currIndex[m]);
            
            // check bounds //
            while (temp < 0.0)
                temp += 1.0;
            while (temp >= 1.0)
                temp -= 1.0;
            
            *(curr_out[m]) = temp;		// save to output
            
            temp += curr_step_size;		// advance index
            currIndex[m] = (float)temp;	// save next index
            (curr_out[m])++;			// advance the outlet pointer
        }
    }
}


/********************************************************************************
void phasorShift_float(t_phasorShift *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; left inlet sets "ps_freq" variable;
		other inlets generate error message in max window
returns:		nothing
********************************************************************************/
void phasorShift_float(t_phasorShift *x, double f)
{
	if (x->ps_obj.z_in == 0) // if right inlet
	{
		x->ps_freq = f;				// save frequency input
	}
	else
	{
		object_post((t_object*)x, "%s: that inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void phasorShift_int(t_phasorShift *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles ints sent to inlets; left inlet sets "ps_freq" variable;
		other inlets generate error message in max window
returns:		nothing
********************************************************************************/
void phasorShift_int(t_phasorShift *x, long l)
{
	if (x->ps_obj.z_in == 0) // if right inlet
	{
		x->ps_freq = (double) l;			// save frequency input
	}
	else
	{
		object_post((t_object*)x, "%s: that inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void phasorShift_assist(t_phasorShift *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void phasorShift_assist(t_phasorShift *x, t_object *b, long msg, long arg, char *s)
{
	char out_mess[30];
	short which_outlet;
	short num_out = x->ps_outletcount;
	
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal/float) phasor frequency");
				break;
		}
	} else if (msg==ASSIST_OUTLET) {
		which_outlet = arg + 1;
		sprintf(out_mess, "(signal) phasor output %hd of %hd", which_outlet, num_out);
		strcpy(s, out_mess);
	}
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void phasorShift_getinfo(t_phasorShift *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void phasorShift_getinfo(t_phasorShift *x)
{
	object_post((t_object*)x, "%s object by Nathan Wolek", OBJECT_NAME);
	object_post((t_object*)x, "Last updated on %s - www.nathanwolek.com", __DATE__);
}


