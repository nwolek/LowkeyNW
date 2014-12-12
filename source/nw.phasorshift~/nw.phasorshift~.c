/*
** phasor.shift~.c
**
** MSP object
** generates evenly shifted phase signals
** 
** 2001/08/09 started by Nathan Wolek
** 2002/08/23 removed old resource assist strings
** 2002/09/24 added getinfo message
** 2002/11/19 removed old table code, made Carbon compatible
** 2006/11/24 moved to Xcode
** 
*/


#include "ext.h"		// required for all MAX external objects
#include "z_dsp.h"		// required for all MSP external objects
#include <string.h>

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"phasor.shift~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for interpolation flag */
#define INTERP_OFF			0
#define INTERP_ON			1

#define OUTLET_MAX		64					// maximum number of outlets specifiable
#define OUTLET_MIN		2					// minimum number of outlets specifiable
#define VEC_SIZE		OUTLET_MAX + 4		// size of vector array passed to perform method

void *this_class;		// required global pointer to this class

/* structure definition for this object */
typedef struct _phasorShift
{
	t_pxobject 	ps_obj;
	long 		ps_outletcount;
	float 		ps_currIndex[OUTLET_MAX];
	float 		ps_freq;
	float		ps_stepsize;
	short		ps_inlet_connected;
	
} t_phasorShift;	

/* method definitions for this object */
void phasorShift_setIndexArray(t_phasorShift *x);
void *phasorShift_new(long outlets);
void phasorShift_dsp(t_phasorShift *x, t_signal **sp, short *count);
t_int *phasorShift_perform(t_int *w);
void phasorShift_float(t_phasorShift *x, double f);
void phasorShift_int(t_phasorShift *x, long l);
void phasorShift_assist(t_phasorShift *x, t_object *b, long msg, long arg, char *s);
void phasorShift_getinfo(t_phasorShift *x);
/* method definitions for debugging this object */
#ifdef DEBUG
	void phasorShift_index(t_phasorShift *x, long value);
#endif /* DEBUG */
float allpassInterp(float *in_array, float index, float last_out, long buf_length);

/********************************************************************************
void main(void)

inputs:			nothing
description:	called the first time the object is used in MAX environment; 
		defines inlets, outlets and accepted messages
returns:		nothing
********************************************************************************/
void main(void)
{
	setup(&this_class, phasorShift_new, (method)dsp_free, (short)sizeof(t_phasorShift), 0L, 
				A_DEFLONG, 0);
	addmess((method)phasorShift_dsp, "dsp", A_CANT, 0);
	
	#ifdef DEBUG
		addmess((method)phasorShift_index, "index", A_DEFLONG, 0);
	#endif /* DEBUG */
	
	/* bind method "phasorShift_float" to the float message */
	addfloat((method)phasorShift_float);
	
	/* bind method "phasorShift_int" to the int message */
	addfloat((method)phasorShift_int);
	
	/* bind method "phasorShift_assist" to the assistance message */
	addmess((method)phasorShift_assist, "assist", A_CANT, 0);
	
	/* bind method "phasorShift_getinfo" to the getinfo message */
	addmess((method)phasorShift_getinfo, "getinfo", A_NOTHING, 0);
	
	dsp_initclass();
	
	#ifndef DEBUG
		
	#endif /* DEBUG */
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
		post("%s: pointers set", OBJECT_NAME);
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
	
	t_phasorShift *x = (t_phasorShift *)newobject(this_class);
	
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
	
	/* return a pointer to the new object */
	return (x);
}

/********************************************************************************
void phasorShift_dsp(t_phasorShift *x, t_signal **sp, short *count)

inputs:			x		-- pointer to this object
				sp		-- array of pointers to input & output signals
				count	-- array of shorts detailing number of signals attached
					to each inlet
description:	called when DSP call chain is built; adds object to signal flow
returns:		nothing
********************************************************************************/
void phasorShift_dsp(t_phasorShift *x, t_signal **sp, short *count)
{
	long i;
	
	void *v[VEC_SIZE];
	
	x->ps_inlet_connected = count[0];
	
	v[0] = x;
	v[1] = &(sp[1]->s_n);
	v[2] = &(sp[1]->s_sr);
	v[3] = sp[0]->s_vec;
	
	for (i = 0; i < x->ps_outletcount; i++) {
		v[i+4] = (float *)(sp[i+1]->s_vec);	// get pointers to outputs
	}
	
	dsp_addv(phasorShift_perform, VEC_SIZE, v);
	#ifdef DEBUG
		post("%s: ps_freq is being updated at audio rate", OBJECT_NAME);
		post("%s: output sampling rate is %f", OBJECT_NAME, sp[1]->s_sr);
	#endif /* DEBUG */

}

/********************************************************************************
t_int *phasorShift_perform(t_int *w)

inputs:			w		-- array of signal vectors specified in "phasorShift_dsp"
description:	called at interrupt level to compute object's output
returns:		pointer to the next 
********************************************************************************/
t_int *phasorShift_perform(t_int *w)
{
	t_phasorShift *x = (t_phasorShift *)(w[1]);	// create local pointer to this object
	int vector_size = *(int *)(w[2]);			// create lacal var for vector size
	float samp_rate = *(float *)(w[3]);			// create lacal var for sampling rate
	float curr_freq, step_size;					// vars for freq and stepsize
	
	const long outlet_count = x->ps_outletcount;// local var for number of outlets
	float *outs[OUTLET_MAX];
	float *currIndex;
	long n, i;									// count for updating outlets
	float temp;									// temp var for computations
	
	if (x->ps_obj.z_disabled)
		goto out;
	
	if (x->ps_inlet_connected) {
		x->ps_freq = *(float *)(w[4]);	// grab signal rate frequency input
		curr_freq = x->ps_freq;
		x->ps_stepsize = curr_freq / samp_rate;
									// compute ps_table samples per output sample
		step_size = x->ps_stepsize;	// set local var equal to global
	} else {
		curr_freq = x->ps_freq;
		x->ps_stepsize = curr_freq / samp_rate;
									// compute ps_table samples per output sample
		step_size = x->ps_stepsize;	// set local var equal to global
	}
	
	currIndex = x->ps_currIndex;	// set local var equal to global
	
	for (i = 0; i < outlet_count; i++) {	// grab pointers to the outlets
		outs[i] = (float *)(w[i+5]);
	}
	
	vector_size += 1;
	while (--vector_size)		// compute for each sample in vector
	{
		n = outlet_count;
		while (--n >= 0) {
			temp = currIndex[n];	// get ps_table position
			
			// check bounds //
			while (temp < 0.0)
				temp += 1.0;
			while (temp >= 1.0)
				temp -= 1.0;
				
			*(outs[n]) = temp;		// save to output
			
			temp += step_size;		// advance index
			currIndex[n] = temp;	// save next index
			++(outs[n]);			// advance current vector pointer
		}
	}
out:
	return(w + VEC_SIZE + 1);		// pointer to next vector
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
		post("%s: that inlet does not accept floats", OBJECT_NAME);
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
		post("%s: that inlet does not accept floats", OBJECT_NAME);
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
		sprintf(out_mess, "(signal) phasor output %ld of %ld", which_outlet, num_out);
		strcpy(s, out_mess);
	}
	
	#ifdef DEBUG
		post("%s: assist message displayed", OBJECT_NAME);
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
	post("%s object by Nathan Wolek", OBJECT_NAME);
	post("Last updated on %s - www.nathanwolek.com", __DATE__);
}

/* the following methods are only compiled into the code during debugging*/
#ifdef DEBUG	
/********************************************************************************
void phasorShift_index(t_phasorShift *x, long value)

inputs:			x		-- pointer to this object
				value	-- argument from "ps_table" message; sets table index
description:	inquire the array values for a specified outlet index
returns:		nothing
********************************************************************************/
	void phasorShift_index(t_phasorShift *x, long value)
	{
		if (value >= 0 && value <= (TABLE_SIZE - 1)) 
		{
			post("%s: the index for outlet %ld is %f", 
						OBJECT_NAME, value+1, x->ps_currIndex[value]);
		}
		else
		{
			post("%s: value %ld is out of array bounds", OBJECT_NAME, value);
		}
	}
#endif /* DEBUG */
