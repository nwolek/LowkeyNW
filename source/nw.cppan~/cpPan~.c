/*
** cpPan~.c
**
** MSP object
** allows mono input signal to be panned across two output channels
** 
** 2001/03/22 started by Nathan Wolek
** 2001/03/23 finished by Nathan Wolek
** 2002/07/11 fixed problem at 1.0 on tip from Ben Nevile
** 2002/08/23 removed resource dependent code, finally fixed table properly
** 2002/09/23 added getinfo message
** 2002/11/19 made Carbon compatable
*/


#include "ext.h"		// required for all MAX external objects
#include "z_dsp.h"		// required for all MSP external objects
#include <math.h>		// required for certain math functions
#include <string.h>

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"cpPan~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

#define DEFAULT_POS		0.5					// default position, when no argument given
#define TABLE_SIZE 		1024				// size of table used for panning function
#define TABLE_COEFF		(sqrt(2.0) / 2.0)	// coefficient used in calculating table
#define DtoR 			2.0 * 3.1415927 / 360.0
						// allows easy conversion from degrees to radians

void *this_class;		// required global pointer to this class

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
void cpPan_dsp(t_cpPan *x, t_signal **sp, short *count);
t_int *cpPan_perform1(t_int *w);
t_int *cpPan_perform2(t_int *w);
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
returns:		nothing
********************************************************************************/
void main(void)
{
	setup(&this_class, cpPan_new, (method)dsp_free, (short)sizeof(t_cpPan), 0L, 
				A_DEFFLOAT, 0);
	addmess((method)cpPan_dsp, "dsp", A_CANT, 0);
	
	#ifdef DEBUG
		addmess((method)cpPan_table, "table", A_DEFLONG, 0);
		addmess((method)cpPan_position, "position", 0);
	#endif /* DEBUG */
	
	/* bind method "cpPan_float" to the assistance message */
	addfloat((method)cpPan_float);
	
	/* bind method "cpPan_assist" to the assistance message */
	addmess((method)cpPan_assist, "assist", A_CANT, 0);
	
	/* bind method "cpPan_getinfo" to the getinfo message */
	addmess((method)cpPan_getinfo, "getinfo", A_NOTHING, 0);
	
	dsp_initclass();
	
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
		post("%s: Tables filled", OBJECT_NAME);
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
	t_cpPan *x = (t_cpPan *)newobject(this_class);
	
	dsp_setup((t_pxobject *)x, 2);					// two inlets
	outlet_new((t_pxobject *)x, "signal");			// left outlet
	outlet_new((t_pxobject *)x, "signal");			// right outlet
	
	cpPan_fillTables(x);							// fill the tables
	cpPan_setPosVars(x, initial_pos);				// set the initial position
	
	x->x_obj.z_misc = Z_NO_INPLACE;
	
	/* return a pointer to the new object */
	return (x);
}

/********************************************************************************
void cpPan_dsp(t_cpPan *x, t_signal **sp, short *count)

inputs:			x		-- pointer to this object
				sp		-- array of pointers to input & output signals
				count	-- array of shorts detailing number of signals attached
					to each inlet
description:	called when DSP call chain is built; adds object to signal flow
returns:		nothing
********************************************************************************/
void cpPan_dsp(t_cpPan *x, t_signal **sp, short *count)
{
	if (count[1])
	{
		dsp_add(cpPan_perform2, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, 
				sp[3]->s_vec, sp[0]->s_n);
		#ifdef DEBUG
			post("%s: pan values are being updated at audio rate", OBJECT_NAME);
		#endif /* DEBUG */
	}
	else
	{
		dsp_add(cpPan_perform1, 5, x, sp[0]->s_vec, sp[2]->s_vec, sp[3]->s_vec, 
				sp[0]->s_n);
		#ifdef DEBUG
			post("%s: pan values are being updated at control rate", OBJECT_NAME);
		#endif /* DEBUG */
	}
}

/********************************************************************************
t_int *cpPan_perform1(t_int *w)

inputs:			w		-- array of signal vectors specified in "cpPan_dsp"
description:	called at interrupt level to compute object's output; used when
		the panning information is input at the control rate
returns:		pointer to the next 
********************************************************************************/
t_int *cpPan_perform1(t_int *w)
{
	float valL, valR;						// initialize value variables for loop
	
	t_cpPan *x = (t_cpPan *)(w[1]);			// create local pointer to this object
	float *in = (float *)(w[2]);			// create local pointer to audio input
	float *outL = (float *)(w[3]);			// create local pointer to left output
	float *outR = (float *)(w[4]);			// create local pointer to right output
	long vector_size = w[5] + 1;				// create lacal var for vector size
	
	float multL = x->curr_multL;			// get current left channel multiplier
	float multR = x->curr_multR;			// get current right channel multiplier
	
	while (--vector_size)					// compute for each sample in vector
	{
		valL = *in;
		*outL = multL * valL;				// multiply left value by table value
		
		valR = *in;
		*outR = multR * valR;				// multiply right value by table value
		
		++in, ++outL, ++outR;				// advance the pointers
	}
	return(w + 6);							// pointer to next argument index
}

/********************************************************************************
t_int *cpPan_perform2(t_int *w)

inputs:			w		-- array of signal vectors specified in "cpPan_dsp"
description:	called at interrupt level to compute object's output; used when
		the panning information is input at the audio rate
returns:		pointer to the next 
********************************************************************************/
t_int *cpPan_perform2(t_int *w)
{
	float valL, valR, pan_val;	
	long pan_index;							// initialize variables for loop
	
	t_cpPan *x = (t_cpPan *)(w[1]);			// create local pointer to this object
	float *in = (float *)(w[2]);			// create local pointer to audio input
	float *pan_in = (t_float *)(w[3]);		// create local pointer to pan input
	float *outL = (float *)(w[4]);			// create local pointer to left output
	float *outR = (float *)(w[5]);			// create local pointer to right output
	int vector_size = (int)(w[6]) + 1;		// create lacal var for vector size
	float *tabL = x->table_left;			// create local pointer to left table
	float *tabR = x->table_right;			// create local pointer to right table
	
	while (--vector_size)					// compute for each sample in vector
	{
		pan_val = *pan_in;								// get "pan_val"
		
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
		*outL = tabL[pan_index] * valL;	// multiply left value by table value
		
		valR = *in;
		*outR = tabR[pan_index] * valR;	// multiply right value by table value
		
		++in, ++outL, ++outR, ++pan_in;		// advance the pointers
	}
	return(w + 7);							// pointer to next argument index
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
		post("%s: left inlet does not accept floats", OBJECT_NAME);
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
		post("%s: pan value is out of range", OBJECT_NAME);	
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
		post("%s: assist message displayed", OBJECT_NAME);
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
	post("%s object by Nathan Wolek", OBJECT_NAME);
	post("Last updated on %s - www.nathanwolek.com", __DATE__);
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
			post("%s: at table position %ld the signal will be multiplied by...", 
						OBJECT_NAME, value);
			post("Left channel: %f", x->table_left[value]);
			post("Right channel: %f", x->table_right[value]);
		}
		else
		{
			post("%s: value %ld is out of array bounds", OBJECT_NAME, value);
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
		post("%s: pan position = %f, table index = %ld", OBJECT_NAME, x->curr_pos, 
					x->curr_index);
		post("%s: at this table position, the signal will be multiplied by...", 
						OBJECT_NAME);
		post("Left channel: %f", x->curr_multL);
		post("Right channel: %f", x->curr_multR);
	}
#endif /* DEBUG */

