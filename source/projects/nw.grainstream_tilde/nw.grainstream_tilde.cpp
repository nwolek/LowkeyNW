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

#include "c74_msp.h"

using namespace c74::max;

//#define DEBUG			//enable debugging messages

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
void grainstream_perform64zero(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainstream_perform64(t_grainstream *x, t_object *dsp64, double **ins, long numins, double **outs,long numouts, long vectorsize, long flags, void *userparam);
void grainstream_initGrain(t_grainstream *x, float in_pos_start, float in_pitch_mult, float in_length, float in_gain_mult);
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
        object_post((t_object*)x, "adding 64 bit perform method");
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
            object_post((t_object*)x, "output is being computed");
        #endif /* DEBUG */
        dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)grainstream_perform64, 0, NULL);
    } else {
        #ifdef DEBUG
            object_post((t_object*)x, "no output computed");
        #endif /* DEBUG */
    }
    
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
	for (long channel = 0; channel<numouts; ++channel) {
		for (long i = 0; i<vectorsize; ++i)
			outs[channel][i] = 0.0;
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
    double *in_freq = ins[0];
    double *in_sound_start = ins[1];
    double *in_sample_increment = ins[2];
    double *in_gain = ins[3];
    double *out_signal = outs[0];
    double *out_signal2 = outs[1];
    double *out_sample_count = outs[2];
    
    // local vars for snd and win buffer
    t_buffer_obj *snd_object, *win_object;
    float *tab_s, *tab_w;
    double snd_out, snd_out2, win_out;
    long size_s, chan_s, size_w;
    
    // local vars for object vars and while loop
    double index_s, index_w, temp_index_frac;
    long n, count_samp, temp_index_int, temp_index_int_times_chan;
    double s_step_size, w_step_size, w_last_index, g_gain;
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
    chan_s = buffer_getchannelcount(snd_object);
    
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
        
        // if we made it here, then we actually start counting
        ++count_samp;
        
        // advance sound index
        if (g_direction == FORWARD_GRAINS) {
            index_s += s_step_size;     // addition
        } else {
            index_s -= s_step_size;     // subtract
        }
        
        // wrap to make index in bounds
        while (index_s < 0.)
            index_s += size_s;
        while (index_s >= size_s)
            index_s -= size_s;
        
        // WINDOW OUT
        
        // compute temporary vars for interpolation
        temp_index_int = (long)(index_w); // integer portion of index
        temp_index_frac = index_w - (double)temp_index_int; // fractional portion of index
        
        // get value from win buffer samples
        if (interp_w == INTERP_ON) {
            win_out = mcLinearInterp(tab_w, temp_index_int, temp_index_frac, size_w, 1);
        } else {
            win_out = tab_w[temp_index_int];
        }
        
        // SOUND OUT
        
        // compute temporary vars for interpolation
        temp_index_int = (long)(index_s); // integer portion of index
        temp_index_frac = index_s - (double)temp_index_int; // fractional portion of index
        temp_index_int_times_chan = temp_index_int * chan_s;
        
        // get value from snd buffer samples
        if (interp_s == INTERP_ON) {
            snd_out = mcLinearInterp(tab_s, temp_index_int_times_chan, temp_index_frac, size_s, chan_s);
            snd_out2 = (chan_s == 2) ?
                mcLinearInterp(tab_s, temp_index_int_times_chan + 1, temp_index_frac, size_s, chan_s) :
                snd_out;
        } else {
            snd_out = tab_s[temp_index_int_times_chan];
            snd_out2 = (chan_s == 2) ?
                tab_s[temp_index_int_times_chan + 1] :
                snd_out;
        }
        
        // OUTLETS
        
        *out_signal = snd_out * win_out * g_gain;
        *out_signal2 = snd_out2 * win_out * g_gain;
        *out_sample_count = (double)count_samp;
        
        // update vars for last output
        w_last_index = index_w;
    
advance_pointers:
        // advance all pointers
        ++in_freq, ++in_sound_start, ++in_sample_increment, ++in_gain;
        ++out_signal, ++out_signal2, ++out_sample_count;
    }

    // update object history for next vector
    x->curr_snd_pos = index_s;
    x->curr_win_pos = index_w;
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
        object_post((t_object*)x, "initializing grain");
    #endif /* DEBUG */
    
    /* should the buffers be updated ? */
    
    t_buffer_obj	*snd_object;
    t_buffer_obj	*win_object;
    
    if (x->next_snd_buf_ptr != NULL) {
        x->snd_buf_ptr = x->next_snd_buf_ptr;
        x->next_snd_buf_ptr = NULL;
        
        #ifdef DEBUG
            object_post((t_object*)x, "sound buffer pointer updated");
        #endif /* DEBUG */
    }
    if (x->next_win_buf_ptr != NULL) {
        x->win_buf_ptr = x->next_win_buf_ptr;
        x->next_win_buf_ptr = NULL;
        
        #ifdef DEBUG
            object_post((t_object*)x, "window buffer pointer updated");
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
    
    // grain_freq must be positive and above 0.01 Hz or 1.66 min duration
    if (x->grain_freq < 0.) x->grain_freq *= -1;
    if (x->grain_freq < 0.01) x->grain_freq = 0.01;
    x->grain_length = 1000. / x->grain_freq;
    
    // compute window buffer step size per vector sample
    x->win_step_size = (double)(buffer_getframecount(win_object)) * x->grain_freq * x->output_1oversr;
    if (x->win_step_size < 0.) x->win_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
    // compute sound buffer step size per vector sample
    x->snd_step_size = x->grain_pitch * buffer_getsamplerate(snd_object) * x->output_1oversr;
    if (x->snd_step_size < 0.) x->snd_step_size *= -1.; // needs to be positive to prevent buffer overruns
    
    // compute amount of sound file for grain
    x->grain_sound_length = x->grain_length * x->grain_pitch;
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
        object_post((t_object*)x, "beginning of grain");
        object_post((t_object*)x, "win step size = %f samps", x->win_step_size);
        object_post((t_object*)x, "snd step size = %f samps", x->snd_step_size);
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
        
        if (buffer_getchannelcount(b_object) > 2) {
			object_error((t_object*)x, "buffer~ > %s < must be mono or stereo", s->s_name);
			x->next_snd_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->snd_buf_ptr == NULL) { // if first buffer make current buffer
				x->snd_sym = s;
				x->snd_buf_ptr = b;
				//x->snd_last_out = 0.0;	//removed 2005.02.03
				
				#ifdef DEBUG
					object_post((t_object*)x, "current sound set to buffer~ > %s <", s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->snd_sym = s;
				x->next_snd_buf_ptr = b;
				//x->snd_buf_length = b->b_frames;	//removed 2002.07.11
				//x->snd_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					object_post((t_object*)x, "next sound set to buffer~ > %s <", s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		object_error((t_object*)x, "no buffer~ * %s * found", s->s_name);
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
			object_error((t_object*)x, "buffer~ > %s < must be mono", s->s_name);
			x->next_win_buf_ptr = NULL;		//added 2002.07.15
		} else {
			if (x->win_buf_ptr == NULL) { // if first buffer make current buffer
				x->win_sym = s;
				x->win_buf_ptr = b;
				//x->win_last_out = 0.0;	//removed 2005.02.03
				x->win_last_index = buffer_getframecount(b_object);
				
				#ifdef DEBUG
					object_post((t_object*)x, "current window set to buffer~ > %s <", s->s_name);
				#endif /* DEBUG */
			} else { // else defer to next buffer
				x->win_sym = s;
				x->next_win_buf_ptr = b;
				//x->win_buf_length = b->b_frames;	//removed 2002.07.11
				//x->win_last_out = 0.0;		//removed 2002.07.24
				
				#ifdef DEBUG
					object_post((t_object*)x, "next window set to buffer~ > %s <", s->s_name);
				#endif /* DEBUG */
			}
		}
	} else {
		object_error((t_object*)x, "no buffer~ > %s < found", s->s_name);
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
            object_post((t_object*)x, "that inlet does not accept floats");
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
            object_post((t_object*)x, "that inlet does not accept ints");
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
			object_post((t_object*)x, "sndInterp is set to off");
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->snd_interp = INTERP_ON;
		#ifdef DEBUG
			object_post((t_object*)x, "sndInterp is set to on");
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "sndInterp message was not understood");
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
			object_post((t_object*)x, "winInterp is set to off");
		#endif // DEBUG //
	} else if (l == INTERP_ON) {
		x->win_interp = INTERP_ON;
		#ifdef DEBUG
			object_post((t_object*)x, "winInterp is set to on");
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "winInterp was not understood");
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
			object_post((t_object*)x, "reverse is set to on");
		#endif // DEBUG //
	} else if (l == FORWARD_GRAINS) {
		x->next_grain_direction = FORWARD_GRAINS;
		#ifdef DEBUG
			object_post((t_object*)x, "reverse is set to off");
		#endif // DEBUG //
	} else {
		object_error((t_object*)x, "reverse was not understood");
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
		object_post((t_object*)x, "assist message displayed");
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
	while (index_iP1 >= in_size * in_chans) index_iP1 -= in_size;
	
	// get samples
	sample1 = (double)in_array[index_i];
	sample2 = (double)in_array[index_iP1];
	
	//linear interp formula
	out = sample1 + index_frac * (sample2 - sample1);
	
	return out;
}