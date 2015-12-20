#ifndef macro_StepCounter_h
#define macro_StepCounter_h

enum {
	STEP_NONE = 0,
	STEP_WALK,
	STEP_RUN,
	STEP_HOP
};

extern static const STEPCOUNTER_HISTORY_LENGTH;

typedef struct _stepcounter {
	double *_ax_record;
	double *_ay_record;
	double *_gz_record;

	int _record_begin;
	int _record_curr;
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
int stepcounter_next(const double*);

/*
 * Release resources allocated to the step counter object.
 */
void stepcounter_free(stepcounter*);

#endif /* end of include guard */
