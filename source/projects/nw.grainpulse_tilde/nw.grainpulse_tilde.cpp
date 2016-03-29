/*
** nw.grainpulse~.c
**
** MSP object
** sends out a single grains when it receives a pulse
** 2001/08/29 started by Nathan Wolek
**
** Copyright © 2002,2014 by Nathan Wolek
** License: http://opensource.org/licenses/BSD-3-Clause
**
*/

#include "c74_msp.h"

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"nw.grainpulse~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for direction flag */
#define FORWARD_GRAINS		0
#define REVERSE_GRAINS		1

/* for interpolation flag */
#define INTERP_OFF			0
#define INTERP_ON			1

/* for overflow flag, added 2002.10.28 */
#define OVERFLOW_OFF		0
#define OVERFLOW_ON			1

static t_class *grainpulse_class;		// required global pointing to this class

typedef struct _grainpulse
{
	t_pxobject x_obj;					// <--
	// sound buffer info
	t_symbol *snd_sym;
	t_buffer_ref *snd_buf_ptr;
	t_buffer_ref *next_snd_buf_ptr;
	//double snd_last_out; removed 2005.01.25
	//long snd_buf_length;	//removed 2002.07.11
	short snd_interp;
	// window buffer info
	t_symbol *win_sym;
	t_buffer_ref *win_buf_ptr;
	t_buffer_ref *next_win_buf_ptr;
	//double win_last_out; removed 2005.01.25
	//long win_buf_length;	//removed 2002.07.11
	short win_interp;
	// current grain info
	double grain_pos_start;	// in samples
	double grain_length;	// in milliseconds
	double grain_pitch;		// as multiplier
	double grain_gain;		// linear gain mult; add 2008.04.22
	double grain_sound_length;	// in milliseconds
	double win_step_size;	// in samples
	double snd_step_size;	// in samples
	double curr_win_pos;	// in samples
	double curr_snd_pos;	// in samples
	short grain_direction;	// forward or reverse
	short overflow_status;	//added 2002.10.28, only used while grain is sounding // <--
			//will produce false positives otherwise
	// defered grain info at control rate
	double next_grain_pos_start;	// in milliseconds
	double next_grain_length;		// in milliseconds
	double next_grain_pitch;		// as multiplier
	double next_grain_gain;			// linear gain mult; add 2008.04.22
	short next_grain_direction;		// forward or reverse
	// signal or control grain info
	short grain_pos_start_connected;	// <--
	short grain_length_connected;		// <--
	short grain_pitch_connected;		// <--
	short grain_gain_connected;			// add 2008.04.22
	// grain tracking info
	short grain_stage;
	long curr_count_samp;
    float last_pulse_in;				// <--
	double output_sr;					// <--
	double output_1oversr;				// <--
	//bang on init outlet, added 2004.03.10
	void *out_reportoninit;
	t_symbol *ts_offset;
	t_symbol *ts_dur;
	t_symbol *ts_pscale;
} t_grainpulse;

void *grainpulse_new(t_symbol *snd, t_symbol *win);
void grainpulse_perform64zero(t_grainpulse *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainpulse_perform64(t_grainpulse *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainpulse_initGrain(t_grainpulse *x, float in_pos_start, float in_length, 
		float in_pitch_mult, float in_gain_mult);
void grainpulse_reportoninit(t_grainpulse *x, t_symbol *s, short argc, t_atom argv);
void grainpulse_dsp64(t_grainpulse *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void grainpulse_setsnd(t_grainpulse *x, t_symbol *s);
void grainpulse_setwin(t_grainpulse *x, t_symbol *s);
void grainpulse_float(t_grainpulse *x, double f);
void grainpulse_int(t_grainpulse *x, long l);
void grainpulse_sndInterp(t_grainpulse *x, long l);
void grainpulse_winInterp(t_grainpulse *x, long l);
void grainpulse_reverse(t_grainpulse *x, long l);
void grainpulse_assist(t_grainpulse *x, t_object *b, long msg, long arg, char *s);
void grainpulse_getinfo(t_grainpulse *x);
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
    
    c = class_new(OBJECT_NAME, (method)grainpulse_new, (method)dsp_free,
			(short)sizeof(t_grainpulse), 0L, A_SYM, A_SYM, 0);
    class_dspinit(c); // add standard functions to class
	
	/* bind method "grainpulse_setsnd" to the 'setSound' message */
	class_addmethod(c, (method)grainpulse_setsnd, "setSound", A_SYM, 0);
	
	/* bind method "grainpulse_setwin" to the 'setWin' message */
	class_addmethod(c, (method)grainpulse_setwin, "setWin", A_SYM, 0);
	
	/* bind method "grainpulse_float" to incoming floats */
	class_addmethod(c, (method)grainpulse_float, "float", A_FLOAT, 0);
	
	/* bind method "grainpulse_int" to incoming ints */
	class_addmethod(c, (method)grainpulse_int, "int", A_LONG, 0);
	
	/* bind method "grainpulse_reverse" to the direction message */
	class_addmethod(c, (method)grainpulse_reverse, "reverse", A_LONG, 0);
	
	/* bind method "grainpulse_sndInterp" to the sndInterp message */
	class_addmethod(c, (method)grainpulse_sndInterp, "sndInterp", A_LONG, 0);
	
	/* bind method "grainpulse_winInterp" to the winInterp message */
	class_addmethod(c, (method)grainpulse_winInterp, "winInterp", A_LONG, 0);
	
	/* bind method "grainpulse_assist" to the assistance message */
	class_addmethod(c, (method)grainpulse_assist, "assist", A_CANT, 0);
	
	/* bind method "grainpulse_getinfo" to the getinfo message */
	class_addmethod(c, (method)grainpulse_getinfo, "getinfo", A_NOTHING, 0);
    
    /* bind method "grainpulse_dsp64" to the dsp64 message */
    class_addmethod(c, (method)grainpulse_dsp64, "dsp64", A_CANT, 0);
	
    class_register(C74_CLASS_BOX, c); // register the class w max
    grainpulse_class = c;
    
    /* needed for 'buffer~' work, checks for validity of buffer specified */
    ps_buffer = gensym("buffer~");
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: main function was called", OBJECT_NAME);
    #endif /* DEBUG */
    
    return 0;
}

/********************************************************************************
void *grainpulse_new(double initial_pos)

inputs:			*snd		-- name of buffer holding sound
				*win		-- name of buffer holding window
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; sets variables and buffers
returns:		nothing
********************************************************************************/
void *grainpulse_new(t_symbol *snd, t_symbol *win)
{
	t_grainpulse *x = (t_grainpulse *) object_alloc((t_class*) grainpulse_class);
	dsp_setup((t_pxobject *)x, 5);					// five inlets; change 2008.04.22
    outlet_new((t_pxobject *)x, "signal");			// overflow outlet
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
	x->grain_pos_start = x->next_grain_pos_start = 0.0;
	x->grain_length = x->next_grain_length = 50.0;
	x->grain_pitch = x->next_grain_pitch = 1.0;
	x->grain_gain = x->next_grain_gain = 1.0;
	x->win_step_size = x->snd_step_size = 0.0;
	x->curr_snd_pos = 0.0;
	x->curr_win_pos = 0.0;
	x->last_pulse_in = 0.0;
    x->curr_count_samp = -1;
	
	/* setup t_symbols for output messages (saves overhead)*/
	x->ts_offset = gensym("offset");
	x->ts_dur = gensym("dur");
	x->ts_pscale = gensym("pscale");
	
	/* set flags to defaults */
	x->snd_interp = INTERP_ON;
	x->win_interp = INTERP_ON;
	x->grain_direction = x->next_grain_direction = FORWARD_GRAINS;
	
	x->x_obj.z_misc = Z_NO_INPLACE;
	
	/* return a pointer to the new object */
	return (x);
}


/********************************************************************************
 void grainpulse_dsp64()
 
 inputs:     x		-- pointer to this object
 dsp64		-- signal chain to which object belongs
 count	-- array detailing number of signals attached to each inlet
 samplerate -- number of samples per second
 maxvectorsize -- sample frames per vector of audio
 flags --
 description:	called when 64 bit DSP call chain is built; adds object to signal flow
 returns:		nothing
 ********************************************************************************/
void grainpulse_dsp64(t_grainpulse *x, t_object *dsp64, short *count, double samplerate,
                        long maxvectorsize, long flags)
{
    
    #ifdef DEBUG
        object_post((t_object*)x, "%s: adding 64 bit perform method", OBJECT_NAME);
    #endif /* DEBUG */
    
    /* set buffers */
    grainpulse_setsnd(x, x->snd_sym);
    grainpulse_setwin(x, x->win_sym);
    
    /* test inlets for signal data */
    x->grain_pos_start_connected = count[1];
    x->grain_length_connected = count[2];
    x->grain_pitch_connected = count[3];
    x->grain_gain_connected = count[4];
    
    // grab sample rate
    x->output_sr = samplerate;
    x->output_1oversr = 1.0 / x->output_sr;
    
    // set overflow status
    x->overflow_status = OVERFLOW_OFF;
    
    if (count[5] && count[0]) {	// if input and output connected..
        #ifdef DEBUG
            object_post((t_object*)x, "%s: output is being computed", OBJECT_NAME);
        #endif /* DEBUG */
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)grainpulse_perform64, 0, NULL);
    } else {					// if not...
        #ifdef DEBUG
            object_post((t_object*)x, "%s: no output computed", OBJECT_NAME);
        #endif /* DEBUG */
    }
    
}


/********************************************************************************
 void *grainpulse_perform64zero()
 
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
void grainpulse_perform64zero(t_grainpulse *x, t_object *dsp64, double **ins, long numins, double **outs,
                                long numouts, long vectorsize, long flags, void *userparam)
{
	for (long channel = 0; channel<numouts; ++channel) {
		for (long i = 0; i<vectorsize; ++i)
			outs[channel][i] = 0.0;
	}
}

/********************************************************************************
 void *grainpulse_perform64()
 
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
void grainpulse_perform64(t_grainpulse *x, t_object *dsp64, double **ins, long numins, double **outs,
                            long numouts, long vectorsize, long flags, void *userparam)
{
    // local vars outlets and inlets
    double *in_pulse = ins[0];
    double *in_sound_start = ins[1];
    double *in_dur = ins[2];
    double *in_sample_increment = ins[3];
    double *in_gain = ins[4];
    double *out_signal = outs[0];
    double *out_signal2 = outs[1];
    double *out_sample_count = outs[2];
    double *out_overflow = outs[3];
    
    // local vars for snd and win buffer
    t_buffer_obj *snd_object, *win_object;
    float *tab_s, *tab_w;
    double snd_out, win_out;
    long size_s, size_w;
    
    // local vars for object vars and while loop
    double index_s, index_w, temp_index_frac;
    long n, count_samp, temp_index_int;
    double s_step_size, w_step_size, g_gain;
    short interp_s, interp_w, g_direction, of_status;
    float last_pulse;
    
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
    index_w = x->curr_win_pos;
    s_step_size = x->snd_step_size;
    w_step_size = x->win_step_size;
    
    // get grain options
    interp_s = x->snd_interp;
    interp_w = x->win_interp;
    g_gain = x->grain_gain;
    g_direction = x->grain_direction;
    of_status = x->overflow_status;
    
    // get history from last vector
    last_pulse = x->last_pulse_in;
    count_samp = x->curr_count_samp;
    
    n = vectorsize;
    while(n--)
    {
        // advance window index
        index_w += w_step_size;
        // and if we exceed the window size, stop producing grain
        if (index_w > size_w) count_samp = -1;
        
        // should we start a grain ?
        if (count_samp == -1) { // if sample count is -1...
            if (last_pulse == 0.0 && *in_pulse == 1.0) { // if pulse begins...
                buffer_unlocksamples(snd_object);
                buffer_unlocksamples(win_object);
                
                grainpulse_initGrain(x, *in_sound_start, *in_dur, *in_sample_increment, *in_gain);
                
                // get snd buffer info
                snd_object = buffer_ref_getobject(x->snd_buf_ptr);
                tab_s = buffer_locksamples(snd_object);
                if (!tab_s)	{	// buffer samples were not accessible
                    *out_signal = 0.0;
                    *out_signal2 = 0.0;
                    *out_overflow = 0.0;
                    *out_sample_count = (double)count_samp;
                    last_pulse = *in_pulse;
                    goto advance_pointers;
                }
                size_s = buffer_getframecount(snd_object);
                
                // get win buffer info
                win_object = buffer_ref_getobject(x->win_buf_ptr);
                tab_w = buffer_locksamples(win_object);
                if (!tab_w)	{	// buffer samples were not accessible
                    *out_signal = 0.0;
                    *out_signal2 = 0.0;
                    *out_overflow = 0.0;
                    *out_sample_count = (double)count_samp;
                    last_pulse = *in_pulse;
                    goto advance_pointers;
                }
                size_w = buffer_getframecount(win_object);
                
                // get snd and win index info
                index_s = x->curr_snd_pos;
                index_w = x->curr_win_pos;
                s_step_size = x->snd_step_size;
                w_step_size = x->win_step_size;
                
                // get grain options
                interp_s = x->snd_interp;
                interp_w = x->win_interp;
                g_gain = x->grain_gain;
                g_direction = x->grain_direction;
                /*** of_status = x->overflow_status; ***/
                
                // get history from last vector
                last_pulse = x->last_pulse_in;
                count_samp = x->curr_count_samp;
                
                // BUT this stays off until duty cycle ends
                of_status = OVERFLOW_OFF;
                
            } else { // if not...
                *out_signal = 0.0;
                *out_signal2 = 0.0;
                *out_overflow = 0.0;
                *out_sample_count = (double)count_samp;
                last_pulse = *in_pulse;
                goto advance_pointers;
            }
        }

        // pulse tracking for overflow
        if (!of_status) {
            if (last_pulse == 1.0 && *in_pulse == 0.0) { // if grain on & pulse ends...
                of_status = OVERFLOW_ON;	//start overflowing
            }
        }
        
        // if we made it here, then we will actually start counting
        count_samp++;
        
        // advance sound index
        if (g_direction == FORWARD_GRAINS) {
            index_s += s_step_size;     // addition
        } else {	// if REVERSE_GRAINS
            index_s -= s_step_size;		// subtract
        }
        
        // wrap sound index if not within bounds
        while (index_s < 0.0)
            index_s += size_s;
        while (index_s >= size_s)
            index_s -= size_s;
        
        // WINDOW OUT
        
        // compute temporary vars for interpolation
        temp_index_int = (long)(index_w); // integer portion of index
        temp_index_frac = index_w - (double)temp_index_int; // fractional portion of index
        
        // get value from the win buffer samples
        if (interp_w == INTERP_ON) {
            win_out = mcLinearInterp(tab_w, temp_index_int, temp_index_frac, size_w, 1);
        } else {	// if INTERP_OFF
            win_out = tab_w[temp_index_int];
        }
        
        // SOUND OUT
        
        // compute temporary vars for interpolation
        temp_index_int = (long)(index_s); // integer portion of index
        temp_index_frac = index_s - (double)temp_index_int; // fractional portion of index
        
        // get value from the snd buffer samples
        if (interp_s == INTERP_ON) {
            snd_out = mcLinearInterp(tab_s, temp_index_int, temp_index_frac, size_s, 1);
        } else {	// if INTERP_OFF
            snd_out = tab_s[temp_index_int];
        }
        
        // OUTLETS
        
        // multiply snd_out by win_out by gain value
        *out_signal = snd_out * win_out * g_gain;
        *out_signal2 = 0.;
        
        if (of_status) {
            *out_overflow = *in_pulse;
        } else {
            *out_overflow = 0.0;
        }
        
        *out_sample_count = (double)count_samp;
        
        // update vars for last output
        last_pulse = *in_pulse;
    
advance_pointers:
        // advance all pointers
        ++in_pulse, ++in_sound_start, ++in_dur, ++in_sample_increment, ++in_gain;
        ++out_signal, ++out_signal2, ++out_overflow, ++out_sample_count;
    }

    // update object history for next vector
    x->curr_snd_pos = index_s;
    x->curr_win_pos = index_w;
    x->last_pulse_in = last_pulse;
    x->overflow_status = of_status;
    x->curr_count_samp = count_samp;

    buffer_unlocksamples(snd_object);
    buffer_unlocksamples(win_object);
    return;

    // alternate blank output
zero:
    n = vectorsize;
    while(n--)
    {
        *out_signal++ = 0.;
        *out_signal2++ = 0.;
        *out_sample_count++ = -1.;
        *out_overflow++ = -1.;
    }

out:
    return;

}

/********************************************************************************
void grainpulse_initGrain(t_grainpulse *x, float in_pos_start, float in_length,
		float in_pitch_mult, float in_gain_mult)

inputs:			x					-- pointer to this object
				in_pos_start		-- offset within sampled buffer
				in_length			-- length of grain
				in_pitch_mult		-- sample playback speed, 1 = normal
				in_gain_mult		-- scales gain output, 1 = no change
description:	initializes grain vars; called from perform method when pulse is 
		received
returns:		nothing 
********************************************************************************/
void grainpulse_initGrain(t_grainpulse *x, float in_pos_start, float in_length, 
		float in_pitch_mult, float in_gain_mult)
{
	#ifdef DEBUG
		object_post((t_object*)x, "%s: initializing grain", OBJECT_NAME);
	#endif /* DEBUG */
    
    t_buffer_obj	*snd_object;
    t_buffer_obj	*win_object;
	
	if (x->next_snd_buf_ptr != NULL) {
		x->snd_buf_ptr = x->next_snd_buf_ptr;
		x->next_snd_buf_ptr = NULL;
		
		#ifdef DEBUG
			object_post((t_object*)x, "%s: sound buffer pointer updated", OBJECT_NAME);
		#endif /* DEBUG */
	}
	if (x->next_win_buf_ptr != NULL) {
		x->win_buf_ptr = x->next_win_buf_ptr;
		x->next_win_buf_ptr = NULL;
		
		#ifdef DEBUG
			object_post((t_object*)x, "%s: window buffer pointer updated", OBJECT_NAME);
		#endif /* DEBUG */
	}
	
	snd_object = buffer_ref_getobject(x->snd_buf_ptr);
	win_object = buffer_ref_getobject(x->win_buf_ptr);
	
    /* should input variables be at audio or control rate ? */
    
    // temporarily stash here as milliseconds
    x->grain_pos_start = x->grain_pos_start_connected ? in_pos_start : x->next_grain_pos_start;
    
    x->grain_length = x->grain_length_connected ? in_length : x->next_grain_length;
    
    x->grain_pitch = x->grain_pitch_connected ? in_pitch_mult : x->next_grain_pitch;
    
    x->grain_gain = x->grain_gain_connected ? in_gain_mult : x->next_grain_gain;
    
    /* compute dependent variables */
    
	// compute amount of sound file for grain
	x->grain_sound_length = x->grain_length * x->grain_pitch;
    if (x->grain_sound_length < 0.) x->grain_sound_length *= -1.; // needs to be positive to prevent buffer overruns
	
	// compute window buffer step size per vector sample 
	x->win_step_size = (double)(buffer_getframecount(win_object)) / (x->grain_length * x->output_sr * 0.001);
    if (x->win_step_size < 0.) x->win_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
	// compute sound buffer step size per vector sample
	x->snd_step_size = x->grain_pitch * buffer_getsamplerate(snd_object) * x->output_1oversr;
    if (x->snd_step_size < 0.) x->snd_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
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
	
	// send report out at beginning of grain
	//defer(x, (void *)grainpulse_reportoninit,0L,0,0L);
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: beginning of grain", OBJECT_NAME);
		object_post((t_object*)x, "%s: win step size = %f samps", OBJECT_NAME, x->win_step_size);
		object_post((t_object*)x, "%s: snd step size = %f samps", OBJECT_NAME, x->snd_step_size);
	#endif /* DEBUG */
}

/********************************************************************************
void grainpulse_reportoninit(t_pulsesamp *x, t_symbol *s, short argc, t_atom argv)

inputs:			x		-- pointer to our object
description:	sends settings when grain is initialized; allows external settings
	to advance; extra arguments allow for use of defer method
returns:		nothing
********************************************************************************/
void grainpulse_reportoninit(t_grainpulse *x, t_symbol *s, short argc, t_atom argv)
{
	t_atom ta_msgvals[3];
	
	atom_setfloat(ta_msgvals, (float) x->grain_pos_start);
	atom_setfloat((ta_msgvals + 1), (float) x->grain_length);
	atom_setfloat((ta_msgvals + 2), (float) x->grain_pitch);
	
	if (sys_getdspstate()) {
		//report settings used in grain production
		outlet_anything(x->out_reportoninit, x->ts_offset, 1, ta_msgvals);
		outlet_anything(x->out_reportoninit, x->ts_dur, 1, (ta_msgvals + 1));
		outlet_anything(x->out_reportoninit, x->ts_pscale, 1, (ta_msgvals + 2));
	}
}

/********************************************************************************
void grainpulse_setsnd(t_index *x, t_symbol *s)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
description:	links buffer holding the grain sound source 
returns:		nothing
********************************************************************************/
void grainpulse_setsnd(t_grainpulse *x, t_symbol *s)
{
	t_buffer_ref *b = buffer_ref_new((t_object*)x, s);
	
    if (buffer_ref_exists(b)) {
        t_buffer_obj	*b_object = buffer_ref_getobject(b);
        
		if (buffer_getchannelcount(b_object) != 1) {
			object_error((t_object*)x, "%s: buffer~ > %s < must be mono", OBJECT_NAME, s->s_name);
			x->next_snd_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->snd_buf_ptr == NULL) { // if first buffer make current buffer
				x->snd_sym = s;
				x->snd_buf_ptr = b;
				//x->snd_last_out = 0.0; removed 2005.01.25
				
				#ifdef DEBUG
					object_post((t_object*)x, "%s: current sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->snd_sym = s;
				x->next_snd_buf_ptr = b;
				//x->snd_buf_length = b->b_frames;	//removed 2002.07.11
				//x->snd_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					object_post((t_object*)x, "%s: next sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		object_error((t_object*)x, "%s: no buffer~ * %s * found", OBJECT_NAME, s->s_name);
		x->next_snd_buf_ptr = NULL;
	}
}

/********************************************************************************
void grainpulse_setwin(t_grainpulse *x, t_symbol *s)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
description:	links buffer holding the grain window 
returns:		nothing
********************************************************************************/
void grainpulse_setwin(t_grainpulse *x, t_symbol *s)
{
    t_buffer_ref *b = buffer_ref_new((t_object*)x, s);
    
    if (buffer_ref_exists(b)) {
        t_buffer_obj	*b_object = buffer_ref_getobject(b);
        
        if (buffer_getchannelcount(b_object) != 1) {
			object_error((t_object*)x, "%s: buffer~ > %s < must be mono", OBJECT_NAME, s->s_name);
			x->next_win_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->win_buf_ptr == NULL) { // if first buffer make current buffer
				x->win_sym = s;
				x->win_buf_ptr = b;
				//x->win_last_out = 0.0; removed 2005.01.25
				
				/* set current win position to 1 more than length */
				x->curr_win_pos = (double)(buffer_getframecount(b_object)) + 1.0;
				
				#ifdef DEBUG
					object_post((t_object*)x, "%s: current window set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->win_sym = s;
				x->next_win_buf_ptr = b;
				//x->win_buf_length = b->b_frames;	//removed 2002.07.11
				//x->win_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					object_post((t_object*)x, "%s: next window set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		object_error((t_object*)x, "%s: no buffer~ > %s < found", OBJECT_NAME, s->s_name);
		x->next_win_buf_ptr = NULL;
	}
}

/********************************************************************************
void grainpulse_float(t_grainpulse *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; inlet 4 sets 
		"next_grain_pitch" variable; left inlet generates error message in max 
		window
returns:		nothing
********************************************************************************/
void grainpulse_float(t_grainpulse *x, double f)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		x->next_grain_pos_start = f;
	}
	else if (x->x_obj.z_in == 2) // if inlet 3
	{
		if (f > 0.0) {
			x->next_grain_length = f;
		} else {
			object_post((t_object*)x, "%s: grain length must be greater than zero", OBJECT_NAME);
		}
	}
	else if (x->x_obj.z_in == 3) // if inlet 4
	{
		x->next_grain_pitch = f;
	}
	else if (x->x_obj.z_in == 4) // if inlet 5; add 2008.04.22
	{
		x->next_grain_gain = f;
	}
	else if (x->x_obj.z_in == 0)
	{
		object_post((t_object*)x, "%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void grainpulse_int(t_grainpulse *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles int sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; inlet 4 sets 
		"next_grain_pitch" variable; left inlet generates error message in max 
		window
returns:		nothing
********************************************************************************/
void grainpulse_int(t_grainpulse *x, long l)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		x->next_grain_pos_start = (double) l;
	}
	else if (x->x_obj.z_in == 2) // if inlet 3
	{
		if (l > 0) {
			x->next_grain_length = (double) l;
		} else {
			object_post((t_object*)x, "%s: grain length must be greater than zero", OBJECT_NAME);
		}
	}
	else if (x->x_obj.z_in == 3) // if inlet 4
	{
		x->next_grain_pitch = (double) l;
	}
	else if (x->x_obj.z_in == 4) // if inlet 5
	{
		x->next_grain_gain = (double) l;
	}
	else if (x->x_obj.z_in == 0)
	{
		object_post((t_object*)x, "%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void grainpulse_sndInterp(t_grainpulse *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "sndInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the sound
		buffer; default is on
returns:		nothing
********************************************************************************/
void grainpulse_sndInterp(t_grainpulse *x, long l)
{
	if (l == INTERP_OFF) {
		x->snd_interp = INTERP_OFF;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: sndInterp is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->snd_interp = INTERP_ON;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: sndInterp is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "%s: sndInterp message was not understood", OBJECT_NAME);
	}
}

/********************************************************************************
void grainpulse_winInterp(t_grainpulse *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "winInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the window
		buffer; default is off
returns:		nothing
********************************************************************************/
void grainpulse_winInterp(t_grainpulse *x, long l)
{
	if (l == INTERP_OFF) {
		x->win_interp = INTERP_OFF;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: winInterp is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->win_interp = INTERP_ON;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: winInterp is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "%s: winInterp was not understood", OBJECT_NAME);
	}
}

/********************************************************************************
void grainpulse_reverse(t_grainpulse *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "reverse" message is received; allows user 
		to define whether sound is played forward or reverse; default is forward
returns:		nothing
********************************************************************************/
void grainpulse_reverse(t_grainpulse *x, long l)
{
	if (l == REVERSE_GRAINS) {
		x->next_grain_direction = REVERSE_GRAINS;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: reverse is set to on", OBJECT_NAME);
		#endif // DEBUG //
	} else if (l == FORWARD_GRAINS) {
		x->next_grain_direction = FORWARD_GRAINS;
		#ifdef DEBUG
			object_post((t_object*)x, "%s: reverse is set to off", OBJECT_NAME);
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "%s: reverse was not understood", OBJECT_NAME);
	}
	
}

/********************************************************************************
void grainpulse_assist(t_grainpulse *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void grainpulse_assist(t_grainpulse *x, t_object *b, long msg, long arg, char *s)
{
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) pulse starts grain production");
				break;
			case 1:
				strcpy(s, "(signal/float) sound buffer offset in ms");
				break;
			case 2:
				strcpy(s, "(signal/float) grain duration in ms");
				break;
			case 3:
				strcpy(s, "(signal/float) sample increment, 1.0 = unchanged");
				break;
			case 4:
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
            case 3:
                strcpy(s, "(signal) overflow");
                break;
		}
	}
	
	#ifdef DEBUG
		object_post((t_object*)x, "%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void grainpulse_getinfo(t_grainpulse *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void grainpulse_getinfo(t_grainpulse *x)
{
	object_post((t_object*)x, "%s object by Nathan Wolek", OBJECT_NAME);
	object_post((t_object*)x, "Last updated on %s - www.nathanwolek.com", __DATE__);
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