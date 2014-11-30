/*
** g_phase_stereo~.c
**
** MSP object
** sends out a continuous stream of grains 
** 
** 2001/03/29 started by Nathan Wolek (as grain.phase~)
** 2002/07/11 buffer length no longer stored locally
** 2002/07/15 fixed mono buffer check problem
** 2002/07/25 added deferred buffer changes
** 2002/09/24 added getinfo message
** 2003/07/10 update to CW 8
** 2004/08/08 adding multi-channel capability, beta working
** 2004/08/10 stereo output
** 2005/01/13 testing multichannel allpass interp
** 2005/01/27 scrapping allpass interp for linear
** 2005/02/02 new interp working
** 2005/02/03 INTERP_ON now default for window; added "b_inuse" check
** 
*/

#include "ext.h"		// required for all MAX external objects
#include "ext_common.h" // contains CLIP macro
#include "z_dsp.h"		// required for all MSP external objects
#include "buffer.h"		// required to deal with buffer object
#include <string.h>

//#define DEBUG			//enable debugging messages

#define OBJECT_NAME		"g_phase_stereo~"		// name of the object

/* for the assist method */
#define ASSIST_INLET	1
#define ASSIST_OUTLET	2

/* for direction flag */
#define FORWARD_GRAINS		0
#define REVERSE_GRAINS		1

/* for interpolation flag */
#define INTERP_OFF			0
#define INTERP_ON			1

void *this_class;		// required global pointing to this class

typedef struct _gphasestereo
{
	t_pxobject x_obj;					// <--
	// sound buffer info
	t_symbol *snd_sym;
	t_symbol *next_snd_sym; // added 2004.08.08
	t_buffer *snd_buf_ptr;
	t_buffer *next_snd_buf_ptr;
	//double snd_last_out;	// removed 2005.01.27
	short snd_interp;
	short snd_chan;	// added 2004.08.08
	short next_snd_chan; // added 2004.08.08
	// window buffer info
	t_symbol *win_sym;
	t_symbol *next_win_sym; // added 2004.08.08
	t_buffer *win_buf_ptr;
	t_buffer *next_win_buf_ptr;
	//double win_last_out;	// removed 2005.01.27
	double win_last_index;
	short win_interp;
	short win_chan;	// added 2004.08.08
	short next_win_chan; // added 2004.08.08
	// grain info
	double grain_pos_start;	// in samples
	double grain_pitch;	// as multiplier, 0 to 1
	short grain_direction;	// forward or reverse
	double grain_sound_length; // in samples
	double curr_snd_pos; // in samples
	double snd_step_size; // in samples
	// defered grain info at control rate
	double next_grain_pos_start;	// in milliseconds
	double next_grain_pitch;		// as multiplier, 0 to 1
	short next_grain_direction;		// forward or reverse
	// signal or control grain info
	short grain_pos_start_connected;	// <--
	short grain_pitch_connected;		// <--
	double input_sr;					// <--
	double output_sr;					// <--
	double output_1oversr;				// <--
} t_gphasestereo;

void *gphasestereo_new(t_symbol *s, short argc, t_atom *argv);
t_int *gphasestereo_perform(t_int *w);
void gphasestereo_dsp(t_gphasestereo *x, t_signal **sp, short *count);
void gphasestereo_setsnd(t_gphasestereo *x, t_symbol *s, long chan);
void gphasestereo_setwin(t_gphasestereo *x, t_symbol *s, long chan);
long gphasestereo_updatesnd(t_gphasestereo *x);
long gphasestereo_updatewin(t_gphasestereo *x);
void gphasestereo_float(t_gphasestereo *x, double f);
void gphasestereo_int(t_gphasestereo *x, long l);
void gphasestereo_assist(t_gphasestereo *x, t_object *b, long msg, long arg, char *s);
void gphasestereo_getinfo(t_gphasestereo *x);
void gphasestereo_sndInterp(t_gphasestereo *x, long l);
void gphasestereo_winInterp(t_gphasestereo *x, long l);
void gphasestereo_reverse(t_gphasestereo *x, long l);
double mcLinearInterp(float *in_array, long index_i, double index_frac, long in_size, short in_chans);

t_symbol *ps_buffer;

/********************************************************************************
void main(void)

inputs:			nothing
description:	called the first time the object is used in MAX environment; 
		defines inlets, outlets and accepted messages
returns:		nothing
********************************************************************************/
void main(void)
{
	setup((t_messlist **)&this_class, (method)gphasestereo_new, (method)dsp_free, 
			(short)sizeof(t_gphasestereo), 0L, A_GIMME, 0);
	addmess((method)gphasestereo_dsp, "dsp", A_CANT, 0);
	
	/* bind method "gphasestereo_setsnd" to the 'setSound' message */
	addmess((method)gphasestereo_setsnd, "setSound", A_SYM, A_DEFLONG, 0);
	
	/* bind method "gphasestereo_setwin" to the 'setWin' message */
	addmess((method)gphasestereo_setwin, "setWin", A_SYM, A_DEFLONG, 0);
	
	/* bind method "gphasestereo_float" to the incoming floats */
	addfloat((method)gphasestereo_float);
	
	/* bind method "gphasestereo_int" to the incoming ints */
	addfloat((method)gphasestereo_int);
	
	/* bind method "gphasestereo_assist" to the assistance message */
	addmess((method)gphasestereo_assist, "assist", A_CANT, 0);
	
	/* bind method "gphasestereo_getinfo" to the getinfo message */
	addmess((method)gphasestereo_getinfo, "getinfo", A_NOTHING, 0);
	
	/* bind method "gphasestereo_reverse" to the direction message */
	addmess((method)gphasestereo_reverse, "reverse", A_LONG, 0);
	
	/* bind method "gphasestereo_sndInterp" to the sndInterp message */
	addmess((method)gphasestereo_sndInterp, "sndInterp", A_LONG, 0);
	
	/* bind method "gphasestereo_winInterp" to the winInterp message */
	addmess((method)gphasestereo_winInterp, "winInterp", A_LONG, 0);
	
	dsp_initclass();
	
	/* needed for 'buffer~' work, checks for validity of buffer specified */
	ps_buffer = gensym("buffer~");
	
	#ifndef DEBUG
		
	#endif /* DEBUG */
}

/********************************************************************************
void *gphasestereo_new(double initial_pos)

inputs:			*s		-- symbol containing arguments
				argc	-- number of arguments in array
				argv	-- array of arguments values
description:	called for each new instance of object in the MAX environment;
		defines inlets and outlets; sets variables and buffers
returns:		nothing
********************************************************************************/
void *gphasestereo_new(t_symbol *s, short argc, t_atom *argv)
{
	t_symbol *temp_sym;
	long temp_long;
	
	t_gphasestereo *x = (t_gphasestereo *)newobject(this_class);
	dsp_setup((t_pxobject *)x, 3);					// three inlets
	outlet_new((t_pxobject *)x, "signal");			// one outlet
	outlet_new((t_pxobject *)x, "signal");			// two outlet
	
	// interpret arguments, added 2004.08.08
	switch (argc)
	{
		case 2:
			// if only 2 arguments are specified...
			// argument 0
			if (argv[0].a_type == A_SYM) // test for symbol...
			{
				temp_sym = argv[0].a_w.w_sym;
				x->next_snd_sym = temp_sym; // set snd buffer name
				x->next_snd_chan = 0; // set to channel 1
			} else {
				post("%s: argument %ld is not a symbol", OBJECT_NAME, 0);
			}
			// argument 1
			if (argv[1].a_type == A_SYM) // test for symbol...
			{
				temp_sym = argv[1].a_w.w_sym;
				x->next_win_sym = temp_sym; // set win buffer name
				x->next_win_chan = 0; // set to channel 1
			} else {
				post("%s: argument %ld is not a symbol", OBJECT_NAME, 1);
			}
			break;
		default:
			post("%s: 2 required", OBJECT_NAME);
			break;
	}
	
	/* zero pointers */
	x->snd_buf_ptr = x->next_snd_buf_ptr = NULL;
	x->win_buf_ptr = x->next_win_buf_ptr = NULL;
	
	/* setup variables */
	x->grain_pos_start = x->next_grain_pos_start = 0.0;
	x->grain_pitch = x->next_grain_pitch = 1.0;
	x->curr_snd_pos = x->snd_step_size = 0.0; 
	x->snd_chan = x->win_chan = 0;
	x->snd_sym = x->win_sym = gensym("");
	
	/* set flags to defaults */
	x->snd_interp = INTERP_ON;
	x->win_interp = INTERP_ON;
	x->grain_direction = x->next_grain_direction = FORWARD_GRAINS;
	
	x->x_obj.z_misc = Z_NO_INPLACE;
	
	/* return a pointer to the new object */
	return (x);
}

/********************************************************************************
void gphasestereo_dsp(t_gphasestereo *x, t_signal **sp, short *count)

inputs:			x		-- pointer to this object
				sp		-- array of pointers to input & output signals
				count	-- array of shorts detailing number of signals attached
					to each inlet
description:	called when DSP call chain is built; adds object to signal flow
returns:		nothing
********************************************************************************/
void gphasestereo_dsp(t_gphasestereo *x, t_signal **sp, short *count)
{
	/* set buffers */
	gphasestereo_setsnd(x, x->next_snd_sym, x->next_snd_chan);
	gphasestereo_setwin(x, x->next_win_sym, x->next_win_chan);
	
	/* test inlet 2 and 3 for signal data */
	x->grain_pos_start_connected = count[1];
	x->grain_pitch_connected = count[2];
	
	x->input_sr = sp[0]->s_sr;
	x->output_sr = sp[3]->s_sr;
	x->output_1oversr = 1.0 / x->output_sr;
	
	dsp_add(gphasestereo_perform, 7, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,
		sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
}

/********************************************************************************
t_int *gphasestereo_perform(t_int *w)

inputs:			w		-- array of signal vectors specified in "gphasestereo_dsp"
description:	called at interrupt level to compute object's output; used when
		outlets are connected; tests inlet 2 & 3 to use either control or audio
		rate data
returns:		pointer to the next 
********************************************************************************/
t_int *gphasestereo_perform(t_int *w)
{
	t_gphasestereo *x = (t_gphasestereo *)(w[1]);
	t_float *in_phasor = (t_float *)(w[2]);
	t_float *in_pos_start = (t_float *)(w[3]);
	t_float *in_pitch = (t_float *)(w[4]);
	t_float *out = (t_float *)(w[5]);
	t_float *out2 = (t_float *)(w[6]);
	int vec_size = (int)(w[7]);
	
	t_buffer *s_ptr = x->snd_buf_ptr;
	t_buffer *w_ptr = x->win_buf_ptr;
	float *tab_s, *tab_w;
	double  snd_out, snd_out2, win_out;// last_s, last_w;	// removed 2005.01.27
	double temp, index_s, index_w, last_index_w, s_step_size, temp_index_frac;
	long size_s, size_w, frames_s, frames_w, saveinuse_s, saveinuse_w, temp_index_int;
	short interp_s, interp_w, g_direction, nc_s, nc_w, chan_s, chan_w;
	
	if (x->x_obj.z_disabled)						// object is enabled
		goto out;
	if ((s_ptr == NULL) || (w_ptr == NULL))			// buffer names are defined
		goto zero;
	if (!s_ptr->b_valid || !w_ptr->b_valid)			// files are loaded
		goto zero;
	
	// set "in use" to true; added 2005.02.03
	saveinuse_s = s_ptr->b_inuse;
	s_ptr->b_inuse = true;
	saveinuse_w = w_ptr->b_inuse;
	w_ptr->b_inuse = true;
	
	tab_s = s_ptr->b_samples;
	tab_w = w_ptr->b_samples;
	size_s = s_ptr->b_size;
	size_w = w_ptr->b_size;
	frames_s = s_ptr->b_frames;	//added 2005.02.02
	frames_w = w_ptr->b_frames;	//added 2005.02.02
	//last_s = x->snd_last_out;	// removed 2005.01.27
	//last_w = x->win_last_out;	// removed 2005.01.27
	last_index_w = x->win_last_index;
	index_s = x->curr_snd_pos;
	s_step_size = x->snd_step_size;
	chan_s = x->snd_chan;
	chan_w = x->win_chan;
	nc_s = s_ptr->b_nchans;
	nc_w = w_ptr->b_nchans;
	
	interp_s = x->snd_interp;
	interp_w = x->win_interp;
	g_direction = x->grain_direction;
	
	while(vec_size--){
		temp = *in_phasor;
		temp *= frames_w;
		index_w = temp;
		
		/* check bounds of window index */
		while (index_w < 0)
			index_w += frames_w;
		while (index_w >= frames_w)
			index_w -= frames_w;
			
		if (index_w < last_index_w) {		// if window has wrapped...
			if (index_w < 10.0) {			// and is at beginning...
					
				if (gphasestereo_updatesnd(x)) {
					s_ptr = x->snd_buf_ptr;
					tab_s = s_ptr->b_samples;
					frames_s = s_ptr->b_frames;
					size_s = s_ptr->b_size;
					nc_s = s_ptr->b_nchans;
					
					#ifdef DEBUG
						post("%s: sound buffer pointer updated", OBJECT_NAME);
					#endif /* DEBUG */
				}
				if (gphasestereo_updatewin(x)) {
					w_ptr = x->win_buf_ptr;
					tab_w = w_ptr->b_samples;
					frames_w = w_ptr->b_frames;
					size_w = w_ptr->b_size;
					nc_w = w_ptr->b_nchans;
					
					#ifdef DEBUG
						post("%s: window buffer pointer updated", OBJECT_NAME);
					#endif /* DEBUG */
				}
				
				
				//get next channel settings
				x->snd_chan = x->next_snd_chan;
				chan_s = x->snd_chan;
				x->win_chan = x->next_win_chan;
				chan_w = x->win_chan;
				
				
				//get next direction
				x->grain_direction = x->next_grain_direction;
				g_direction = x->grain_direction;
				
				// test if pitch should be at audio or control rate
				if (x->grain_pitch_connected) { // if pitch is at audio rate
					x->grain_pitch = *in_pitch;
				} else { // if pitch is at control rate
					x->grain_pitch = x->next_grain_pitch;
				}
				
				// compute sound buffer step size per output sample
				x->snd_step_size = x->grain_pitch * s_ptr->b_sr * x->output_1oversr;
				s_step_size = x->snd_step_size;
				
				// test if position should be at audio or control rate
				if (x->grain_pos_start_connected) { // if position is at audio rate
					if (g_direction == FORWARD_GRAINS) {	// if forward...
						x->grain_pos_start = *in_pos_start * s_ptr->b_msr;
						index_s = x->grain_pos_start - s_step_size;
					} else {	// if reverse...
						// estimate length of window for reversed grains
						temp = index_w - (last_index_w - frames_w);
						temp *= frames_w;
						x->grain_pos_start = (*in_pos_start * s_ptr->b_msr) + temp;
						index_s = x->grain_pos_start + s_step_size;
					}
				} else { // if position is at control rate
					if (g_direction == FORWARD_GRAINS) {	// if forward...
						x->grain_pos_start = x->next_grain_pos_start * s_ptr->b_msr;
						index_s = x->grain_pos_start - s_step_size;
					} else {	// if reverse...
						// estimate length of window for reversed grains
						temp = index_w - (last_index_w - frames_w);
						temp *= frames_w;
						x->grain_pos_start = (x->next_grain_pos_start * s_ptr->b_msr) + temp;
						index_s = x->grain_pos_start + s_step_size;
					}
				}
				
			}
		}
		
		
		/* handle temporary vars for interpolation */
		temp_index_int = (long)(index_w); // integer portion of index
		temp_index_frac = index_w - (double)temp_index_int; // fractional portion of index
		if (nc_w > 1) // if buffer has multiple channels...
		{
			// get index to sample from with the interleaved frame
			temp_index_int = temp_index_int * nc_w + chan_w;
		}
		
		switch (interp_w) {
			case INTERP_ON:
				// perform linear interpolation on window buffer output
				win_out = mcLinearInterp(tab_w, temp_index_int, temp_index_frac, size_w, nc_w);
				break;
			case INTERP_OFF:
				// interpolation sounds better than following, but uses more CPU
				win_out = tab_w[temp_index_int];
				break;
		}
		
		
		/* sound index is a double because it uses linearInterp */
		if (g_direction == FORWARD_GRAINS) {	// if forward...
			index_s += s_step_size;		// add to sound index
		} else {	// if reverse...
			index_s -= s_step_size;		// subtract from sound index
		}
		
		/* check bounds of sound index */
		while (index_s < 0)
			index_s += frames_s;
		while (index_s >= frames_s)
			index_s -= frames_s;
		
		
		/* handle temporary vars for interpolation */
		temp_index_int = (long)(index_s); // integer portion of index
		temp_index_frac = index_s - (double)temp_index_int; // fractional portion of index
		
		
		switch (nc_s)
		{
			case 1: // mono
				
				switch (interp_s) {
					case INTERP_ON:
						// perform linear interpolation on sound buffer output
						snd_out = mcLinearInterp(tab_s, temp_index_int, temp_index_frac, size_s, nc_s);
						break;
					case INTERP_OFF:
						// interpolation sounds better than following, but uses more CPU
						snd_out = tab_s[temp_index_int];
						break;
				}
				snd_out2 = 0.;
				break;
			case 2: // stereo
				// offset to channel within the interleaved frame if specified
				temp_index_int = temp_index_int * nc_s + chan_s;
				
				switch (interp_s) {
					case INTERP_ON:
						// perform linear interpolation on sound buffer output
						snd_out = mcLinearInterp(tab_s, temp_index_int, temp_index_frac, size_s, nc_s);
						temp_index_int += 1; // next channel
						snd_out2 = mcLinearInterp(tab_s, temp_index_int, temp_index_frac, size_s, nc_s);
						break;
					case INTERP_OFF:
						// interpolation sounds better than following, but uses more CPU
						snd_out = tab_s[temp_index_int];
						temp_index_int += 1; // next channel
						snd_out2 = tab_s[temp_index_int];
						break;
				}
				
				break;
			default: // just in case
				snd_out = 0.;
				snd_out2 = 0.;
				break;
		}
		
		/* multiply snd_out by win_out */
		*out = snd_out * win_out;
		*out2 = snd_out2 * win_out;
		
		/* update last output variables */
		//last_s = snd_out;	// removed 2005.01.27
		//last_w = win_out;	// removed 2005.01.27
		last_index_w = index_w;
		
		/* advance pointers*/
		in_phasor++, in_pos_start++, in_pitch++, out++, out2++;
	}
	
	/* update last output variables */
	//x->snd_last_out = last_s;	// removed 2005.01.27
	//x->win_last_out = last_w;	// removed 2005.01.27
	x->win_last_index = index_w;
	x->curr_snd_pos = index_s;
	x->snd_step_size = s_step_size;
	
	// reset "in use"; added 2005.02.03
	s_ptr->b_inuse = saveinuse_s;
	w_ptr->b_inuse = saveinuse_w;
	
	return (w + 8);
	
zero:
	while(vec_size--) 
	{
		*out++ = 0.;
		*out2++ = 0.;
	}
out:
	return (w + 8);
}


/********************************************************************************
void gphasestereo_setsnd(t_gphasestereo *x, t_symbol *s, long chan)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
				chan	-- which channel to use in buffer
description:	links buffer holding the grain sound source 
returns:		nothing
********************************************************************************/
void gphasestereo_setsnd(t_gphasestereo *x, t_symbol *s, long chan)
{
	t_buffer *b;
	long achan = CLIP(chan,1,4) - 1;
	
	if ((b = (t_buffer *)(s->s_thing)) && ob_sym(b) == ps_buffer) {
	// validate buffer
		if (x->snd_buf_ptr == NULL) { 
		// if first buffer make current buffer
			x->snd_sym = s;
			//x->snd_last_out = 0.0;	// removed 2005.02.02
			x->snd_chan = achan;
			x->snd_buf_ptr = b; // last so that all is ready
			
			#ifdef DEBUG
				post("%s: current sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
			#endif /* DEBUG */
		} else { 
		// else defer to next buffer
			if (b != x->snd_buf_ptr) 
			// only if it is not the same as the current
			{
				x->next_snd_sym = s;
				x->next_snd_buf_ptr = b;
				
				#ifdef DEBUG
					post("%s: next sound set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
			
			// always copy this
			x->next_snd_chan = achan;
			
		}
	} else {
	// else tell user it's invalid
		error("%s: no buffer~ * %s * found", OBJECT_NAME, s->s_name);
		x->next_snd_buf_ptr = NULL;
	}
}

/********************************************************************************
void gphasestereo_setwin(t_gphasestereo *x, t_symbol *s, long chan)

inputs:			x		-- pointer to this object
				s		-- name of buffer to link
				chan	-- which channel to use in buffer
description:	links buffer holding the grain window 
returns:		nothing
********************************************************************************/
void gphasestereo_setwin(t_gphasestereo *x, t_symbol *s, long chan)
{
	
	t_buffer *b;
	long achan = CLIP(chan,1,4) - 1;
	
	if ((b = (t_buffer *)(s->s_thing)) && ob_sym(b) == ps_buffer) {
	// validate buffer
		if (x->win_buf_ptr == NULL) { 
		// if first buffer make current buffer
			x->win_sym = s;
			//x->win_last_out = 0.0;	//removed 2005.02.02
			x->win_last_index = b->b_frames;
			x->win_chan = achan;
			x->win_buf_ptr = b; // last so that all is ready
			
			#ifdef DEBUG
				post("%s: current win set to buffer~ > %s <", OBJECT_NAME, s->s_name);
			#endif /* DEBUG */
		} else { 
		// else defer to next buffer
			if (b != x->win_buf_ptr) 
			// only if it is not the same as the current
			{
				x->next_win_sym = s;
				x->next_win_buf_ptr = b;
				
				#ifdef DEBUG
					post("%s: next win set to buffer~ > %s <", OBJECT_NAME, s->s_name);
				#endif /* DEBUG */
			}
			
			// always copy this
			x->next_win_chan = achan;
			
		}
	} else {
	// else tell user it's invalid
		error("%s: no buffer~ * %s * found", OBJECT_NAME, s->s_name);
		x->next_snd_buf_ptr = NULL;
	}
	
}

/********************************************************************************
void gphasestereo_updatesnd(t_gphasestereo *x)

inputs:			x		-- pointer to this object
description:	updates buffer holding the grain sound source 
returns:		true or false for success
********************************************************************************/
long gphasestereo_updatesnd(t_gphasestereo *x)
{
	if (x->next_snd_buf_ptr != NULL) {
		x->snd_sym = x->next_snd_sym;
		//x->snd_last_out = 0.0;	//removed 2005.02.02
		x->snd_buf_ptr = x->next_snd_buf_ptr;
		x->next_snd_buf_ptr = NULL;
		return true;
	}
	else 
	{
		return false;
	}
}

/********************************************************************************
void gphasestereo_updatewin(t_gphasestereo *x)

inputs:			x		-- pointer to this object
description:	updates buffer holding the grain win source 
returns:		true or false for success
********************************************************************************/
long gphasestereo_updatewin(t_gphasestereo *x)
{
	if (x->next_win_buf_ptr != NULL) {
		x->win_sym = x->next_win_sym;
		//x->win_last_out = 0.0;	//removed 2005.02.02
		x->win_buf_ptr = x->next_win_buf_ptr;
		x->next_win_buf_ptr = NULL;
		return true;
	}
	else 
	{
		return false;
	}
}

/********************************************************************************
void gphasestereo_float(t_gphasestereo *x, double f)

inputs:			x		-- pointer to our object
				f		-- value of float input
description:	handles floats sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; left inlet generates 
		error message in max window
returns:		nothing
********************************************************************************/
void gphasestereo_float(t_gphasestereo *x, double f)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		x->next_grain_pos_start = f;
	}
	else if (x->x_obj.z_in == 2) // if inlet 3
	{
		x->next_grain_pitch = f;
	}
	else if (x->x_obj.z_in == 0)
	{
		post("%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void gphasestereo_int(t_gphasestereo *x, long l)

inputs:			x		-- pointer to our object
				l		-- value of int input
description:	handles int sent to inlets; inlet 2 sets "next_grain_pos_start" 
		variable; inlet 3 sets "next_grain_length" variable; left inlet generates 
		error message in max window
returns:		nothing
********************************************************************************/
void gphasestereo_int(t_gphasestereo *x, long l)
{
	if (x->x_obj.z_in == 1) // if inlet 2
	{
		x->next_grain_pos_start = (double) l;
	}
	else if (x->x_obj.z_in == 2) // if inlet 3
	{
		x->next_grain_pitch = (double) l;
	}
	else if (x->x_obj.z_in == 0)
	{
		post("%s: left inlet does not accept floats", OBJECT_NAME);
	}
}

/********************************************************************************
void gphasestereo_assist(t_gphasestereo *x, t_object *b, long msg, long arg, char *s)

inputs:			x		-- pointer to our object
				b		--
				msg		--
				arg		--
				s		--
description:	method called when "assist" message is received; allows inlets 
		and outlets to display assist messages as the mouse passes over them
returns:		nothing
********************************************************************************/
void gphasestereo_assist(t_gphasestereo *x, t_object *b, long msg, long arg, char *s)
{
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) phase, 0 to 1");
				break;
			case 1:
				strcpy(s, "(signal/float) sound begin, in milliseconds");
				break;
			case 2:
				strcpy(s, "(signal/float) grain pitch multiplier, 1.0 = unchanged");
				break;
		}
	} else if (msg==ASSIST_OUTLET) {
		switch (arg) {
			case 0:
				strcpy(s, "(signal) grain output");
				break;
		}
	}
	
	#ifdef DEBUG
		post("%s: assist message displayed", OBJECT_NAME);
	#endif /* DEBUG */
}

/********************************************************************************
void gphasestereo_getinfo(t_gphasestereo *x)

inputs:			x		-- pointer to our object
				
description:	method called when "getinfo" message is received; displays info
		about object and last update
returns:		nothing
********************************************************************************/
void gphasestereo_getinfo(t_gphasestereo *x)
{
	post("%s object by Nathan Wolek", OBJECT_NAME);
	post("Last updated on %s - www.nathanwolek.com", __DATE__);
}

/********************************************************************************
void gphasestereo_sndInterp(t_gphasestereo *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "sndInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the sound
		buffer; default is on
returns:		nothing
********************************************************************************/
void gphasestereo_sndInterp(t_gphasestereo *x, long l)
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
void gphasestereo_winInterp(t_gphasestereo *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "winInterp" message is received; allows user 
		to define whether interpolation is used in pulling values from the window
		buffer; default is on
returns:		nothing
********************************************************************************/
void gphasestereo_winInterp(t_gphasestereo *x, long l)
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
void gphasestereo_reverse(t_gphasestereo *x, long l)

inputs:			x		-- pointer to our object
				l		-- flag value
description:	method called when "reverse" message is received; allows user 
		to define whether sound is played forward or reverse; default is forward
returns:		nothing
********************************************************************************/
void gphasestereo_reverse(t_gphasestereo *x, long l)
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
