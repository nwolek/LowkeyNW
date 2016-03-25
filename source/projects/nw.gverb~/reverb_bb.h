/*
** reverb_bb.h
**
** header file
** defines building blocks for reverb algorithms
** 
** 2002/05/20 started by Nathan Wolek
** 2002/07/18 first working version
** 2002.08.19 added new local vars in compute functions
** 
*/

#ifndef __REVERB_BB
#define __REVERB_BB

#ifndef __MATH_H
#define __MATH_H
#include <math.h>
#endif /* __MATH_H */

#ifndef __MAXMSP_EXTLIB
	#define __MAXMSP_EXTLIB
	#include "ext.h"		// required for all MAX external objects
	#include "z_dsp.h"		// required for all MSP external objects
#endif /* __MAXMSP_EXTLIB */

#define PI_10 3.141592654
#define OSC_TABLE_SIZE 1024			// size of oscillation table
#define DELAY_SHORT_MAX 5000			// size of short delay line buffer
#define DELAY_LONG_MAX 44100			// size of long delay line buffer
#define COMB_LONG_DELAY_MAX 2000		// size of comb long delay line
#define ALLPASS_SHORT_DELAY_MAX 500		// size of allpass short delay line
#define ALLPASS_LONG_DELAY_MAX 3000		// size of allpass long delay line
#define ALLPASS_MOD_DELAY_MAX 1000		// size of modulating allpass delay line

typedef struct _sintable {			// sinTable info
	long tableLength;
	float *table_alloc;
	float *table_mem;
} rbb_sintable;

typedef struct _lowpass {			// lowpass filter info
	float coeff;
	float oneMcoeff;
	float last_out;
} rbb_lowpass;

typedef struct _delaybuff_short {			// short delay buffer info
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long delayLength;
} rbb_delaybuff_short;

typedef struct _delaybuff_long {			// long delay buffer info
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long delayLength;
} rbb_delaybuff_long;

typedef struct _comb_long {		// comb_long filter info
	float coeff;
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long delayLength;
} rbb_comb_long;

typedef struct _allpass_short {		// allpass_short filter info
	float coeff;
	float coeff_neg;
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long delayLength;
} rbb_allpass_short;

typedef struct _allpass_long {		// allpass_long filter info
	float coeff;
	float coeff_neg;
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long delayLength;
} rbb_allpass_long;

typedef struct _allpass_mod {		// allpass_mod filter info
	float coeff;
	float coeff_neg;
	long buff_length;
	float *buff_alloc;
	float *buff_mem;
	float *buff_start;		//added 2002.08.19
	float *buff_end;		//added 2002.08.19
	long buff_write;
	long initDelayLength;
	rbb_sintable *oscTable;
	float oscPhase;
	float oscFreq;
	float oscSamplingInc;
	float oscDepth;
	float last_out;
	float last_lfo;
} rbb_allpass_mod;

void rbb_init_sinTable(rbb_sintable *info_ptr);
void rbb_free_sinTable(rbb_sintable *info_ptr);

void rbb_init_lowPass(rbb_lowpass *info_ptr);
void rbb_set_lowPass_coeff(rbb_lowpass *info_ptr, float c);
void rbb_compute_lowPass1(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr);
void rbb_compute_lowPass2(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr);

void rbb_init_shortDelay(rbb_delaybuff_short *info_ptr);
void rbb_free_shortDelay(rbb_delaybuff_short *info_ptr);
void rbb_set_shortDelay_delay(rbb_delaybuff_short *info_ptr, long d);
void rbb_compute_shortDelay(float *in_ptr, rbb_delaybuff_short *info_ptr, float *out_ptr);

void rbb_init_longDelay(rbb_delaybuff_long *info_ptr);
void rbb_free_longDelay(rbb_delaybuff_long *info_ptr);
void rbb_set_longDelay_delay(rbb_delaybuff_long *info_ptr, long d);
void rbb_compute_longDelay(float *in_ptr, rbb_delaybuff_long *info_ptr, float *out_ptr);

void rbb_init_allpassShort(rbb_allpass_short *info_ptr);
void rbb_free_allpassShort(rbb_allpass_short *info_ptr);
void rbb_set_allpassShort_coeff(rbb_allpass_short *info_ptr, float c);
void rbb_set_allpassShort_delay(rbb_allpass_short *info_ptr, long d);
void rbb_compute_allpassShort(float *in_ptr, rbb_allpass_short *info_ptr, float *out_ptr);

void rbb_init_allpassLong(rbb_allpass_long *info_ptr);
void rbb_free_allpassLong(rbb_allpass_long *info_ptr);
void rbb_set_allpassLong_coeff(rbb_allpass_long *info_ptr, float c);
void rbb_set_allpassLong_delay(rbb_allpass_long *info_ptr, long d);
void rbb_compute_allpassLong(float *in_ptr, rbb_allpass_long *info_ptr, float *out_ptr);

void rbb_init_allpassMod(rbb_allpass_mod *info_ptr, rbb_sintable *osc_ptr);
void rbb_free_allpassMod(rbb_allpass_mod *info_ptr);
void rbb_set_allpassMod_coeff(rbb_allpass_mod *info_ptr, float c);
void rbb_set_allpassMod_freq(rbb_allpass_mod *info_ptr, float f, float sr);
void rbb_set_allpassMod_delay(rbb_allpass_mod *info_ptr, long d);
void rbb_compute_allpassMod(float *in_ptr, rbb_allpass_mod *info_ptr, float *out_ptr);

void rbb_allpassInterp(float *in_array, float index, long bufferLength, 
	float last_out, float *out_ptr);
	
	
#endif /* __REVERB_BB */