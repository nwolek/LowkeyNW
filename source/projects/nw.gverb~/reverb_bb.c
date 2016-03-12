/*
** reverb_bb.c
**
** c file
** defines building blocks for reverb algorithms
** 
** 2002.05.20 started by Nathan Wolek
** 2002.08.19 added new local vars in compute functions
** 
*/

#include "reverb_bb.h"	// defines structs for reverb network

/********************************************************************************
void rbb_init_sinTable(rbb_sintable *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	initializes a sine function table
returns:		nothing
********************************************************************************/
void rbb_init_sinTable(rbb_sintable *info_ptr)
{
	int curr_value;
	float x;
	
	// setup sine function table
	info_ptr->tableLength = OSC_TABLE_SIZE;
	info_ptr->table_alloc = sysmem_newptr((info_ptr->tableLength) * sizeof(float));
	info_ptr->table_mem = info_ptr->table_alloc;
	
	// fill sine function table
	curr_value = info_ptr->tableLength;
	while (--curr_value >= 0)
	{
		x = curr_value/(float)info_ptr->tableLength;
		info_ptr->table_alloc[curr_value] = (float) sin(x * 2.0f * PI_10);
	}
	
}

/********************************************************************************
void rbb_free_sinTable(rbb_sintable *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a sine table
returns:		nothing
********************************************************************************/
void rbb_free_sinTable(rbb_sintable *info_ptr)
{
	if (info_ptr->table_alloc)
		sysmem_freeptr(info_ptr->table_alloc);
	info_ptr->table_alloc = 0;
}

/********************************************************************************
void rbb_init_lowPass(rbb_lowpass *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the lowpass filter
				
description:	initializes a lowpass filter
returns:		nothing
********************************************************************************/
void rbb_init_lowPass(rbb_lowpass *info_ptr)
{
	// setup lowpass filter
	info_ptr->coeff = 0.0;
	info_ptr->oneMcoeff = 1.0;
	info_ptr->last_out = 0.0;
}

/********************************************************************************
void rbb_set_lowPass_coeff(rbb_lowpass *info_ptr, float c)

inputs:			*info_ptr -- pointer to information needed for the lowpass filter
				c		  -- coefficient
description:	sets lowpass filter coefficient
returns:		nothing
********************************************************************************/
void rbb_set_lowPass_coeff(rbb_lowpass *info_ptr, float c)
{
	// set lowpass filter coefficient
	info_ptr->coeff = c;
	info_ptr->oneMcoeff = 1.0 - c;
}

/********************************************************************************
void rbb_compute_lowPass1(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the lowpass filter
				*out_ptr -- pointer to output value
description:	computes the lowpass filter using coeff to multiply the input 
	and oneMcoeff to multiply the recursive element
returns:		nothing
********************************************************************************/
void rbb_compute_lowPass1(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr)
{
	// compute output
	*out_ptr = 		( *in_ptr 				* 	info_ptr->coeff) 
				+	( info_ptr->last_out 	* 	info_ptr->oneMcoeff);
				
	// save output for sample delay
	info_ptr->last_out = *out_ptr;
}

/********************************************************************************
void rbb_compute_lowPass2(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the lowpass filter
				*out_ptr -- pointer to output value
description:	computes the lowpass filter using oneMcoeff to multiply the input 
	and coeff to multiply the recursive element
returns:		nothing
********************************************************************************/
void rbb_compute_lowPass2(float *in_ptr, rbb_lowpass *info_ptr, float *out_ptr)
{
	// compute output
	*out_ptr = 		( *in_ptr 				* 	info_ptr->oneMcoeff) 
				+	( info_ptr->last_out 	* 	info_ptr->coeff);
				
	// save output for sample delay
	info_ptr->last_out = *out_ptr;
}

/********************************************************************************
void rbb_init_shortDelay(rbb_delaybuff_small *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	initializes a short delay
returns:		nothing
********************************************************************************/
void rbb_init_shortDelay(rbb_delaybuff_short *info_ptr)
{
	int curr_value;
	
	// setup short delay buffer
	info_ptr->buff_length = DELAY_SHORT_MAX;
	info_ptr->buff_alloc = sysmem_newptr((info_ptr->buff_length) * sizeof(float));
	info_ptr->buff_mem = info_ptr->buff_alloc;
	info_ptr->buff_start = info_ptr->buff_alloc;	//added 2002.08.19
	info_ptr->buff_end = info_ptr->buff_start + info_ptr->buff_length;	//added 2002.08.19
	
	// fill short delay buffer
	curr_value = info_ptr->buff_length;
	while (--curr_value >= 0)
	{
		info_ptr->buff_alloc[curr_value] = 0.0f;
	}
	
	// remaining variables
	info_ptr->delayLength = 0.;
}

/********************************************************************************
void rbb_free_shortDelay(rbb_delaybuff_short *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a short delay
returns:		nothing
********************************************************************************/
void rbb_free_shortDelay(rbb_delaybuff_short *info_ptr)
{
	if (info_ptr->buff_alloc)
		sysmem_freeptr(info_ptr->buff_alloc);
	info_ptr->buff_alloc = 0;
}

/********************************************************************************
void rbb_set_shortDelay_delay(rbb_delaybuff_short *info_ptr, long d)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				d		  -- delay length
description:	sets short delay length
returns:		nothing
********************************************************************************/
void rbb_set_shortDelay_delay(rbb_delaybuff_short *info_ptr, long d)
{
	if (0 < d && d <= info_ptr->buff_length)
	{
		info_ptr->delayLength = d;
	}
	else
	{
		info_ptr->delayLength = info_ptr->buff_length;
		post("short delay: length %ld invalid, set to max %ld", d, info_ptr->delayLength);
	}
}

/********************************************************************************
void rbb_compute_shortDelay(float *in_ptr, rbb_delaybuff_small *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the delay buffer
				*out_ptr -- pointer to output value
description:	computes the short delays based on the info in the struct that is 
	passed to the method
returns:		nothing
********************************************************************************/
void rbb_compute_shortDelay(float *in_ptr, rbb_delaybuff_short *info_ptr, float *out_ptr)
{
	// compute read position
	long b_length = info_ptr->buff_length;
	float *b_write = info_ptr->buff_mem;
	float *b_read = b_write - info_ptr->delayLength;
	float *b_start = info_ptr->buff_start;
	float *b_end = info_ptr->buff_end;
	
	// check bounds of read position
	while (b_read < b_start) b_read += b_length;
	while (b_read >= b_end) b_read -= b_length;
	
	// pull output from buffer
	*out_ptr = *b_read;
	
	// put input into buffer
	*b_write = *in_ptr;
	
	// advance write position and check bounds
	info_ptr->buff_mem += 1;
	if (info_ptr->buff_mem == b_end) info_ptr->buff_mem = b_start;
}

/********************************************************************************
void rbb_init_longDelay(rbb_delaybuff_long *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	initializes a long delay
returns:		nothing
********************************************************************************/
void rbb_init_longDelay(rbb_delaybuff_long *info_ptr)
{
	int curr_value;
	
	// setup long delay buffer
	info_ptr->buff_length = DELAY_LONG_MAX;
	info_ptr->buff_alloc = sysmem_newptr((info_ptr->buff_length) * sizeof(float));
	info_ptr->buff_mem = info_ptr->buff_alloc;
	info_ptr->buff_start = info_ptr->buff_alloc;	//added 2002.08.19
	info_ptr->buff_end = info_ptr->buff_start + info_ptr->buff_length;	//added 2002.08.19
	
	// fill long delay buffer
	curr_value = info_ptr->buff_length;
	while (--curr_value >= 0)
	{
		info_ptr->buff_alloc[curr_value] = 0.0f;
	}
	
	// remaining variables
	info_ptr->delayLength = 0.;
}

/********************************************************************************
void rbb_free_longDelay(rbb_delaybuff_long *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a long delay
returns:		nothing
********************************************************************************/
void rbb_free_longDelay(rbb_delaybuff_long *info_ptr)
{
	if (info_ptr->buff_alloc)
		sysmem_freeptr(info_ptr->buff_alloc);
	info_ptr->buff_alloc = 0;
}

/********************************************************************************
void rbb_set_longDelay_delay(rbb_delaybuff_long *info_ptr, long d)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				d		  -- delay length
description:	sets long delay length
returns:		nothing
********************************************************************************/
void rbb_set_longDelay_delay(rbb_delaybuff_long *info_ptr, long d)
{
	if (0 < d && d <= info_ptr->buff_length)
	{
		info_ptr->delayLength = d;
	}
	else
	{
		info_ptr->delayLength = info_ptr->buff_length;
		post("long delay: length %ld invalid, set to max %ld", d, info_ptr->delayLength);
	}
}

/********************************************************************************
void rbb_compute_longDelay(float *in_ptr, rbb_delaybuff_long *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the delay buffer
				*out_ptr -- pointer to output value
description:	computes a long delay based on the info in the struct that is 
	passed to the method
returns:		nothing
********************************************************************************/
void rbb_compute_longDelay(float *in_ptr, rbb_delaybuff_long *info_ptr, float *out_ptr)
{
	// compute read position
	long b_length = info_ptr->buff_length;
	float *b_write = info_ptr->buff_mem;
	float *b_read = b_write - info_ptr->delayLength;
	float *b_start = info_ptr->buff_start;
	float *b_end = info_ptr->buff_end;
	
	// check bounds of read position
	while (b_read < b_start) b_read += b_length;
	while (b_read >= b_end) b_read -= b_length;
	
	// pull output from buffer
	*out_ptr = *b_read;
	
	// put input into buffer
	*b_write = *in_ptr;
	
	// advance write position and check bounds
	info_ptr->buff_mem += 1;
	if (info_ptr->buff_mem == b_end) info_ptr->buff_mem = b_start;
}

/********************************************************************************
void rbb_init_allpassShort(rbb_allpass_short *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the allpass
				
description:	initializes a short allpass filter
returns:		nothing
********************************************************************************/
void rbb_init_allpassShort(rbb_allpass_short *info_ptr)
{
	int curr_value;
	
	// setup allpass short delay buffer
	info_ptr->buff_length = ALLPASS_SHORT_DELAY_MAX;
	info_ptr->buff_alloc = sysmem_newptr((info_ptr->buff_length) * sizeof(float));
	info_ptr->buff_mem = info_ptr->buff_alloc;
	info_ptr->buff_start = info_ptr->buff_alloc;	//added 2002.08.19
	info_ptr->buff_end = info_ptr->buff_start + info_ptr->buff_length;	//added 2002.08.19
	
	// fill allpass short delay buffer
	curr_value = info_ptr->buff_length;
	while (--curr_value >= 0)
	{
		info_ptr->buff_alloc[curr_value] = 0.0f;
	}
	
	// remaining variables
	info_ptr->delayLength = 0.;
	info_ptr->coeff = 0.0;
	info_ptr->coeff_neg = 0.0;
}

/********************************************************************************
void rbb_free_allpassShort(rbb_allpass_short *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a short allpass delay
returns:		nothing
********************************************************************************/
void rbb_free_allpassShort(rbb_allpass_short *info_ptr)
{
	if (info_ptr->buff_alloc)
		sysmem_freeptr(info_ptr->buff_alloc);
	info_ptr->buff_alloc = 0;
}

/********************************************************************************
void rbb_set_allpassShort_coeff(rbb_allpass_short *info_ptr, float c)

inputs:			*info_ptr -- pointer to information needed for the allpass filter
				c		  -- coefficient
description:	sets allpass filter coefficient
returns:		nothing
********************************************************************************/
void rbb_set_allpassShort_coeff(rbb_allpass_short *info_ptr, float c)
{
	// set lowpass filter coefficient
	info_ptr->coeff = c;
	info_ptr->coeff_neg = -1.0 * c;
}

/********************************************************************************
void rbb_set_allpassShort_delay(rbb_allpass_short *info_ptr, long d)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				d		  -- delay length
description:	sets short allpass delay length
returns:		nothing
********************************************************************************/
void rbb_set_allpassShort_delay(rbb_allpass_short *info_ptr, long d)
{
	if (0 < d && d <= info_ptr->buff_length)
	{
		info_ptr->delayLength = d;
	}
	else
	{
		info_ptr->delayLength = info_ptr->buff_length;
		post("allpass short: length %ld invalid, set to max %ld", d, info_ptr->delayLength);
	}
}

/********************************************************************************
void rbb_compute_allpassShort(float *in_ptr, rbb_allpass_short *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the allpass
				*out_ptr -- pointer to output value
description:	computes the shorter delay buffer allpass filters
returns:		nothing
********************************************************************************/
void rbb_compute_allpassShort(float *in_ptr, rbb_allpass_short *info_ptr, float *out_ptr)
{
	// compute read position
	long b_length = info_ptr->buff_length;
	float *b_write = info_ptr->buff_mem;
	float *b_read = b_write - info_ptr->delayLength;
	float *b_start = info_ptr->buff_start;
	float *b_end = info_ptr->buff_end;
	
	// check bounds of read position
	while (b_read < b_start) b_read += b_length;
	while (b_read >= b_end) b_read -= b_length;
	
	// compute output
	*out_ptr = ( *in_ptr * info_ptr->coeff ) + *b_read;
	
	// compute feedback
	*b_write = *in_ptr + ( *out_ptr * info_ptr->coeff_neg );
	
	// advance write position and check bounds
	info_ptr->buff_mem += 1;
	if (info_ptr->buff_mem == b_end) info_ptr->buff_mem = b_start;
}

/********************************************************************************
void rbb_init_allpassLong(rbb_allpass_long *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the allpass
				
description:	initializes long delay buffer allpass filters
returns:		nothing
********************************************************************************/
void rbb_init_allpassLong(rbb_allpass_long *info_ptr)
{
	int curr_value;
	
	// setup allpass long delay buffer
	info_ptr->buff_length = ALLPASS_LONG_DELAY_MAX;
	info_ptr->buff_alloc = sysmem_newptr((info_ptr->buff_length) * sizeof(float));
	info_ptr->buff_mem = info_ptr->buff_alloc;
	info_ptr->buff_start = info_ptr->buff_alloc;	//added 2002.08.19
	info_ptr->buff_end = info_ptr->buff_start + info_ptr->buff_length;	//added 2002.08.19
	
	// fill allpass long delay buffer
	curr_value = info_ptr->buff_length;
	while (--curr_value >= 0)
	{
		info_ptr->buff_alloc[curr_value] = 0.0f;
	}
	
	// remaining variables
	info_ptr->delayLength = 0.;
	info_ptr->coeff = 0.0;
	info_ptr->coeff_neg = 0.0;
}

/********************************************************************************
void rbb_free_allpassLong(rbb_allpass_long *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a long allpass delay
returns:		nothing
********************************************************************************/
void rbb_free_allpassLong(rbb_allpass_long *info_ptr)
{
	if (info_ptr->buff_alloc)
		sysmem_freeptr(info_ptr->buff_alloc);
	info_ptr->buff_alloc = 0;
}

/********************************************************************************
void rbb_set_allpassLong_coeff(rbb_allpass_long *info_ptr, float c)

inputs:			*info_ptr -- pointer to information needed for the allpass filter
				c		  -- coefficient
description:	sets allpass filter coefficient
returns:		nothing
********************************************************************************/
void rbb_set_allpassLong_coeff(rbb_allpass_long *info_ptr, float c)
{
	// set lowpass filter coefficient
	info_ptr->coeff = c;
	info_ptr->coeff_neg = -1.0 * c;
}

/********************************************************************************
void rbb_set_allpassLong_delay(rbb_allpass_long *info_ptr, long d)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				d		  -- delay length
description:	sets long allpass delay length
returns:		nothing
********************************************************************************/
void rbb_set_allpassLong_delay(rbb_allpass_long *info_ptr, long d)
{
	if (0 < d && d <= info_ptr->buff_length)
	{
		info_ptr->delayLength = d;
	}
	else
	{
		info_ptr->delayLength = info_ptr->buff_length;
		post("allpass long: length %ld invalid, set to max %ld", d, info_ptr->delayLength);
	}
}

/********************************************************************************
void rbb_compute_allpassLong(float *in_ptr, rbb_allpass_long *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the allpass
				*out_ptr -- pointer to output value
description:	computes the longer delay buffer allpass filters
returns:		nothing
********************************************************************************/
void rbb_compute_allpassLong(float *in_ptr, rbb_allpass_long *info_ptr, float *out_ptr)
{
	// compute read position
	long b_length = info_ptr->buff_length;
	float *b_write = info_ptr->buff_mem;
	float *b_read = b_write - info_ptr->delayLength;
	float *b_start = info_ptr->buff_start;
	float *b_end = info_ptr->buff_end;
	
	// check bounds of read position
	while (b_read < b_start) b_read += b_length;
	while (b_read >= b_end) b_read -= b_length;
	
	// compute output
	*out_ptr = ( *in_ptr * info_ptr->coeff ) + *b_read;
	
	// compute feedback
	*b_write = *in_ptr + ( *out_ptr * info_ptr->coeff_neg );
	
	// advance write position and check bounds
	info_ptr->buff_mem += 1;
	if (info_ptr->buff_mem == b_end) info_ptr->buff_mem = b_start;
}

/********************************************************************************
void rbb_init_allpassMod(rbb_allpass_mod *info_ptr, rbb_sintable *osc_ptr)

inputs:			*info_ptr -- pointer to information needed for the allpass
				*osc_ptr  -- pointer to oscilation table
description:	initializes a modulating delay buffer allpass filter
returns:		nothing
********************************************************************************/
void rbb_init_allpassMod(rbb_allpass_mod *info_ptr, rbb_sintable *osc_ptr)
{
	int curr_value;
	
	// setup allpass long delay buffer
	info_ptr->buff_length = ALLPASS_MOD_DELAY_MAX;
	info_ptr->buff_alloc = (float*)sysmem_newptr((info_ptr->buff_length) * sizeof(float));
	info_ptr->buff_mem = info_ptr->buff_alloc;
	info_ptr->buff_start = info_ptr->buff_alloc;	//added 2002.08.19
	info_ptr->buff_end = info_ptr->buff_start + info_ptr->buff_length;	//added 2002.08.19
	info_ptr->buff_write = 0;
	
	// fill allpass long delay buffer
	curr_value = info_ptr->buff_length;
	while (--curr_value >= 0)
	{
		info_ptr->buff_alloc[curr_value] = 0.0f;
	}
	
	// remaining variables
	info_ptr->coeff = 0.0;
	info_ptr->coeff_neg = 0.0;
	info_ptr->initDelayLength = 0.;
	info_ptr->oscTable = osc_ptr;
	info_ptr->oscPhase = 0.0;
	info_ptr->oscFreq = 0.0;
	info_ptr->oscSamplingInc = 0.0;
	info_ptr->oscDepth = 0.0;
	info_ptr->last_out = 0.0;
	info_ptr->last_lfo = 0.0;
}

/********************************************************************************
void rbb_free_allpassMod(rbb_allpass_mod *info_ptr)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				
description:	frees memory of a modulating allpass delay
returns:		nothing
********************************************************************************/
void rbb_free_allpassMod(rbb_allpass_mod *info_ptr)
{
	if (info_ptr->buff_alloc)
		sysmem_freeptr(info_ptr->buff_alloc);
	info_ptr->buff_alloc = 0;
}

/********************************************************************************
void rbb_set_allpassMod_coeff(rbb_allpass_mod *info_ptr, float c)

inputs:			*info_ptr -- pointer to information needed for the allpass filter
				c		  -- coefficient
description:	sets allpass filter coefficient
returns:		nothing
********************************************************************************/
void rbb_set_allpassMod_coeff(rbb_allpass_mod *info_ptr, float c)
{
	// set lowpass filter coefficient
	info_ptr->coeff = c;
	info_ptr->coeff_neg = -1.0 * c;
}

/********************************************************************************
void rbb_set_allpassMod_freq(rbb_allpass_mod *info_ptr, float f, float sr)

inputs:			*info_ptr -- pointer to information needed for the allpass filter
				f		  -- frequency
				sr		  -- sampling rate
description:	sets allpass filter modulation frequency
returns:		nothing
********************************************************************************/
void rbb_set_allpassMod_freq(rbb_allpass_mod *info_ptr, float f, float sr)
{
	// set lowpass filter coefficient
	info_ptr->oscFreq = f;
	info_ptr->oscSamplingInc = (info_ptr->oscTable)->tableLength * f / sr;
}

/********************************************************************************
void rbb_set_allpassMod_delay(rbb_allpass_mod *info_ptr, long d)

inputs:			*info_ptr -- pointer to information needed for the delay buffer
				d		  -- delay length
description:	sets modulating allpass delay length
returns:		nothing
********************************************************************************/
void rbb_set_allpassMod_delay(rbb_allpass_mod *info_ptr, long d)
{
	if (0 < d && d <= info_ptr->buff_length)
	{
		info_ptr->initDelayLength = d;
	}
	else
	{
		info_ptr->initDelayLength = info_ptr->buff_length - (long)info_ptr->oscDepth + 1;
		post("allpass long: length %ld invalid, set to max %ld", d, info_ptr->initDelayLength);
	}
}

/********************************************************************************
void rbb_compute_allpassMod(float *in_ptr, rbb_allpass_mod *info_ptr, float *out_ptr)

inputs:			*in_ptr -- pointer to input value
				*info_ptr -- pointer to information needed for the allpass
				*out_ptr -- pointer to output value
description:	computes the modulating delay buffer allpass filters
returns:		nothing
********************************************************************************/
void rbb_compute_allpassMod(float *in_ptr, rbb_allpass_mod *info_ptr, float *out_ptr)
{
	float lfo_out, buffRead, buffer_output;
	
	long b_length = info_ptr->buff_length;
	float *b_start = info_ptr->buff_start;
	float *b_end = info_ptr->buff_end;
	float *b_write = info_ptr->buff_mem;
	long ot_length = (info_ptr->oscTable)->tableLength;
	
	// compute phase
	info_ptr->oscPhase += info_ptr->oscSamplingInc;
	
	// check bounds of phase
	while (info_ptr->oscPhase >= (float)ot_length) 
			info_ptr->oscPhase -= (float)ot_length;
	
	rbb_allpassInterp((info_ptr->oscTable)->table_alloc, info_ptr->oscPhase, 
			ot_length, info_ptr->last_lfo, &lfo_out);
	info_ptr->last_lfo = lfo_out;
	
	// compute read position
	buffRead = (float)(info_ptr->buff_write) - info_ptr->initDelayLength + 
			(lfo_out * info_ptr->oscDepth);
	
	// check bounds of read position
	while (buffRead < 0.0) buffRead += (float)b_length;
	while (buffRead >= (float)b_length) buffRead -= (float)b_length;
	
	// all pass interpolate the buffer output
	rbb_allpassInterp(info_ptr->buff_alloc, buffRead, b_length, 
			info_ptr->last_out, &buffer_output);
	info_ptr->last_out = buffer_output;				// update last_out
	
	// compute output
	*out_ptr = ( *in_ptr * info_ptr->coeff_neg ) + buffer_output;
	
	// compute feedback
	*b_write = *in_ptr + ( *out_ptr * info_ptr->coeff );
	
	// advance write position and check bounds
	info_ptr->buff_write += 1;
	info_ptr->buff_mem += 1;
	if (info_ptr->buff_mem == b_end)
	{
		info_ptr->buff_write = 0;
		info_ptr->buff_mem = b_start;
	}
}

/********************************************************************************
float rbb_allpassInterp(float *in_array, float index, long bufferLength, 
		float last_out, float *out_ptr)

inputs:			in_array -- name of array of input values
				index -- floating point index value to interpolate
				bufferLength -- length of in_array
				last_out -- value of last output from buffer
				out_ptr -- pointer to output location
description:	performs allpass interpolation on an input array and returns the
	results to a location specified by a pointer; implements filter as specified
	in Dattorro 2: J. Audio Eng. Soc., Vol 45, No 10, 1997 October
returns:		interpolated output
********************************************************************************/
void rbb_allpassInterp(float *in_array, float index, long bufferLength, 
		float last_out, float *out_ptr)
{
	// index = i.frac
	long index_i = (long)index;					// i
	long index_iP1 = index_i + 1;				// i + 1
	float index_frac = index - (float)index_i;	// frac
	
	// make sure that index_iP1 is not out of range
	while (index_iP1 >= bufferLength) index_iP1 -= bufferLength;
	
	// formula as on bottom of page 765 of above Dattorro article
	*out_ptr = in_array[index_i] + index_frac * (in_array[index_iP1] - last_out);
}