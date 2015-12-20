#ifndef macro_StepCounter_h
#define macro_StepCounter_h

#include <stdlib.h>

enum {
	STEP_NONE = 0,
	STEP_HOP,
	STEP_RUN,
	STEP_WALK
};

/*
 * Human readable C string for the type of step.
 * The caller should always free the allocated memory, even when it is
 * STEP_NONE.
 */
char *step_string(const int);

/* Completely opaque stepcounter class */
typedef struct _stepcounter {
	double *_ax_record;
	double *_ay_record;
	double *_gz_record;

	uint16_t _record_count;
	uint16_t _num_peak_records;
	int _record_begin;
	int _record_curr;


	int _state;
} stepcounter;

/*
 * Create a new step counter object.
 *
 * If it fails to allocate the memory for the object, return NULL.
 */
stepcounter *stepcounter_create();

/*
 * Take an array of six doubles, [ax, ay, az, gx, gy, gz], as input, calculate
 * whether or not this should be recognized as triggering a step.
 *
 * Return an integer, one of STEP_NONE, STEP_WALK, STEP_RUN, STEP_HOP, based on
 * the calculation.
 *
 * The algorithm used for determining the step is explained in the .c file.
 */
int stepcounter_next(stepcounter *sc, const double*);

/*
 * Reached the end of records. Internally, the step counter checks any cached
 * records and return an integer, which is one of STEP_NONE, STEP_WALK,
 * STEP_RUN, and STEP_HOP to indicate the result of the check.
 */
int stepcounter_end();

/*
 * Release resources allocated to the step counter object.
 */
void stepcounter_free(stepcounter*);

/*
 * Dump internal information for easy debugging.
 */
void stepcounter_print(stepcounter*);

#endif /* end of include guard */
