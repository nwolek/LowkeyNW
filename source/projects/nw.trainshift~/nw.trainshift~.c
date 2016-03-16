/*
** nw.trainshift~.c
**
** MSP object
** generates evenly shifted train signals
** 2001/08/29 started by Nathan Wolek
** 
** Copyright © 2001,2014 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
** 
*/


#include "ext.h"		// required for all MAX external objects
#include "ext_obex.h"   // required for new style MAX objects
#include "z_dsp.h"		// required for all MSP external objects
#include <string.h>

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.trainshift~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

#define OUTLET_MAX		64					// maximum number of outlets specifiable
#define OUTLET_MIN		2					// minimum number of outlets specifiable
#define VEC_SIZE		OUTLET_MAX + 5		// size of vector array passed to perform method

static t_class *trainshift_class;		// required global pointer to this class

/* structure definition for this object */
typedef struct _trainShift
{
	t_pxobject 	ts_obj;
	long 		ts_outletcount;
	float 		ts_currIndex[OUTLET_MAX];
	float 		ts_interval_ms;
	float		ts_width_ratio;
	float		ts_step_size;
	short		ts_interval_connected;
	short		ts_width_connected;
	float		ts_shortest_pulse;
    double      ts_samp_rate;
	
} t_trainShift;	

/* method definitions for this object */
void trainShift_setIndexArray(t_trainShift *x);
void *trainShift_new(long outlets);
void trainShift_dsp(t_trainShift *x, t_signal **sp, short *count);
void trainShift_dsp64(t_trainShift *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags);
t_int *trainShift_perform(t_int *w);
void trainShift_perform64(t_trainShift *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void trainShift_float(t_trainShift *x, double f);
void trainShift_int(t_trainShift *x, long l);
void trainShift_assist(t_trainShift *x, t_object *b, long msg, long arg, char *s);
void trainShift_getinfo(t_trainShift *x);


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
    
    c = class_new(OBJECT_NAME, (method)trainShift_new, (method)dsp_free, (short)sizeof(t_trainShift), 0L,
				A_DEFLONG, 0);
    class_dspinit(c); // add standard functions to class
    
	class_addmethod(c, (method)trainShift_dsp, "dsp", A_CANT, 0);
	
	/* bind method "trainShift_float" to the float message */
	class_addmethod(c, (method)trainShift_float, "float", A_FLOAT, 0);
	
	/* bind method "trainShift_int" to the int message */
	class_addmethod(c, (method)trainShift_int, "int", A_LONG, 0);
	
	/* bind method "trainShift_assist" to the assistance message */
	class_addmethod(c, (method)trainShift_assist, "assist", A_CANT, 0);
	
	/* bind method "trainShift_getinfo" to the getinfo message */
	class_addmethod(c, (method)trainShift_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "trainShift_dsp64" to the dsp64 message */
    class_addmethod(c, (method)trainShift_dsp64, "dsp64", A_CANT, 0);
	
    class_register(CLASS_BOX, c); // register the class w max
    trainshift_class = c;
	
	#ifdef DEBUG
		post("%s: main function was called", OBJECT_NAME);
	#endif /* DEBUG */
    
    return 0;
}

/********************************************************************************
void trainShift_setIndexArray(t_trainShift *x)

inputs:			x		-- pointer to this object
description:	fills array with indexs for train output to outlets
returns:		nothing
********************************************************************************/
void trainShift_setIndexArray(t_trainShift *x)
{
	float *tab = (float *)(x->ts_currIndex);		// get pointer to ps_table
	float num_out = (float)(x->ts_outletcount);		// local var for number of outlets
	long n = OUTLET_MAX;				// set counter equal to ts_outletcount
	
	n += 1;
	while (--n) {	// fill indexs with zero first, to be safe
		tab[n] = 0.0;
	}
	
	n = x->ts_outletcount;			// set counter equal to ts_outletcount
	while (--n >= 0) {
		/* fill ps_table with pointer values */
		tab[n] = ((float)n / num_out) + 1.0;
	}
	
	#ifdef DEBUG
		post("%s: pointers set", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void *trainShift_new(long outlets)

inputs:			initial_pos		-- object argument 1; number of outlets
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; defines default var values
returns:		nothing
********************************************************************************/
void *trainShift_new(long outlets)
{
	long i;
	
	t_trainShift *x = (t_trainShift *) object_alloc((t_class*) trainshift_class);
	
	// set outlets within limits
	x->ts_outletcount = 
		outlets>OUTLET_MAX?OUTLET_MAX:outlets<OUTLET_MIN?OUTLET_MIN:outlets;
	
	dsp_setup((t_pxobject *)x, 2);					// two inlets
	for (i = 0; i < x->ts_outletcount; i++) {
		outlet_new((t_pxobject *)x, "signal");		// create outlets
	}
	
	trainShift_setIndexArray(x);					// set the indexs
	
	x->ts_interval_ms = 1000.0;						// default interval to 1000.0
	x->ts_width_ratio = 0.5;						// default width to 0.5
	x->ts_shortest_pulse = 2000.0 / sys_getsr();
	
	x->ts_obj.z_misc = Z_NO_INPLACE;
    
    #ifdef DEBUG
        post("%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
	
	/* return a pointer to the new object */
	return (x);
}

/********************************************************************************
void trainShift_dsp(t_trainShift *x, t_signal **sp, short *count)

inputs:			x		-- pointer to this object
				sp		-- array of pointers to input & output signals
				count	-- array of shorts detailing number of signals attached
					to each inlet
description:	called when DSP call chain is built; adds object to signal flow
returns:		nothing
********************************************************************************/
void trainShift_dsp(t_trainShift *x, t_signal **sp, short *count)
{
    
    #ifdef DEBUG
        post("%s: adding 32 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    long i;
	
	void *v[VEC_SIZE];
	
	// check if inlets are connected at audio rate
	x->ts_interval_connected = count[0];
	x->ts_width_connected = count[1];
	
    x->ts_samp_rate = sp[2]->s_sr;
	x->ts_shortest_pulse = 2000.0 / x->ts_samp_rate;
	
    // TODO: not sure this is needed, since constraints happen in float and perform methods
	if (x->ts_interval_ms < x->ts_shortest_pulse)
		x->ts_interval_ms = x->ts_shortest_pulse;
	
	v[0] = x;
	v[1] = &(sp[2]->s_n);
	v[2] = &(sp[2]->s_sr);
	v[3] = sp[0]->s_vec;
	v[4] = sp[1]->s_vec;
	
	for (i = 0; i < x->ts_outletcount; i++) {
		v[i+5] = (float *)(sp[i+2]->s_vec);	// get pointers to outputs
	}
	
	dsp_addv(trainShift_perform, VEC_SIZE, v);
	#ifdef DEBUG
		post("%s: output sampling rate is %f", OBJECT_NAME, sp[2]->s_sr);
	#endif /* DEBUG */

}

/********************************************************************************
 void trainShift_dsp64()
 
 inputs:			x		-- pointer to this object
 dsp64		-- signal chain to which object belongs
 count	-- array detailing number of signals attached to each inlet
 samplerate -- number of samples per second
 maxvectorsize -- sample frames per vector of audio
 flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void trainShift_dsp64(t_trainShift *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        post("%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    // check if inlets are connected at audio rate
    x->ts_interval_connected = count[0];
    x->ts_width_connected = count[1];
    
    // save other info to object vars
    x->ts_samp_rate = samplerate;
    x->ts_shortest_pulse = 2000.0 / x->ts_samp_rate;
    
    // add the perform routine to the signal chain
    dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)trainShift_perform64, 0, NULL);

}

/********************************************************************************
t_int *trainShift_perform(t_int *w)

inputs:			w		-- array of signal vectors specified in "trainShift_dsp"
description:	called at interrupt level to compute object's output
returns:		pointer to the next 
********************************************************************************/
t_int *trainShift_perform(t_int *w)
{
	t_trainShift *x = (t_trainShift *)(w[1]);	// create local pointer to this object
	int vector_size = *(int *)(w[2]);			// create lacal var for vector size
	float samp_rate = *(float *)(w[3]);			// create lacal var for sampling rate
	float curr_length, curr_width;				// local vars for interval and width
	float curr_step_size;						// step size for each sample
	
	const long outlet_count = x->ts_outletcount;// local var for number of outlets
	float *outs[OUTLET_MAX];
	float *currIndex;
	long n, i;									// count for updating outlets
	float temp;									// temp var for computations
	
	if (x->ts_obj.z_disabled)
		goto out;
	
	if (x->ts_interval_connected) {		// if audio rate connection...
		temp = *(float *)(w[4]);		// grab audio rate interval input
		if (temp > x->ts_shortest_pulse) {	// check restrictions...
			x->ts_interval_ms = temp;	// set global
		} else {
			x->ts_interval_ms = x->ts_shortest_pulse;
		}
	}
	curr_length = x->ts_interval_ms;
	x->ts_step_size = 1000.0 / (curr_length * samp_rate);
	curr_step_size = x->ts_step_size;
	
	if (x->ts_width_connected) {			// if audio rate connection...
		temp = *(float *)(w[5]);			// grab audio rate width input
		if (temp >= 0.0 && temp <= 1.0) {	// check restrictions...
			x->ts_width_ratio = temp;		// set global
		}
	}
	curr_width = x->ts_width_ratio;			// grab control rate width input
	
	currIndex = x->ts_currIndex;	// set local var equal to global
	
	for (i = 0; i < outlet_count; i++) {	// grab pointers to the outlets
		outs[i] = (float *)(w[i+6]);
	}
	
	vector_size += 1;
	while (--vector_size)		// compute for each sample in vector
	{
		n = outlet_count;
		while (--n >= 0) {
			temp = currIndex[n];	// get ts_table position
			
			// check bounds //
			while (temp < 0.0)
				temp += 1.0;
				
			if (temp <= curr_width) {
				*(outs[n]) = 1.0;		// save to output
			} else {
				*(outs[n]) = 0.0;		// save to output
			}
			
			temp -= curr_step_size;		// advance index
			currIndex[n] = temp;	// save next index
			++(outs[n]);			// advance current vector pointer
		}
	}
out:
	return(w + VEC_SIZE + 1);		// pointer to next vector
}

/********************************************************************************
 void *trainShift_perform64(t_trainShift *x, t_object *dsp64, double **ins, long numins, double **outs,
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
void trainShift_perform64(t_trainShift *x, t_object *dsp64, double **ins, long numins, double **outs,
                            long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars for outlets, interval, width, step size and index
    t_double *curr_out[OUTLET_MAX];
    t_double curr_length = x->ts_interval_connected ? *ins[0] : x->ts_interval_ms;
    t_double curr_width = x->ts_width_connected ? *ins[1] : x->ts_width_ratio;
    double curr_step_size;
    float *currIndex = x->ts_currIndex; // TODO: upgrade to double later
    
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
    if (curr_length < x->ts_shortest_pulse) curr_length = x->ts_shortest_pulse;
    if (curr_width < 0.) curr_width = 1 / x->ts_samp_rate;
    if (curr_width > 1.) curr_width = 1.;
    
    // then compute step size
    curr_step_size = 1000.0 / (curr_length * x->ts_samp_rate);
    
    // update object variables
    x->ts_interval_ms = curr_length;
    x->ts_step_size = curr_step_size;
    
    
    n = vectorsize;
    while(n--)
    {
        m = numouts;
        while(m--)
        {
            
            temp = (double)(currIndex[m]);
            
            // check bounds //
            while (temp < 0.0)
                temp += 1.0;
            
            if (temp <= curr_width) {
                *(curr_out[m]) = 1.0;		// save to output
            } else {
                *(curr_out[m]) = 0.0;		// save to output
            }
            
            temp -= curr_step_size;		// advance index
            currIndex[m] = (float)temp;	// save next index
            (curr_out[m])++;			// advance the outlet pointer
            
        }
        
    }
    
    
}

/********************************************************************************
void trainShift_float(t_trainShift *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; first inlet sets "ts_interval_ms";
		second inlet sets "ts_width_ratio"
returns:		nothing
********************************************************************************/
void trainShift_float(t_trainShift *x, double f)
{
	if (x->ts_obj.z_in == 0) // if first inlet
	{
		if (f > x->ts_shortest_pulse) { // if greater than two samples...
			x->ts_interval_ms = f;				// save interval input
		} else { // if not...
			post("%s: pulse interval must be greater than %f", OBJECT_NAME, 
				x->ts_shortest_pulse);
			x->ts_interval_ms = x->ts_shortest_pulse;
		}
	}
	else if (x->ts_obj.z_in == 1) // if second inlet
	{
		if (f >= 0.0 && f <= 1.0) { // if within 0 and 1 bounds...
			x->ts_width_ratio = f;				// save width input
		} else { // if not..
			post("%s: pulse width must be between 0 and 1", OBJECT_NAME);
		}
	}
	else
	{
		post("%s: that inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void trainShift_int(t_trainShift *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles ints sent to inlets; first inlet sets "ts_interval_ms";
		second inlet sets "ts_width_ratio"
returns:		nothing
********************************************************************************/
void trainShift_int(t_trainShift *x, long l)
{
	if (x->ts_obj.z_in == 0) // if first inlet
	{
		if (l > x->ts_shortest_pulse) { // if greater than two samples...
			x->ts_interval_ms = (double) l;			// save interval input
		} else { // if not...
			post("%s: pulse interval must be greater than %f", OBJECT_NAME, 
				x->ts_shortest_pulse);
			x->ts_interval_ms = x->ts_shortest_pulse;
		}
	}
	else if (x->ts_obj.z_in == 1) // if second inlet
	{
		post("%s: pulse width must be a float between 0 and 1", OBJECT_NAME);
	}
	else
	{
		post("%s: that inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void trainShift_assist(t_trainShift *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void trainShift_assist(t_trainShift *x, t_object *b, long msg, long arg, char *s)
{
	char out_mess[30];
	short which_outlet;
	short num_out = x->ts_outletcount;
	
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal/float) pulse interval in ms");
				break;
			case 1:
				strcpy(s, "(signal/float) pulse width from 0 to 1");
				break;
		}
	} else if (msg==ASSIST_OUTLET) {
		which_outlet = arg + 1;
		sprintf(out_mess, "(signal) pulse output %hd of %hd", which_outlet, num_out);
		strcpy(s, out_mess);
	}
	
	#ifdef DEBUG
		post("%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void trainShift_getinfo(t_trainShift *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void trainShift_getinfo(t_trainShift *x)
{
	post("%s object by Nathan Wolek", OBJECT_NAME);
	post("Last updated on %s - www.nathanwolek.com", __DATE__);
}
