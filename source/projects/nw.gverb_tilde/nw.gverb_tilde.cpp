/*
** nw.gverb~.c
**
** MSP object
** msp implementation of Griesinger vst plugin
** 2002/05/20 started by Nathan Wolek
**
** Copyright © 2002,2014 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
** 
*/

#include "c74_msp.h"
#include "reverb_bb.h"

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.gverb~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

// constant settings for the reverb algorithm
#define MODRATE_MAX 1.0f			// max modulation rate
#define EXCUR_MAX 20				// max depth of mod delay
#define ALLPASS_SHORT_DELAY_VALUES {142, 107, 379, 277}	// delay settings
#define ALLPASS_LONG_DELAY_VALUES {1800, 2656}	// delay settings
#define ALLPASS_MOD_DELAY_INIT_VALUES {672, 908}	// initial delay settings
#define DELAY_SMALL_VALUES {4453, 4217, 3720, 3163}	// short delay values

#define LOWPASS_NUM 3			// number of lowpass filters
#define ALLPASS_SHORT_NUM 4		// number of short buffered allpass filters
#define ALLPASS_LONG_NUM 2		// number of long buffered allpass filters
#define ALLPASS_MOD_NUM 2		// number of modulating buffered allpass filters
#define DELAYBUFF_SMALL_NUM 4	// number of short delay lines

// coefficients for reverb components
#define IN_DIFF_1		0.750
#define IN_DIFF_2		0.625
#define DEC_DIFF_1		0.700
#define DEC_DIFF_2		0.500
#define DAMPING			0.0005
#define BANDWIDTH		0.9995
#define AP_MODRATE_1	1.13671
#define AP_MODRATE_2	1.11718
#define AP_MODDEPTH_1	16.11
#define AP_MODDEPTH_2	15.87

// fix for denormal through square injection of dc offset
#define TINY_DC		0.0000000000000000000000001f

static t_class *gverb_class;		// required global pointer to this class

/* structure definition for this object */
typedef struct _gverb
{
	t_pxobject x_obj;
	
	// arrays to hold exact buffer lengths
	long apShort_values[ALLPASS_SHORT_NUM];
	long apLong_values[ALLPASS_LONG_NUM];
	long smallDelay_values[DELAYBUFF_SMALL_NUM];
	long apMod_init_values[ALLPASS_MOD_NUM];
	
	// structs for reverb building blocks
	rbb_sintable oscTable;
	rbb_lowpass lpFilters[LOWPASS_NUM];
	rbb_allpass_short apFilters_short[ALLPASS_SHORT_NUM];
	rbb_allpass_long apFilters_long[ALLPASS_LONG_NUM];
	rbb_allpass_mod apFilters_mod[ALLPASS_MOD_NUM];
	rbb_delaybuff_short delayBuffs_small[DELAYBUFF_SMALL_NUM];
	
	double verb_decay;					// in milliseconds
	double verb_decay_1over;			// in milliseconds
	double verb_decay_coeff;
	
	double lastout_L;					// last output
	double lastout_R;
	
	// inlet connections
	short verb_decay_connected;
	
	// sample rate info
	double output_sr;
	double output_msr;
	double output_1overmsr;
	
	// maintain dc_offset for square injection
	double sqinject_val;
	
} t_gverb;

/* method definitions for this object */
void *gverb_new(double d);
void gverb_dsp64(t_gverb *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags);
void gverb_perform64(t_gverb *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void gverb_float(t_gverb *x, double f);
void gverb_int(t_gverb *x, long l);
void gverb_assist(t_gverb *x, t_object *b, long msg, long arg, char *s);
void gverb_getinfo(t_gverb *x);
void gverb_init(t_gverb *x);
void gverb_free(t_gverb *x);
/* method definitions for debugging this object */
#ifdef DEBUG
	
#endif /* DEBUG */

/********************************************************************************
int main(void)

inputs:			nothing
description:	called the first time the object is used in MAX environment; 
		defines inlets, outlets and accepted messages
returns:		int
********************************************************************************/
int C74_EXPORT main(void)
{
    t_class *c;
    
    c = class_new(OBJECT_NAME, (method)gverb_new, (method)gverb_free,
			(short)sizeof(t_gverb), 0L, A_DEFFLOAT, 0);
    class_dspinit(c); // add standard functions to class
    	
	/* bind method "gverb_float" to incoming floats */
	class_addmethod(c, (method)gverb_float, "float", A_FLOAT, 0);
	
	/* bind method "gverb_int" to incoming ints */
	class_addmethod(c, (method)gverb_int, "int", A_LONG, 0);
	
	/* bind method "gverb_assist" to the assistance message */
	class_addmethod(c, (method)gverb_assist, "assist", A_CANT, 0);
	
	/* bind method "gverb_getinfo" to the getinfo message */
	class_addmethod(c, (method)gverb_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "gverb_dsp64" to the dsp64 message */
    class_addmethod(c, (method)gverb_dsp64, "dsp64", A_CANT, 0);
    
    class_register(C74_CLASS_BOX, c); // register the class w max
    gverb_class = c;
	
    #ifdef DEBUG
        object_post((t_object*)x, "%s: main function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    return 0;
}

/********************************************************************************
void *gverb_new(double d)

inputs:			void
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; 
returns:		nothing
********************************************************************************/
void *gverb_new(double d)
{
	t_gverb *x = (t_gverb *)object_alloc((t_class*) gverb_class);
	
	dsp_setup((t_pxobject *)x, 2);					// two inlets
	outlet_new((t_pxobject *)x, "signal");			// left outlet
	outlet_new((t_pxobject *)x, "signal");			// right outlet
	
	/* setup variables */
	x->verb_decay = d > 0.0 ? d : 1000.0;
	x->verb_decay_1over = d > 0.0 ? 1.0 / d : 0.001;
	
	// get sample rate info
	x->output_sr = sys_getsr();
	x->output_msr = x->output_sr * 0.001;
	x->output_1overmsr = 1.0 / x->output_msr;
	
	// initialize square injection value
	x->sqinject_val = TINY_DC;
	
	x->verb_decay_coeff = 
			pow(10.0, (-16416.0 * x->verb_decay_1over * x->output_1overmsr));
	
	gverb_init(x);
	
	x->x_obj.z_misc = Z_NO_INPLACE;
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
	
	/* return a pointer to the new object */
	return (x);
}


/********************************************************************************
 void gverb_dsp64()
 
 inputs:			x		-- pointer to this object
 dsp64		-- signal chain to which object belongs
 count	-- array detailing number of signals attached to each inlet
 samplerate -- number of samples per second
 maxvectorsize -- sample frames per vector of audio
 flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void gverb_dsp64(t_gverb *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    // check inlet connection
    x->verb_decay_connected = count[1];
    
    // get sample rate info
    x->output_sr = samplerate;
    x->output_msr = x->output_sr * 0.001;
    x->output_1overmsr = 1.0 / x->output_msr;
    
    x->verb_decay_coeff =
    pow(10.0, (-16416.0 * x->verb_decay_1over * x->output_1overmsr));
    
    // update allpass mod with sampling rate
    rbb_set_allpassMod_freq(x->apFilters_mod, AP_MODRATE_1, x->output_sr);
    rbb_set_allpassMod_freq(((x->apFilters_mod) + 1), AP_MODRATE_2, x->output_sr);
    
    dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)gverb_perform64, 0, NULL);
    
}


/********************************************************************************
 void *gverb_perform64(t_gverb *x, t_object *dsp64, double **ins, long numins, double **outs,
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
void gverb_perform64(t_gverb *x, t_object *dsp64, double **ins, long numins, double **outs,
                          long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars for inlets/outlets
    double *in_dry = ins[0];
    double *in_decay = ins[1];
    double *out_wet1 = outs[0];
    double *out_wet2 = outs[1];
    
    // local vars for object vars
    double fDecay = x->verb_decay_coeff;
    double lastout_L = x->lastout_L;
    double lastout_R = x->lastout_R;
    double sqinject_val = x->sqinject_val * -1.0; // flip sign each time
    
    // local vars used for while loop, TODO: upgrade to doubles
    double val_dry, val_decay, val_wet1, val_wet2;
    float val_dry_float, x2, x3, x4, x5, x6;
    float x7L, x8L, x9L, x10L, x11L, x12L, x13L, x14L;
    float x7R, x8R, x9R, x10R, x11R, x12R, x13R, x14R;
    long n;
    
    // check constraints
    
    n = vectorsize;
    while(n--)
    {
        val_dry = *in_dry;				// grab input values
        val_dry += sqinject_val;//TINY_DC;		// add small dc offset to protect against denormal
        val_dry_float = (float)val_dry; // TODO: needed before double upgrade
        val_decay = *in_decay;
        
        val_wet1 = val_wet2 = 0.0;		// zero output before each cycle
        
        // zero computation points before each cycle
        x2 = x3 = x4 = x5 = x6 = 0.0;
        x7L = x8L = x9L = x10L = x11L = x12L = x13L = x14L = 0.0;
        x7R = x8R = x9R = x10R = x11R = x12R = x13R = x14R = 0.0;
        
        if (x->verb_decay_connected)	// if decay inlet has signal input..
        {	// recompute decay coeff each sample
            fDecay = pow(10.0, (-16416.0 * x->output_1overmsr / val_decay));
        }
        
        /***** begin processing of samples here *****/
        
        // lowpass 0
        rbb_compute_lowPass1(&val_dry_float, x->lpFilters, &x2);
        // allpass_short 0
        rbb_compute_allpassShort(&x2, x->apFilters_short, &x3);
        // allpass_short 1
        rbb_compute_allpassShort(&x3, x->apFilters_short + 1, &x4);
        // allpass_short 2
        rbb_compute_allpassShort(&x4, x->apFilters_short + 2, &x5);
        // allpass_short 3
        rbb_compute_allpassShort(&x5, x->apFilters_short + 3, &x6);
        
        /* split*/
        
        // add recursion
        x7L = x6 + lastout_R;
        x7R = x6 + lastout_L;
        // allpass_mod 0 & 1
        rbb_compute_allpassMod(&x7L, x->apFilters_mod, &x8L);
        rbb_compute_allpassMod(&x7R, x->apFilters_mod + 1, &x8R);
        // delaybuff_small 0 & 1
        rbb_compute_shortDelay(&x8L, x->delayBuffs_small, &x9L);
        rbb_compute_shortDelay(&x8R, x->delayBuffs_small + 1, &x9R);
        // lowpass 1 & 2
        rbb_compute_lowPass2(&x9L, x->lpFilters + 1, &x10L);
        rbb_compute_lowPass2(&x9R, x->lpFilters + 2, &x10R);
        // * decay
        x11L = fDecay * x10L;
        x11R = fDecay * x10R;
        // allpass_long 0 & 1
        rbb_compute_allpassLong(&x11L, x->apFilters_long, &x12L);
        rbb_compute_allpassLong(&x11R, x->apFilters_long + 1, &x12R);
        // delaybuff_small 2 & 3
        rbb_compute_shortDelay(&x12L, x->delayBuffs_small + 2, &x13L);
        rbb_compute_shortDelay(&x12R, x->delayBuffs_small + 3, &x13R);
        // * decay
        val_wet1 = fDecay * x13L;
        val_wet2 = fDecay * x13R;
        
        /***** end processing of samples here *****/
        
        lastout_L = val_wet1;
        lastout_R = val_wet2;
        
        *out_wet1 = 1.2 * x9R - 0.6 * x12R + 0.6 * x13R - 0.6 * x9L - 0.6 * x12L - 0.6 * x13L;
        *out_wet2 = 1.2 * x9L - 0.6 * x12L + 0.6 * x13L - 0.6 * x9R - 0.6 * x12R - 0.6 * x13R;
        
        ++in_dry, ++in_decay, ++out_wet1, ++out_wet2;		// advance the pointers
    }
    
    // update object variables
    x->verb_decay_coeff = fDecay;
    x->lastout_L = lastout_L;
    x->lastout_R = lastout_R;
    x->sqinject_val = sqinject_val;
    
}

/********************************************************************************
void gverb_float(t_gverb *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; inlet 2 sets "next_verb_decay_time" 
		variable; 
returns:		nothing
********************************************************************************/
void gverb_float(t_gverb *x, double f)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		if (f > 0) {
			x->verb_decay = f;
			x->verb_decay_1over = 1.0 / x->verb_decay;
			x->verb_decay_coeff = 
				pow(10.0, (-16416.0 * x->verb_decay_1over * x->output_1overmsr));
			#ifdef DEBUG
				object_post((t_object*)x, "%s: decay time is %f", OBJECT_NAME, x->verb_decay);
				object_post((t_object*)x, "%s: decay coeff is %f", OBJECT_NAME, x->verb_decay_coeff);
			#endif /* DEBUG */
		}
		else
		{
			object_error((t_object*)x, "%s: decay time must be greater than zero", OBJECT_NAME);
		}
	}
	else
	{
		object_post((t_object*)x, "%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void gverb_int(t_gverb *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles int sent to inlets; inlet 2 sets "next_verb_decay_time" 
		variable; 
returns:		nothing
********************************************************************************/
void gverb_int(t_gverb *x, long l)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		if (l > 0) {
			x->verb_decay = (double) l;
			x->verb_decay_1over = 1.0 / x->verb_decay;
			x->verb_decay_coeff = 
				pow(10.0, (-16416.0 * x->verb_decay_1over * x->output_1overmsr));
			#ifdef DEBUG
				object_post((t_object*)x, "%s: decay time is %f", OBJECT_NAME, x->verb_decay);
				object_post((t_object*)x, "%s: decay coeff is %f", OBJECT_NAME, x->verb_decay_coeff);
			#endif /* DEBUG */
		}
		else
		{
			object_error((t_object*)x, "%s: decay time must be greater than zero", OBJECT_NAME);
		}
	}
	else
	{
		object_error((t_object*)x, "%s: this inlet does not accept integers", OBJECT_NAME);
	}
}

/********************************************************************************
void gverb_assist(t_gverb *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void gverb_assist(t_gverb *x, t_object *b, long msg, long arg, char *s)
{
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) input");
				break;
			case 1:
				strcpy(s, "(signal/float) reverb decay length in ms");
				break;
		}
	} else if (msg==ASSIST_OUTLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) left channel output");
				break;
			case 1:
				strcpy(s, "(signal) right channel output");
				break;
		}
	}
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void gverb_getinfo(t_gverb *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void gverb_getinfo(t_gverb *x)
{
	object_post((t_object*)x, "%s object by Nathan Wolek", OBJECT_NAME);
	object_post((t_object*)x, "Last updated on %s - www.nathanwolek.com", __DATE__);
}

/********************************************************************************
void gverb_init(t_gverb *x)

inputs:			x		-- pointer to our object
				
description:	initializes building blocks for the reverb algorithm
returns:		nothing
********************************************************************************/
void gverb_init(t_gverb *x)
{
	int curr_num;
	// local arrays for delay values
	long temp_apsv[] = ALLPASS_SHORT_DELAY_VALUES;
	long temp_aplv[] = ALLPASS_LONG_DELAY_VALUES;
	long temp_sdv[] = DELAY_SMALL_VALUES;
	long temp_apmv[] = ALLPASS_MOD_DELAY_INIT_VALUES;
	
	// local pointers to building blocks
	rbb_sintable *ot_ptr = &(x->oscTable);
	rbb_lowpass *lpf_ptr = x->lpFilters;
	rbb_allpass_short *aps_ptr = x->apFilters_short;
	rbb_allpass_long *apl_ptr = x->apFilters_long;
	rbb_allpass_mod *apm_ptr = x->apFilters_mod;
	rbb_delaybuff_short *sd_ptr = x->delayBuffs_small;
	
	// fill arrays with buffer lengths
	curr_num = ALLPASS_SHORT_NUM;
	while (--curr_num >= 0) {
		x->apShort_values[curr_num] = temp_apsv[curr_num];
	}
	
	curr_num = ALLPASS_LONG_NUM;
	while (--curr_num >= 0) {
		x->apLong_values[curr_num] = temp_aplv[curr_num];
	}
	
	curr_num = DELAYBUFF_SMALL_NUM;
	while (--curr_num >= 0) {
		x->smallDelay_values[curr_num] = temp_sdv[curr_num];
	}
	
	curr_num = ALLPASS_MOD_NUM;
	while (--curr_num >= 0) {
		x->apMod_init_values[curr_num] = temp_apmv[curr_num];
	}
	
	// osc table
	rbb_init_sinTable(ot_ptr);
	
	// lowpass filters
	curr_num = LOWPASS_NUM;
	while (--curr_num >= 0) {
		rbb_init_lowPass(lpf_ptr + curr_num);
	}
	
	// allpass short filters
	curr_num = ALLPASS_SHORT_NUM;
	while (--curr_num >= 0) {
		rbb_init_allpassShort(aps_ptr + curr_num);
		rbb_set_allpassShort_delay(aps_ptr + curr_num, x->apShort_values[curr_num]);
	}
	
	// allpass long filters
	curr_num = ALLPASS_LONG_NUM;
	while (--curr_num >= 0) {
		rbb_init_allpassLong(apl_ptr + curr_num);
		rbb_set_allpassLong_delay(apl_ptr + curr_num, x->apLong_values[curr_num]);
	}
	
	// allpass mod filters
	curr_num = ALLPASS_MOD_NUM;
	while (--curr_num >= 0) {
		rbb_init_allpassMod(apm_ptr + curr_num, ot_ptr);
		rbb_set_allpassMod_delay(apm_ptr + curr_num, x->apMod_init_values[curr_num]);
	}
	
	// short delay buffers
	curr_num = DELAYBUFF_SMALL_NUM;
	while (--curr_num >= 0) {
		rbb_init_shortDelay(sd_ptr + curr_num);
		rbb_set_shortDelay_delay(sd_ptr + curr_num, x->smallDelay_values[curr_num]);
	}
	
	rbb_set_lowPass_coeff(lpf_ptr, BANDWIDTH);
	rbb_set_lowPass_coeff(lpf_ptr + 1, DAMPING);
	rbb_set_lowPass_coeff(lpf_ptr + 2, DAMPING);
	
	rbb_set_allpassShort_coeff(aps_ptr, IN_DIFF_1);
	rbb_set_allpassShort_coeff(aps_ptr + 1, IN_DIFF_1);
	rbb_set_allpassShort_coeff(aps_ptr + 2, IN_DIFF_2);
	rbb_set_allpassShort_coeff(aps_ptr + 3, IN_DIFF_2);
	
	rbb_set_allpassMod_coeff(apm_ptr, DEC_DIFF_1);
	rbb_set_allpassMod_coeff(apm_ptr + 1, DEC_DIFF_1);
	
	apm_ptr->oscDepth = AP_MODDEPTH_1;
	(apm_ptr + 1)->oscDepth = AP_MODDEPTH_2;
	
	rbb_set_allpassLong_coeff(apl_ptr, DEC_DIFF_2);
	rbb_set_allpassLong_coeff(apl_ptr + 1, DEC_DIFF_2);
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: lowpass coeff is %f", OBJECT_NAME, (x->lpFilters)->coeff);
	#endif /* DEBUG */
	
	x->lastout_L = 0.0;
	x->lastout_R = 0.0;
}

/********************************************************************************
void gverb_free(t_gverb *x)

inputs:			x		-- pointer to our object
				
description:	frees memory used by the reverb algorithm
returns:		nothing
********************************************************************************/
void gverb_free(t_gverb *x)
{
	int curr_num;
	// local pointers to building blocks
	rbb_sintable *ot_ptr = &(x->oscTable);
	rbb_allpass_short *aps_ptr = x->apFilters_short;
	rbb_allpass_long *apl_ptr = x->apFilters_long;
	rbb_allpass_mod *apm_ptr = x->apFilters_mod;
	rbb_delaybuff_short *sd_ptr = x->delayBuffs_small;
	
	// must be first
	dsp_free((t_pxobject *)x);
	
	// osc table
	rbb_free_sinTable(ot_ptr);
	
	// allpass short filters
	curr_num = ALLPASS_SHORT_NUM;
	while (--curr_num >= 0) {
		rbb_free_allpassShort(aps_ptr + curr_num);
	}
	
	// allpass long filters
	curr_num = ALLPASS_LONG_NUM;
	while (--curr_num >= 0) {
		rbb_free_allpassLong(apl_ptr + curr_num);
	}
	
	// allpass mod filters
	curr_num = ALLPASS_MOD_NUM;
	while (--curr_num >= 0) {
		rbb_free_allpassMod(apm_ptr + curr_num);
	}
	
	// short delay buffers
	curr_num = DELAYBUFF_SMALL_NUM;
	while (--curr_num >= 0) {
		rbb_free_shortDelay(sd_ptr + curr_num);
	}
	
	
}

/* the following methods are only compiled into the code during debugging*/
#ifdef DEBUG

#endif /* DEBUG */

