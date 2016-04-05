/*
** nw.cppan~.c
**
** MSP object
** allows mono input signal to be panned across two output channels
** 2001/03/22 started by Nathan Wolek
**
** Copyright © 2001,2014 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
**
*/


#include "c74_msp.h"

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.cppan~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

#define DEFAULT_POS		0.5					// default position, when no argument given
#define TABLE_SIZE 		1024				// size of table used for panning function
#define TABLE_COEFF		(sqrt(2.0) / 2.0)	// coefficient used in calculating table
#define DtoR 			2.0 * 3.1415927 / 360.0
						// allows easy conversion from degrees to radians

static t_class *cpPan_class;		// required global pointer to this class

/* structure definition for this object */
typedef struct _cpPan
{
	t_pxobject x_obj;
	float curr_pos;
	long curr_index;
	float curr_multL;
	float curr_multR;
	float table_left[TABLE_SIZE];
	float table_right[TABLE_SIZE];
} t_cpPan;

/* method definitions for this object */
void cpPan_fillTables(t_cpPan *x);
void *cpPan_new(double initial_pos);
void cpPan_dsp64(t_cpPan *x, t_object *dsp64, short *count, double samplerate,
                  long maxvectorsize, long flags);
void cpPan_perform64c(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void cpPan_perform64a(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void cpPan_float(t_cpPan *x, double f);
void cpPan_setPosVars(t_cpPan *x, double f);
void cpPan_assist(t_cpPan *x, t_object *b, long msg, long arg, char *s);
void cpPan_getinfo(t_cpPan *x);
/* method definitions for debugging this object */
#ifdef DEBUG
	void cpPan_table(t_cpPan *x, long value);
	void cpPan_position(t_cpPan *x);
#endif /* DEBUG */

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
    
    c = class_new(OBJECT_NAME, (method)cpPan_new, (method)dsp_free, (short)sizeof(t_cpPan), 0L,
                  A_DEFFLOAT, 0);
    class_dspinit(c); // add standard functions to class
	
	#ifdef DEBUG
		class_addmethod(c, (method)cpPan_table, "table", A_DEFLONG, 0);
		class_addmethod(c, (method)cpPan_position, "position", 0);
	#endif /* DEBUG */
	
	/* bind method "cpPan_float" to the assistance message */
	class_addmethod(c, (method)cpPan_float, "float", A_FLOAT, 0);
	
	/* bind method "cpPan_assist" to the assistance message */
	class_addmethod(c, (method)cpPan_assist, "assist", A_CANT, 0);
	
	/* bind method "cpPan_getinfo" to the getinfo message */
	class_addmethod(c, (method)cpPan_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "cpPan_dsp64" to the dsp64 message */
    class_addmethod(c, (method)cpPan_dsp64, "dsp64", A_CANT, 0);
	
    class_register(C74_CLASS_BOX, c); // register the class w max
    cpPan_class = c;
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: main function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    return 0;
	
}

/********************************************************************************
void cpPan_fillTables(t_cpPan *x)

inputs:			x		-- pointer to this object
description:	fills "table_left" and "table_right" with values used to determine 
		amplitude of each channel during panning
returns:		nothing
********************************************************************************/
void cpPan_fillTables(t_cpPan *x)
{
	float *tab_a = (float *)(x->table_left);		// get pointer to table_left
	float *tab_b = (float *)(x->table_right);		// get pointer to table_right
	long n = TABLE_SIZE;							// set counter equal to TABLE_SIZE
	float pos_deg, pos_rad;							// initial loop vars
	float degtorad = DtoR;
	float tab_coeff = TABLE_COEFF;
	
	while (--n >= 0) {
		pos_deg = ((float)n / (float)(TABLE_SIZE-1)) * -90.0 + 45.0;	// current position in degrees
		pos_rad = pos_deg * degtorad;					// convert to radians
		
		/* fill table with values */
// WINDOWS CANT LINK THESE, SO I SWAPPED THEM OUT	-TAP (2004.08.05)
//		tab_a[n] = tab_coeff * (cosf(pos_rad) + sinf(pos_rad));
//		tab_b[n] = tab_coeff * (cosf(pos_rad) - sinf(pos_rad));
		tab_a[n] = tab_coeff * (cos(pos_rad) + sin(pos_rad));
		tab_b[n] = tab_coeff * (cos(pos_rad) - sin(pos_rad));
	}
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: Tables filled", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void *cpPan_new(double initial_pos)

inputs:			initial_pos		-- object argument 1; initial position of pan
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; sets argument for "initial_pos"
returns:		nothing
********************************************************************************/
void *cpPan_new(double initial_pos)
{
	t_cpPan *x = (t_cpPan *) object_alloc((t_class*) cpPan_class);
	
	dsp_setup((t_pxobject *)x, 2);					// two inlets
	outlet_new((t_pxobject *)x, "signal");			// left outlet
	outlet_new((t_pxobject *)x, "signal");			// right outlet
	
	cpPan_fillTables(x);							// fill the tables
	cpPan_setPosVars(x, initial_pos);				// set the initial position
	
	x->x_obj.z_misc = Z_NO_INPLACE;
	
    #ifdef DEBUG
        object_post((t_object*)x, "%s: new function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
	/* return a pointer to the new object */
	return (x);
}


/********************************************************************************
 void cpPan_dsp64()
 
 inputs:    x		-- pointer to this object
            dsp64		-- signal chain to which object belongs
            count	-- array detailing number of signals attached to each inlet
            samplerate -- number of samples per second
            maxvectorsize -- sample frames per vector of audio
            flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void cpPan_dsp64(t_cpPan *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    if (count[1])
    {
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)cpPan_perform64a, 0, NULL);
        #ifdef DEBUG
            object_post((t_object*)x, "%s: pan values are being updated at audio rate", OBJECT_NAME);
        #endif /* DEBUG */
    }
    else
    {
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)cpPan_perform64c, 0, NULL);
        #ifdef DEBUG
            object_post((t_object*)x, "%s: pan values are being updated at control rate", OBJECT_NAME);
        #endif /* DEBUG */
    }
    
}


/********************************************************************************
 void *cpPan_perform64c(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,
 long numouts, long vectorsize, long flags, void *userparam)
 
 inputs:	x		--
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
void cpPan_perform64c(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,
                          long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars
    double *in = ins[0];
    double *outL = outs[0];
    double *outR = outs[1];
    double multL = x->curr_multL;			// get current left channel multiplier
    double multR = x->curr_multR;			// get current right channel multiplier
    
    // local vars used for while loop
    double valL, valR;
    long n;
    
    // check constraints
    // completed by cpPan_setPosVars(), so we don't repeat here
    
    n = vectorsize;
    while(n--)
    {
        valL = *in;
        *outL = multL * valL;				// multiply left value by table value
        
        valR = *in;
        *outR = multR * valR;				// multiply right value by table value
        
        ++in, ++outL, ++outR;				// advance the pointers
    }
    
    // update object variables
    // none changed, so we don't need to make updates
    
}

/********************************************************************************
 void *cpPan_perform64a(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,
 long numouts, long vectorsize, long flags, void *userparam)
 
 inputs:	x		--
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
void cpPan_perform64a(t_cpPan *x, t_object *dsp64, double **ins, long numins, double **outs,
                      long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars
    double *in = ins[0];
    double *pan_in = ins[1];
    double *outL = outs[0];
    double *outR = outs[1];
    float *tabL = x->table_left;			// create local pointer to left table, TODO: update to doubles
    float *tabR = x->table_right;			// create local pointer to right table, TODO: update to doubles
    
    // local vars used for while loop
    double valL, valR, pan_val;
    long n, pan_index;
    
    n = vectorsize;
    while(n--)
    {
        pan_val = *pan_in;								// get "pan_val"
        
        // check constraints
        if (pan_val < 0.0)			// if less than 0
        {
            pan_val = 0.0;
        }
        else if (pan_val > 1.0)		// if greater than table length
        {
            pan_val = 1.0;
        }
        
        pan_index = (long) ((pan_val * (float)(TABLE_SIZE - 1)) + 0.5);
        // set "pan_index"
        
        valL = *in;
        *outL = tabL[pan_index] * valL;				// multiply left value by table value
        
        valR = *in;
        *outR = tabR[pan_index] * valR;				// multiply right value by table value
        
        ++in, ++outL, ++outR, ++pan_in;				// advance the pointers
    }
    
    // update object variables
    x->curr_pos = (float)pan_val;
    
}

/********************************************************************************
void cpPan_float(t_cpPan *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; right inlet sets "x_pos" variable;
		left inlet generates error message in max window
returns:		nothing
********************************************************************************/
void cpPan_float(t_cpPan *x, double f)
{
	if (x->x_obj.z_in == 1) // if right inlet
	{
		cpPan_setPosVars(x, f);
	}
	else if (x->x_obj.z_in == 0)
	{
		object_post((t_object*)x, "%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void cpPan_setPosVars(t_cpPan *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	uses float input to set "curr_pos", "curr_index", "curr_multL",
		and "curr_multR" variable, check to make sure value is in range
returns:		nothing
********************************************************************************/
void cpPan_setPosVars(t_cpPan *x, double f)
{
	long index;
	
	if (f >= 0.0 && f <= 1.0) // if within 0 and 1
	{
		x->curr_pos = f;
		
		index = (long) ((f * (float)(TABLE_SIZE - 1)) + 0.5);
		
		x->curr_multL = x->table_left[index];
		x->curr_multR = x->table_right[index];
		
		x->curr_index = index;
	}
	else
	{
		object_post((t_object*)x, "%s: pan value is out of range", OBJECT_NAME);	
	}
}

/********************************************************************************
void cpPan_assist(t_cpPan *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void cpPan_assist(t_cpPan *x, t_object *b, long msg, long arg, char *s)
{
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) input");
				break;
			case 1:
				strcpy(s, "(signal/float) pan position; 0. = hard left, 1. = hard right");
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
void cpPan_getinfo(t_cpPan *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; prints info
		about object and its last update
returns:		nothing
********************************************************************************/
void cpPan_getinfo(t_cpPan *x)
{
	object_post((t_object*)x, "%s object by Nathan Wolek", OBJECT_NAME);
	object_post((t_object*)x, "Last updated on %s - www.nathanwolek.com", __DATE__);
}

/* the following methods are only compiled into the code during debugging*/
#ifdef DEBUG
/********************************************************************************
void cpPan_table(t_cpPan *x, long value)

inputs:			x		-- pointer to this object
				value	-- argument from "table" message; sets table index
description:	inquire the array values for a specified table index
returns:		nothing
********************************************************************************/
	void cpPan_table(t_cpPan *x, long value)
	{
		if (value >= 0 && value <= (TABLE_SIZE - 1)) 
		{
			object_post((t_object*)x, "%s: at table position %ld the signal will be multiplied by...", 
						OBJECT_NAME, value);
			object_post((t_object*)x, "Left channel: %f", x->table_left[value]);
			object_post((t_object*)x, "Right channel: %f", x->table_right[value]);
		}
		else
		{
			object_post((t_object*)x, "%s: value %ld is out of array bounds", OBJECT_NAME, value);
		}
	}
	
/********************************************************************************
void cpPan_position(t_cpPan *x)

inputs:			x		-- pointer to this object
description:	inquire the current position of pan setting
returns:		nothing
********************************************************************************/
	void cpPan_position(t_cpPan *x)
	{
		object_post((t_object*)x, "%s: pan position = %f, table index = %ld", OBJECT_NAME, x->curr_pos, 
					x->curr_index);
		object_post((t_object*)x, "%s: at this table position, the signal will be multiplied by...", 
						OBJECT_NAME);
		object_post((t_object*)x, "Left channel: %f", x->curr_multL);
		object_post((t_object*)x, "Right channel: %f", x->curr_multR);
	}
#endif /* DEBUG */

