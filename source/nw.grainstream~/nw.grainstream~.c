/*
** nw.grainstream~.c
**
** MSP object
** sends out a continuous stream of grains 
** 2001/03/29 started by Nathan Wolek
**
** Copyright © 2002,2015 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
**
*/

#include "ext.h"		// required for all MAX external objects
#include "ext_obex.h"   // required for new style MAX objects
#include "z_dsp.h"		// required for all MSP external objects
#include "buffer.h"		// required to deal with buffer object
#include <string.h>


#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.grainstream~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for direction flag */
#define FORWARD_GRAINS		0
#define REVERSE_GRAINS		1

/* for interpolation flag */
#define INTERP_OFF			0
#define INTERP_ON			1

static t_class *grainstream_class;		// required global pointing to this class

typedef struct _grainstream
{
	t_pxobject x_obj;				// <--
	// sound buffer info
    t_symbol *snd_sym;
    t_buffer_ref *snd_buf_ptr;
    t_buffer_ref *next_snd_buf_ptr;
	//double snd_last_out;
	//long snd_buf_length;	//removed 2002.07.11
	short snd_interp;
	// window buffer info
    t_symbol *win_sym;
    t_buffer_ref *win_buf_ptr;
    t_buffer_ref *next_win_buf_ptr;
	//double win_last_out;
	double win_last_index;
	//long win_buf_length;	//removed 2002.07.11
	short win_interp;
	// current grain info
	double grain_freq;		// in hertz
	double grain_pos_start;	// in samples
    double grain_pitch;		// as multiplier
    double grain_gain;		// linear gain mult
	double grain_length;	// in milliseconds
	double grain_sound_length;	// in milliseconds
	double win_step_size;	// in samples
	double snd_step_size;	// in samples
	double curr_win_pos;	// in samples
	double curr_snd_pos;	// in samples
	short grain_direction;	// forward or reverse
	// defered grain info at control rate
	double next_grain_freq;			// in hertz
	double next_grain_pos_start;	// in milliseconds
    double next_grain_pitch;		// as multiplier
    double next_grain_gain;         // linear gain mult
	short next_grain_direction;		// forward or reverse
	// signal or control grain info
	short grain_freq_connected;				// <--
	short grain_pos_start_connected;		// <--
	short grain_pitch_connected;			// <--
    short grain_gain_connected;
    long curr_count_samp;
	double output_sr;						// <--
	double output_1oversr;					// <--
} t_grainstream;

void *grainstream_new(t_symbol *snd, t_symbol *win);
t_int *grainstream_perform(t_int *w);
t_int *grainstream_perform0(t_int *w);
void grainstream_perform64zero(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainstream_perform64(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainstream_initGrain(t_grainstream *x, float in_pos_start, float in_pitch_mult, float in_length, float in_gain_mult);
void grainstream_dsp(t_grainstream *x, t_signal **sp, short *count);
void grainstream_dsp64(t_grainstream *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void grainstream_setsnd(t_grainstream *x, t_symbol *s);
void grainstream_setwin(t_grainstream *x, t_symbol *s);
void grainstream_float(t_grainstream *x, double f);
void grainstream_int(t_grainstream *x, long l);
void grainstream_sndInterp(t_grainstream *x, long l);
void grainstream_winInterp(t_grainstream *x, long l);
void grainstream_reverse(t_grainstream *x, long l);
void grainstream_assist(t_grainstream *x, t_object *b, long msg, long arg, char *s);
void grainstream_getinfo(t_grainstream *x);
double mcLinearInterp(float *in_array, long index_i, double index_frac, long in_size, short in_chans);

t_symbol *ps_buffer;

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
    
    c = class_new(OBJECT_NAME, (method)grainstream_new, (method)dsp_free,
                  (short)sizeof(t_grainstream), 0L, A_SYM, A_SYM, 0);
    class_dspinit(c); // add standard functions to class
    
	class_addmethod(c, (method)grainstream_dsp, "dsp", A_CANT, 0);
	
	/* bind method "grainstream_setsnd" to the 'setSound' message */
	class_addmethod(c, (method)grainstream_setsnd, "setSound", A_SYM, 0);
	
	/* bind method "grainstream_setwin" to the 'setWin' message */
	class_addmethod(c, (method)grainstream_setwin, "setWin", A_SYM, 0);
	
	/* bind method "grainstream_float" to incoming floats */
	class_addmethod(c, (method)grainstream_float, "float", A_FLOAT, 0);
	
	/* bind method "grainstream_int" to incoming ints */
	class_addmethod(c, (method)grainstream_int, "int", A_LONG, 0);
	
	/* bind method "grainstream_reverse" to the direction message */
	class_addmethod(c, (method)grainstream_reverse, "reverse", A_LONG, 0);
	
	/* bind method "grainstream_sndInterp" to the sndInterp message */
	class_addmethod(c, (method)grainstream_sndInterp, "sndInterp", A_LONG, 0);
	
	/* bind method "grainstream_winInterp" to the winInterp message */
	class_addmethod(c, (method)grainstream_winInterp, "winInterp", A_LONG, 0);
	
	/* bind method "grainstream_assist" to the assistance message */
	class_addmethod(c, (method)grainstream_assist, "assist", A_CANT, 0);
	
	/* bind method "grainstream_getinfo" to the getinfo message */
	class_addmethod(c, (method)grainstream_getinfo, "getinfo", A_NOTHING, 0);
	
    /* bind method "grainstream_dsp64" to the dsp64 message */
    class_addmethod(c, (method)grainstream_dsp64, "dsp64", A_CANT, 0);
    
    class_register(CLASS_BOX, c); // register the class w max
    grainstream_class = c;
	
	/* needed for 'buffer~' work, checks for validity of buffer specified */
	ps_buffer = gensym("buffer~");
	
    #ifdef DEBUG
        post("%s: main function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    return 0;
}

/********************************************************************************
void *grainstream_new(double initial_pos)

inputs:			*snd		-- name of buffer holding sound
				*win		-- name of buffer holding window
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; sets variables and buffers
returns:		nothing
********************************************************************************/
void *grainstream_new(t_symbol *snd, t_symbol *win)
{
	t_grainstream *x = (t_grainstream *) object_alloc((t_class*) grainstream_class);
	dsp_setup((t_pxobject *)x, 4);					// four inlets
    outlet_new((t_pxobject *)x, "signal");          // sample count outlet
    outlet_new((t_pxobject *)x, "signal");			// signal ch2 outlet
    outlet_new((t_pxobject *)x, "signal");			// signal ch1 outlet
	
	/* set buffer names */
	x->snd_sym = snd;
	x->win_sym = win;
	
	/* zero pointers */
	x->snd_buf_ptr = x->next_snd_buf_ptr = NULL;
	x->win_buf_ptr = x->next_win_buf_ptr = NULL;
	
	/* setup variables */
	x->grain_freq = x->next_grain_freq = 20.0;
	x->grain_pos_start = x->next_grain_pos_start = 0.0;
    x->grain_pitch = x->next_grain_pitch = 1.0;
    x->grain_gain = x->next_grain_gain = 1.0;
	x->win_step_size = x->snd_step_size = 0.0;
	x->curr_win_pos = x->curr_snd_pos = 0.0;
	
	/* set flags to defaults */
	x->snd_interp = INTERP_ON;
	x->win_interp = INTERP_ON;
	x->grain_direction = x->next_grain_direction = FORWARD_GRAINS;
	
	x->x_obj.z_misc = Z_NO_INPLACE;
	
	/* return a pointer to the new object */
	return (x);
}

/********************************************************************************
void grainstream_dsp(t_cpPan *x, t_signal **sp, short *count)

inputs:			x		-- pointer to this object
				sp		-- array of pointers to input & output signals
				count	-- array of shorts detailing number of signals attached
					to each inlet
description:	called when DSP call chain is built; adds object to signal flow
returns:		nothing
********************************************************************************/
void grainstream_dsp(t_grainstream *x, t_signal **sp, short *count)
{
    #ifdef DEBUG
        post("%s: adding 32 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    /* set buffers */
	grainstream_setsnd(x, x->snd_sym);
	grainstream_setwin(x, x->win_sym);
	
	/* test inlet 2 and 3 for signal data */
	x->grain_freq_connected = count[0];
	x->grain_pos_start_connected = count[1];
	x->grain_pitch_connected = count[2];
    x->grain_gain_connected = count[3];
	
	x->output_sr = sp[4]->s_sr;
	x->output_1oversr = 1.0 / x->output_sr;
	
	if (!count[4]) {	// nothing computed
		//dsp_add(grainstream_perform0, 2, sp[3]->s_vec, sp[3]->s_n);
		#ifdef DEBUG
			post("%s: no output computed", OBJECT_NAME);
		#endif /* DEBUG */
	} else {		// output computed
		dsp_add(grainstream_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,
			sp[3]->s_vec, sp[3]->s_n);
		#ifdef DEBUG
			post("%s: output is being computed", OBJECT_NAME);
		#endif /* DEBUG */
	}
	
}

/********************************************************************************
 void grainstream_dsp64()
 
 inputs:     x		-- pointer to this object
 dsp64		-- signal chain to which object belongs
 count	-- array detailing number of signals attached to each inlet
 samplerate -- number of samples per second
 maxvectorsize -- sample frames per vector of audio
 flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void grainstream_dsp64(t_grainstream *x, t_object *dsp64, short *count, double samplerate,
                      long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        post("%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    // set buffers
    grainstream_setsnd(x, x->snd_sym);
    grainstream_setwin(x, x->win_sym);
    
    // test inlets for signal connections
    x->grain_freq_connected = count[0];
    x->grain_pos_start_connected = count[1];
    x->grain_pitch_connected = count[2];
    x->grain_gain_connected = count[3];
    
    x->output_sr = samplerate;
    x->output_1oversr = 1.0 / x->output_sr;
    
    if (count[4] || count[5]) // if either output is connected
    {
        #ifdef DEBUG
            post("%s: output is being computed", OBJECT_NAME);
        #endif /* DEBUG */
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)grainstream_perform64zero, 0, NULL); // TEMP ZERO
    } else {
        #ifdef DEBUG
            post("%s: no output computed", OBJECT_NAME);
        #endif /* DEBUG */
    }
    
}

/********************************************************************************
t_int *grainstream_perform(t_int *w)

inputs:			w		-- array of signal vectors specified in "grainstream_dsp"
description:	called at interrupt level to compute object's output; used when
		outlets are connected; tests inlet 2 & 3 to use either control or audio
		rate data
returns:		pointer to the next 
********************************************************************************/
t_int *grainstream_perform(t_int *w)
{
	t_grainstream *x = (t_grainstream *)(w[1]);
	float *in_freq = (float *)(w[2]);
	float *in_pos_start = (float *)(w[3]);
	float *in_pitch_mult = (float *)(w[4]);
	float *out = (t_float *)(w[5]);
	int vec_size = (int)(w[6]);
	float out_1oversr = (float)x->output_1oversr;
	
	t_buffer_obj *snd_object, *win_object;
	float *tab_s, *tab_w;
	double s_step_size, w_step_size;
	double  snd_out, win_out;//, last_s, last_w;	//removed 2005.02.03
	double index_s, index_w, last_index_w, temp_index_frac;
	long size_s, size_w, temp_index_int;
	short interp_s, interp_w, g_direction;
	
	vec_size += 1;		//increase by one for pre-decrement
	--out;				//decrease by one for pre-increment
	
	if (x->x_obj.z_disabled)					// object is enabled
		goto out;
    if ((x->snd_buf_ptr == NULL) || (x->win_buf_ptr == NULL))		// buffer pointers are defined
        goto zero;
	
    // get sound buffer info
    snd_object = buffer_ref_getobject(x->snd_buf_ptr);
    tab_s = buffer_locksamples(snd_object);
    if (!tab_s)		// buffer samples were not accessible
        goto zero;
    size_s = buffer_getframecount(snd_object);
    
    // get window buffer info
    win_object = buffer_ref_getobject(x->win_buf_ptr);
    tab_w = buffer_locksamples(win_object);
    if (!tab_w)		// buffer samples were not accessible
        goto zero;
    size_w = buffer_getframecount(win_object);
	
	// get option settings
	interp_s = x->snd_interp;
	interp_w = x->win_interp;
	g_direction = x->grain_direction;
	// get pointer info
	s_step_size = x->snd_step_size;
	w_step_size = x->win_step_size;
	index_s = x->curr_snd_pos;
	index_w = x->curr_win_pos;
	// history
	last_index_w = x->win_last_index;
	
	while (--vec_size) {
		index_w += w_step_size;
		
		/* check bounds of window index */
		while (index_w < 0)
			index_w += size_w;
		while (index_w >= size_w)
			index_w -= size_w;
			
		if (index_w < last_index_w) {		// if window has wrapped...
			if (index_w < 10.0) {			// and is at beginning...
				// ...then begin a new grain
				
				if (x->next_snd_buf_ptr != NULL) {	//added 2002.07.24
                    buffer_unlocksamples(snd_object);
                    x->snd_buf_ptr = x->next_snd_buf_ptr;
                    x->next_snd_buf_ptr = NULL;
                    
                    snd_object = buffer_ref_getobject(x->snd_buf_ptr);
                    tab_s = buffer_locksamples(snd_object);
                    if (!tab_s)		// buffer samples were not accessible
                        goto zero;
                    size_s = buffer_getframecount(snd_object);
					
					#ifdef DEBUG
						post("%s: sound buffer pointer updated", OBJECT_NAME);
					#endif /* DEBUG */
				}
				if (x->next_win_buf_ptr != NULL) {	//added 2002.07.24
                    buffer_unlocksamples(win_object);
                    x->win_buf_ptr = x->next_win_buf_ptr;
                    x->next_win_buf_ptr = NULL;
                    
                    win_object = buffer_ref_getobject(x->win_buf_ptr);
                    tab_w = buffer_locksamples(win_object);
                    if (!tab_w)	{	// buffer samples were not accessible
                        goto zero;
                    }
                    size_w = buffer_getframecount(win_object);
					
					#ifdef DEBUG
						post("%s: window buffer pointer updated", OBJECT_NAME);
					#endif /* DEBUG */
				}
				
				//
				//
				
				x->grain_direction = x->next_grain_direction;
				g_direction = x->grain_direction;
				
				// test if freq should be at audio or control rate
				if (x->grain_freq_connected) { // if freq is at audio rate
					if (*in_freq != 0.0) x->grain_freq = *in_freq;
				} else { // if freq is at control rate
					if (x->next_grain_freq != 0.0) x->grain_freq = x->next_grain_freq;
				}
				x->grain_length = 1000.0 / x->grain_freq;
				
				// test if pitch should be at audio or control rate
				if (x->grain_pitch_connected) { // if pitch is at audio rate
					x->grain_pitch = *in_pitch_mult;
				} else { // if pitch is at control rate
					x->grain_pitch = x->next_grain_pitch;
				}
				x->grain_sound_length = x->grain_length * x->grain_pitch;
				
				// test if pos_start should be at audio or control rate
				if (x->grain_pos_start_connected) { // if position is at audio rate
					if (x->grain_direction == FORWARD_GRAINS) {	// if forward...
						x->grain_pos_start = *in_pos_start * buffer_getmillisamplerate(snd_object);
					} else {	// if reverse...
						x->grain_pos_start = (*in_pos_start + x->grain_sound_length) * buffer_getmillisamplerate(snd_object);
					}
				} else { // if position is at control rate
					if (x->grain_direction == FORWARD_GRAINS) {	// if forward...
						x->grain_pos_start = x->next_grain_pos_start * buffer_getmillisamplerate(snd_object);
					} else {	// if reverse...
						x->grain_pos_start = (x->next_grain_pos_start + x->grain_sound_length) * buffer_getmillisamplerate(snd_object);
					}
				}
				// compute window buffer step size per vector sample 
				x->win_step_size = size_w * x->grain_freq * out_1oversr;
				// compute sound buffer step size per vector sample
				x->snd_step_size = x->grain_pitch * buffer_getsamplerate(snd_object) * out_1oversr;
				// reset position tracking variables
				if (x->grain_direction == FORWARD_GRAINS) {	// if forward...
					x->curr_snd_pos = x->grain_pos_start - x->snd_step_size;
				} else {	// if reverse...
					x->curr_snd_pos = x->grain_pos_start + x->snd_step_size;
				}
				x->curr_win_pos = 0.0;
				// reset history
				//x->snd_last_out = x->win_last_out = 0.0;	//removed 2005.02.03
				
				// update local vars
				s_step_size = x->snd_step_size;
				w_step_size = x->win_step_size;
				index_s = x->curr_snd_pos;
				index_w = x->curr_win_pos;
				
				#ifdef DEBUG
					post("%s: beginning of grain", OBJECT_NAME);
				#endif /* DEBUG */
					
			}
		}
		
		/* sound index is a double because it uses interp */
		if (g_direction == FORWARD_GRAINS) {	// if forward...
			index_s += s_step_size;		// add to sound index
		} else {	// if reverse...
			index_s -= s_step_size;		// subtract from sound index
		}
		
		/* check bounds of sound index */
		while (index_s < 0)
			index_s += size_s;
		while (index_s >= size_s)
			index_s -= size_s;
		
		//WINDOW OUT
		
		/* handle temporary vars for interpolation */
		temp_index_int = (long)(index_w); // integer portion of index
		temp_index_frac = index_w - (double)temp_index_int; // fractional portion of index
		
		/*
		if (nc_w > 1) // if buffer has multiple channels...
		{
			// get index to sample from within the interleaved frame
			temp_index_int = temp_index_int * nc_w + chan_w;
		}
		*/
		
		switch (interp_w) {
			case INTERP_ON:
				// perform linear interpolation on window buffer output
				win_out = mcLinearInterp(tab_w, temp_index_int, temp_index_frac, size_w, 1);
				break;
			case INTERP_OFF:
				// interpolation sounds better than following, but uses more CPU
				win_out = tab_w[temp_index_int];
				break;
		}
		
		//SOUND OUT
		
		/* handle temporary vars for interpolation */
		temp_index_int = (long)(index_s); // integer portion of index
		temp_index_frac = index_s - (double)temp_index_int; // fractional portion of index
		
		/*
		if (nc_s > 1) // if buffer has multiple channels...
		{
			// get index to sample from within the interleaved frame
			temp_index_int = temp_index_int * nc_s + chan_s;
		}
		*/
		
		switch (interp_s) {
			case INTERP_ON:
				// perform linear interpolation on sound buffer output
				snd_out = mcLinearInterp(tab_s, temp_index_int, temp_index_frac, size_s, 1);
				break;
			case INTERP_OFF:
				// interpolation sounds better than following, but uses more CPU
				snd_out = tab_s[temp_index_int];
				break;
		}
		
		/* multiply snd_out by win_out */
		*++out = snd_out * win_out;
		
		/* update last output variables */
		//last_s = snd_out;	//removed 2005.02.03
		//last_w = win_out;	//removed 2005.02.03
		last_index_w = index_w;
		
		/* advance other pointers */
		++in_freq, ++in_pos_start, ++in_pitch_mult;
	}
	
	/* update global position and step_size vars */
	x->curr_snd_pos = index_s;
	x->curr_win_pos = index_w;
	x->snd_step_size = s_step_size;
	x->win_step_size = w_step_size;
	
	/* update last output variables */
	//x->snd_last_out = last_s;	//removed 2005.02.03
	//x->win_last_out = last_w;	//removed 2005.02.03
	x->win_last_index = index_w;
	
    buffer_unlocksamples(snd_object);
    buffer_unlocksamples(win_object);
	
	return (w + 7);
	
zero:
	while (--vec_size >= 0) *++out = 0.;
out:
	return (w + 7);
}

/********************************************************************************
t_int *grainstream_perform0(t_int *w)

inputs:			w		-- array of signal vectors specified in "grainstream_dsp"
description:	called at interrupt level to compute object's output; used when
		nothing is connected to output; saves CPU cycles
returns:		pointer to the next 
********************************************************************************/
t_int *grainstream_perform0(t_int *w)
{
	t_float *out = (t_float *)(w[1]);
	int vec_size = (int)(w[2]);

	vec_size += 1;		//increase by one for pre-decrement
	--out;				//decrease by one for pre-increment

	while (--vec_size >= 0) {
		*++out = 0.;
	}

	return (w + 3);
}

/********************************************************************************
 void *grainstream_perform64zero()
 
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
void grainstream_perform64zero(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,
                              long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars
    t_double *curr_out[numouts];
    long n, m;
    
    // fill local pointer array for outlets
    m = numouts;
    while(m--)
    {
        curr_out[m] = outs[m];
    }
    
    n = vectorsize;
    while(n--)
    {
        m = numouts;
        while(m--)
        {
            *(curr_out[m]) = 0.0;		// save to output
            (curr_out[m])++;			// advance the outlet pointer
        }
    }
    
}

/********************************************************************************
 void *grainstream_perform64()
 
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
void grainstream_perform64(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,
                          long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars outlets and inlets
    t_double *in_freq = ins[0];
    t_double *in_sound_start = ins[1];
    t_double *in_sample_increment = ins[2];
    t_double *in_gain = ins[3];
    t_double *out_signal = outs[0];
    t_double *out_signal2 = outs[1];
    t_double *out_sample_count = outs[2];
    
    // local vars for snd and win buffer
    t_buffer_obj *snd_object, *win_object;
    t_float *tab_s, *tab_w;
    double snd_out, win_out;
    long size_s, size_w;
    
    // local vars for object vars and while loop
    double index_s, index_w, temp_index_frac;
    long n, count_samp, temp_index_int;
    double s_step_size, w_step_size, w_last_index, approx_grain_length, g_gain;
    short interp_s, interp_w, g_direction;
    
    // check to make sure buffers are loaded with proper file types
    if (x->x_obj.z_disabled)		// and object is enabled
        goto out;
    if (x->snd_buf_ptr == NULL || (x->win_buf_ptr == NULL))
        goto zero;
    
    // get sound buffer info
    snd_object = buffer_ref_getobject(x->snd_buf_ptr);
    tab_s = buffer_locksamples(snd_object);
    if (!tab_s)		// buffer samples were not accessible
        goto zero;
    size_s = buffer_getframecount(snd_object);
    
    // get window buffer info
    win_object = buffer_ref_getobject(x->win_buf_ptr);
    tab_w = buffer_locksamples(win_object);
    if (!tab_w)		// buffer samples were not accessible
        goto zero;
    size_w = buffer_getframecount(win_object);
    
    // get snd and win index info
    index_s = x->curr_snd_pos;
    s_step_size = x->snd_step_size;
    index_w = x->curr_win_pos;
    w_step_size = x->win_step_size;
    
    // get grain options
    interp_s = x->snd_interp;
    interp_w = x->win_interp;
    g_gain = x->grain_gain;
    g_direction = x->grain_direction;
    
    // get history from last vector
    count_samp = x->curr_count_samp;
    w_last_index = x->win_last_index;
    
    n = vectorsize;
    while(n--) {
        
        // advance window index
        index_w += w_step_size;
        
        // wrap to make index in bounds
        while (index_w < 0.)
            index_w += size_w;
        while (index_w >= size_w)
            index_w -= size_w;
        
        if (index_w < w_last_index) {   // if window has wrapped...
            if (index_w < 10.0) {       // and it is beginning...
                buffer_unlocksamples(snd_object);
                buffer_unlocksamples(win_object);
                
                grainstream_initGrain(x, *in_freq, *in_sound_start, *in_sample_increment, *in_gain);
                
                // get snd buffer info
                snd_object = buffer_ref_getobject(x->snd_buf_ptr);
                tab_s = buffer_locksamples(snd_object);
                if (!tab_s)	{	// buffer samples were not accessible
                    *out_signal = 0.0;
                    *out_signal2 = 0.0;
                    *out_sample_count = (double)count_samp;
                    w_last_index = index_w;
                    goto advance_pointers;
                }
                size_s = buffer_getframecount(snd_object);
                
                // get win buffer info
                win_object = buffer_ref_getobject(x->win_buf_ptr);
                tab_w = buffer_locksamples(win_object);
                if (!tab_w)	{	// buffer samples were not accessible
                    *out_signal = 0.0;
                    *out_signal2 = 0.0;
                    *out_sample_count = (double)count_samp;
                    w_last_index = index_w;
                    goto advance_pointers;
                }
                size_w = buffer_getframecount(win_object);
                
                // get snd and win index info
                index_s = x->curr_snd_pos;
                s_step_size = x->snd_step_size;
                index_w = x->curr_win_pos;
                w_step_size = x->win_step_size;
                
                // get grain options
                interp_s = x->snd_interp;
                interp_w = x->win_interp;
                g_gain = x->grain_gain;
                g_direction = x->grain_direction;
                
                // get history
                count_samp = x->curr_count_samp;
            }
        }
        
        
    
advance_pointers:
        // advance all pointers
        ++in_freq, ++in_sound_start, ++in_sample_increment, ++in_gain;
        ++out_signal, ++out_signal2, ++out_sample_count;
    }

    // update object history for next vector
    x->curr_snd_pos = index_s;
    x->curr_count_samp = count_samp;
    x->win_last_index = w_last_index;

    buffer_unlocksamples(snd_object);
    buffer_unlocksamples(win_object);
    return;

    // alternate blank output
zero:
    n = vectorsize;
    while(n--)
    {
        *out_signal = 0.;
        *out_signal2 = 0.;
        *out_sample_count = -1.;
    }

out:
    return;
}

/********************************************************************************
 void grainstream_initGrain()
 
 inputs:			x					-- pointer to this object
 in_freq			-- frequency of grain production
 in_pos_start		-- offset within sampled buffer
 in_pitch_mult		-- sample playback speed, 1 = normal
 in_gain_mult		-- scales gain output, 1 = no change
 description:	initializes grain vars; called from perform method when pulse is
 received
 returns:		nothing
 ********************************************************************************/
void grainstream_initGrain(t_grainstream *x, float in_freq, float in_pos_start, float in_pitch_mult, float in_gain_mult)
{
    #ifdef DEBUG
        post("%s: initializing grain", OBJECT_NAME);
    #endif /* DEBUG */
    
    /* should the buffers be updated ? */
    
    t_buffer_obj	*snd_object;
    t_buffer_obj	*win_object;
    
    if (x->next_snd_buf_ptr != NULL) {
        x->snd_buf_ptr = x->next_snd_buf_ptr;
        x->next_snd_buf_ptr = NULL;
        
        #ifdef DEBUG
            post("%s: sound buffer pointer updated", OBJECT_NAME);
        #endif /* DEBUG */
    }
    if (x->next_win_buf_ptr != NULL) {
        x->win_buf_ptr = x->next_win_buf_ptr;
        x->next_win_buf_ptr = NULL;
        
        #ifdef DEBUG
            post("%s: window buffer pointer updated", OBJECT_NAME);
        #endif /* DEBUG */
    }
    
    snd_object = buffer_ref_getobject(x->snd_buf_ptr);
    win_object = buffer_ref_getobject(x->win_buf_ptr);
    
    /* should input variables be at audio or control rate ? */
    
    x->grain_freq = x->grain_freq_connected ? in_freq : x->next_grain_freq;
    
    // temporarily stash here as milliseconds
    x->grain_pos_start = x->grain_pos_start_connected ? in_pos_start : x->next_grain_pos_start;
    
    x->grain_pitch = x->grain_pitch_connected ? in_pitch_mult : x->next_grain_pitch;
    
    x->grain_gain = x->grain_gain_connected ? in_gain_mult : x->next_grain_gain;
    
    /* compute dependent variables */
    
    // compute window buffer step size per vector sample
    x->win_step_size = (double)(buffer_getframecount(win_object)) * x->grain_freq * x->output_1oversr;
    if (x->win_step_size < 0.) x->win_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
    // compute sound buffer step size per vector sample
    x->snd_step_size = (double)(buffer_getsamplerate(snd_object)) * x->grain_pitch * x->output_1oversr;
    if (x->snd_step_size < 0.) x->snd_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
    // compute amount of sound file for grain
    x->grain_sound_length = x->snd_step_size * ( 1000. / x->grain_freq );
    if (x->grain_sound_length < 0.) x->grain_sound_length *= -1.; // needs to be positive to prevent buffer overruns
    
    // update direction option
    x->grain_direction = x->next_grain_direction;
    
    if (x->grain_direction == FORWARD_GRAINS) {	// if forward...
        x->grain_pos_start = x->grain_pos_start * buffer_getmillisamplerate(snd_object);
        x->curr_snd_pos = x->grain_pos_start - x->snd_step_size;
    } else {	// if reverse...
        x->grain_pos_start = (x->grain_pos_start + x->grain_sound_length) * buffer_getmillisamplerate(snd_object);
        x->curr_snd_pos = x->grain_pos_start + x->snd_step_size;
    }
    
    x->curr_win_pos = 0.0;
    
    // reset history
    x->curr_count_samp = -1;
    
    #ifdef DEBUG
        post("%s: beginning of grain", OBJECT_NAME);
    #endif /* DEBUG */
    
    return;
    
}


/********************************************************************************
void grainstream_setsnd(t_index *x, t_symbol *s)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
description:	links buffer holding the grain sound source 
returns:		nothing
********************************************************************************/
void grainstream_setsnd(t_grainstream *x, t_symbol *s)
{
    t_buffer_ref *b = buffer_ref_new((t_object*)x, s);
    
    if (buffer_ref_exists(b)) {
        t_buffer_obj	*b_object = buffer_ref_getobject(b);
        
        if (buffer_getchannelcount(b_object) != 1) {
			error("%s: buffer~ > %s < must be mono", OBJECT_NAME, s->s_name);
			x->next_snd_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->snd_buf_ptr == NULL) { // if first buffer make current buffer
				x->snd_sym = s;
				x->snd_buf_ptr = b;
				//x->snd_last_out = 0.0;	//removed 2005.02.03
				
				#ifdef DEBUG
					post("%s: current sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->snd_sym = s;
				x->next_snd_buf_ptr = b;
				//x->snd_buf_length = b->b_frames;	//removed 2002.07.11
				//x->snd_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					post("%s: next sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		error("%s: no buffer~ * %s * found", OBJECT_NAME, s->s_name);
		x->next_snd_buf_ptr = NULL;
	}
}

/********************************************************************************
void grainstream_setwin(t_grainstream *x, t_symbol *s)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
description:	links buffer holding the grain window 
returns:		nothing
********************************************************************************/
void grainstream_setwin(t_grainstream *x, t_symbol *s)
{
    t_buffer_ref *b = buffer_ref_new((t_object*)x, s);
    
    if (buffer_ref_exists(b)) {
        t_buffer_obj	*b_object = buffer_ref_getobject(b);
        
        if (buffer_getchannelcount(b_object) != 1) {
			error("%s: buffer~ > %s < must be mono", OBJECT_NAME, s->s_name);
			x->next_win_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->win_buf_ptr == NULL) { // if first buffer make current buffer
				x->win_sym = s;
				x->win_buf_ptr = b;
				//x->win_last_out = 0.0;	//removed 2005.02.03
				x->win_last_index = buffer_getframecount(b_object);
				
				#ifdef DEBUG
					post("%s: current window set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->win_sym = s;
				x->next_win_buf_ptr = b;
				//x->win_buf_length = b->b_frames;	//removed 2002.07.11
				//x->win_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					post("%s: next window set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		error("%s: no buffer~ > %s < found", OBJECT_NAME, s->s_name);
		x->next_win_buf_ptr = NULL;
	}
}

/********************************************************************************
void grainstream_float(t_grainstream *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; inlet 4 sets 
		"next_grain_pitch" variable; left inlet generates error message in max 
		window
returns:		nothing
********************************************************************************/
void grainstream_float(t_grainstream *x, double f)
{
    switch (x->x_obj.z_in) {
        case 0:
            x->next_grain_freq = f;
            break;
        case 1:
            x->next_grain_pos_start = f;
            break;
        case 2:
            x->next_grain_pitch = f;
            break;
        case 3:
            x->next_grain_gain = f;
            break;
        default:
            post("%s: inlet does not accept floats", OBJECT_NAME);
            break;
    }
}

/********************************************************************************
void grainstream_int(t_grainstream *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles int sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; inlet 4 sets 
		"next_grain_pitch" variable; left inlet generates error message in max 
		window
returns:		nothing
********************************************************************************/
void grainstream_int(t_grainstream *x, long l)
{
    switch (x->x_obj.z_in) {
        case 0:
            x->next_grain_freq = (double)l;
            break;
        case 1:
            x->next_grain_pos_start = (double)l;
            break;
        case 2:
            x->next_grain_pitch = (double)l;
            break;
        case 3:
            x->next_grain_gain = (double)l;
            break;
        default:
            post("%s: inlet does not accept ints", OBJECT_NAME);
            break;
    }
}

/********************************************************************************
void grainstream_sndInterp(t_grainstream *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "sndInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the sound
		buffer; default is on
returns:		nothing
********************************************************************************/
void grainstream_sndInterp(t_grainstream *x, long l)
{
	if (l == INTERP_OFF) {
		x->snd_interp = INTERP_OFF;
		#ifdef DEBUG
			post("%s: sndInterp is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->snd_interp = INTERP_ON;
		#ifdef DEBUG
			post("%s: sndInterp is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		error("%s: sndInterp message was not understood", OBJECT_NAME);
	}
}

/********************************************************************************
void grainstream_winInterp(t_grainstream *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "winInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the window
		buffer; default is off
returns:		nothing
********************************************************************************/
void grainstream_winInterp(t_grainstream *x, long l)
{
	if (l == INTERP_OFF) {
		x->win_interp = INTERP_OFF;
		#ifdef DEBUG
			post("%s: winInterp is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->win_interp = INTERP_ON;
		#ifdef DEBUG
			post("%s: winInterp is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		error("%s: winInterp was not understood", OBJECT_NAME);
	}
}

/********************************************************************************
void grainstream_reverse(t_grainstream *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "reverse" message is received; allows user 
		to define whether sound is played forward or reverse; default is forward
returns:		nothing
********************************************************************************/
void grainstream_reverse(t_grainstream *x, long l)
{
	if (l == REVERSE_GRAINS) {
		x->next_grain_direction = REVERSE_GRAINS;
		#ifdef DEBUG
			post("%s: reverse is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == FORWARD_GRAINS) {
		x->next_grain_direction = FORWARD_GRAINS;
		#ifdef DEBUG
			post("%s: reverse is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		error("%s: reverse was not understood", OBJECT_NAME);
	}
	
}


/********************************************************************************
void grainstream_assist(t_grainstream *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void grainstream_assist(t_grainstream *x, t_object *b, long msg, long arg, char *s)
{
    if (msg==ASSIST_INLET) {
        switch (arg) {
            case 0:
                strcpy(s, "(signal/float) frequency in Hz");
                break;
            case 1:
                strcpy(s, "(signal/float) sound buffer offset in ms");
                break;
            case 2:
                strcpy(s, "(signal/float) sample increment, 1.0 = unchanged");
                break;
            case 3:
                strcpy(s, "(signal/float) gain multiplier, 1.0 = unchanged");
                break;
        }
    } else if (msg==ASSIST_OUTLET) {
        switch (arg) {
            case 0:
                strcpy(s, "(signal) audio channel 1");
                break;
            case 1:
                strcpy(s, "(signal) audio channel 2 COMING SOON");
                break;
            case 2:
                strcpy(s, "(signal) sample count");
                break;
        }
    }
	
	#ifdef DEBUG
		post("%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void grainstream_getinfo(t_grainstream *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void grainstream_getinfo(t_grainstream *x)
{
	post("%s object by Nathan Wolek", OBJECT_NAME);
	post("Last updated on %s - www.nathanwolek.com", __DATE__);
}


/********************************************************************************
double mcLinearInterp(float *in_array, long index_i, double index_frac, 
		long in_size, short in_chans)

inputs:			*in_array -- name of array of input values
				index_i -- index value of sample, specific channel within interleaved frame
				index_frac -- fractional portion of index value for interp
				in_size -- size of input buffer to perform wrapping
				in_chans -- number of channels in input buffer
description:	performs linear interpolation on an input array and to return 
	value of a fractional sample location
returns:		interpolated output
********************************************************************************/
double mcLinearInterp(float *in_array, long index_i, double index_frac, long in_size, short in_chans)
{
	double out, sample1, sample2;
	long index_iP1 = index_i + in_chans;		// corresponding sample in next frame
	
	// make sure that index_iP1 is not out of range
	while (index_iP1 >= in_size) index_iP1 -= in_size;
	
	// get samples
	sample1 = (double)in_array[index_i];
	sample2 = (double)in_array[index_iP1];
	
	//linear interp formula
	out = sample1 + index_frac * (sample2 - sample1);
	
	return out;
}