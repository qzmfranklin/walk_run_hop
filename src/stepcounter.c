#include "stepcounter.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *step_string(const int step)
{
	char *str = (char*) malloc(6);
	switch (step) {
	case STEP_NONE:
		strcpy(str, "NONE");
		break;
	case STEP_WALK:
		strcpy(str, "WALK");
		break;
	case STEP_RUN:
		strcpy(str, "RUN");
		break;
	case STEP_HOP:
		strcpy(str, "HOP");
		break;
	}
	return str;
}

/*
 * A brief explanation of the algorithm used in stepcounter_next():
 *
 * The acceleration in the y, i.e. vertical, direction has very the most well
 * formed waves for walk, run, and hop. It goes up very quickly, reaches the
 * peak in less than 0.1 seconds, then drop back to below zero.
 *
 * For convenience, denote acceleration in the y direction as ay. Denote ax as
 * the acceleration in the x direction and gz as the angular speed about the z
 * axis.
 *
 * It was observed that:
 *   a) The difference between the ay values from adjacent records is at least
 *      5.0 (m/s^2) if and only if the person starts a walking/running/hopping
 *      cycle. This can be used to determine the start of a walk/run/hop cycle.
 *   b) After ay reaches a peak value (possibly multiple local positive maximi
 *   	near the peak), it drops below zero rather smoothly. That is, ay does
 *   	not oscilate above and below zero at all when it drops. This can be used
 *   	to determine the 'semi-end' of a cycle.
 *   c) The width of a wave is usually less than 0.2 seconds, which roughly
 *      corresponds to 200 records. Starting from the first sharp increase in
 *      ay, within 200 records after it, ax and gz finished their own waves as
 *      well. The maximal values (positive maximal and negative maximal are
 *      different for this purpose) of ax, ay, and az suffice to decide whether
 *      this cycle is a walk, run, or hop.
 *   d) The periods, i.e., the time intervals between adjacent peaks, are
 *      different for walk, run, and hop too. This can be used to further
 *      confirm the decision made with the observation described in (c).
 *
 *      NOTE: (d) is not implemented yet. Maybe in the future if needed better
 *      precision.
 *
 * For easy reference and future reference, here is the matrix mentioned in (c)
 * and (d):
 *          |  ax   |   ay   | 4.5*gz |     T
 *   -------+-------+--------+--------+----------
 *     hop  | 30-40 |  20-40 | 20-40  |  >   1s
 *     run  |  ~ 20 |  25-30 | 10-17  |  < 0.7s
 *     walk |  < 5  |   5-10 |  < 3   |  >   1s
 */

/*
 * With a frequency of 104Hz, this history length is roughly 0.2 seconds, which
 * is long enough to cover the length any peak. Ideally, this number should be
 * configured as is adjustable with customized user input. But for the purpose
 * of prototyping, this is good enough.
 */
static const uint16_t STEPCOUNTER_HISTORY_LENGTH = 300;

/*
 * Append record to the end of cache. If the cache is full, the oldest record
 * is overwritten.
 *
 * Same with stepcounter_next(), the record is the starting address of six
 * double numbers: [ax, ay, az, gx, gy, gz]
 */
void _stepcounter_record_append(stepcounter *sc, const double *record)
{
	sc->_record_curr = (sc->_record_curr + 1) % STEPCOUNTER_HISTORY_LENGTH;

	sc->_ax_record[sc->_record_curr] = record[0];
	sc->_ay_record[sc->_record_curr] = record[1];
	sc->_gz_record[sc->_record_curr] = record[5];

	/*
	 * >= could be replaced by == because only one record is appended at a
	 * time. But keeping it as >= so that this code fragment can be applied
	 * to broader use later.
	 */
	if (sc->_record_begin >= sc->_record_curr)
		sc->_record_begin = sc->_record_curr + 1;
	else
		sc->_record_count++;
}

void _stepcounter_record_reset(stepcounter *sc)
{
	sc->_record_begin = 0;
	sc->_record_curr = -1;
	sc->_record_count = 0;
	sc->_num_peak_records = 0;
}

enum {
	_STATE_NORMAL = 0,
	_STATE_PEAK
};

stepcounter *stepcounter_create()
{
	stepcounter *sc = (stepcounter*) malloc(sizeof(struct _stepcounter));
	if (sc == NULL)
		goto sc_fail;

	sc->_ax_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_ax_record == NULL)
		goto _ax_fail;

	sc->_ay_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_ay_record == NULL)
		goto _ay_fail;

	sc->_gz_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_gz_record == NULL)
		goto _gz_fail;

	_stepcounter_record_reset(sc);

	sc->_state = _STATE_NORMAL;

	return sc;

_gz_fail:
	free(sc->_ay_record);
_ay_fail:
	free(sc->_ax_record);
_ax_fail:
	free(sc);
sc_fail:
	return NULL;
}

static double _stepcounter_max(const double a, const double b)
{
	/*return a - (double)((int)((uint64_t)(a - b) >> 63) * (b - a));*/
	return a > b ? a : b;
}

static double _stepcounter_min(const double a, const double b)
{
	return a < b ? a : b;
}

/*
 * Copy the matrix here to easy reference while coding and debugging:
 *          |  -ax  |   ay   | 4.5*|gz| |     T
 *   -------+-------+--------+----------+----------
 *     hop  | 30-40 |  20-40 |  20-40   |  >   1s
 *     run  |  ~ 20 |  25-30 |  10-17   |  < 0.7s
 *     walk |  < 5  |   5-10 |   < 3    |  >   1s
 *
 * As of 12/19/2015, the fourth column T (period) is not used. To use it, we
 * need a larger cache and potentially significantly more complex algorithm to
 * handle most of the edge cases. That is a major effort lead to a minor gain,
 * not worth it for prototyping. I want to keep this function as simple as
 * possible, only to give the roughest quickest estimate for a start. We can
 * always come back later and the interface is design so that it is easy to add
 * new things.
 */
int _stepcounter_get_step_from_record_cache(stepcounter *sc)
{
	/* Calculate the max and min of ax, ay, and gz in the record cache. */
	double ax_max = DBL_MIN;
	double ay_max = DBL_MIN;
	double gz_max = DBL_MIN;
	double ax_min = DBL_MAX;
	double ay_min = DBL_MAX;
	double gz_min = DBL_MAX;
	for (int i = 0; i < sc->_num_peak_records; i++) {
		const int offset = (sc->_record_curr - i) % STEPCOUNTER_HISTORY_LENGTH;
		const double ax = sc->_ax_record[offset];
		const double ay = sc->_ay_record[offset];
		const double gz = sc->_gz_record[offset];
		ax_max = _stepcounter_max(ax_max, ax);
		ay_max = _stepcounter_max(ay_max, ay);
		gz_max = _stepcounter_max(gz_max, gz);
		ax_min = _stepcounter_min(ax_min, ax);
		ay_min = _stepcounter_min(ay_min, ay);
		gz_min = _stepcounter_min(gz_min, gz);
	}

	/* Calculate the max absolute value, i.e., peak, in the record cache. */
	/*const double ax_peak = _stepcounter_max(fabs(ax_max), fabs(ax_min));*/
	const double ay_peak = _stepcounter_max(fabs(ay_max), fabs(ay_min));
	const double gz_peak = _stepcounter_max(fabs(gz_max), fabs(gz_min));

	/* Guess the most probable step based on the matrix. */
	/*
	 * TODO: Parametrize the magic constants used here. Best way is to put
	 * it in a configuration file that can be trained to better fit each
	 * individual's specific condition.
	 */

	/*fprintf(stderr,"ay_peak = %f\n", ay_peak);*/

	if (ay_peak < 5.0)
		return STEP_NONE;

	if (ay_peak < 14.0)
		return STEP_WALK;

	/* Hereafter, either hop or run */
	if (gz_peak > 20.0 / 4.5)
		return STEP_HOP;
	else
		return STEP_RUN;
}

int stepcounter_next(stepcounter *sc, const double *record)
{
	_stepcounter_record_append(sc, record);

	if (sc->_state == _STATE_NORMAL) {
		/*
		 * Otherwise, check the ay_delta, if the ay_delta is greater
		 * than the threshold value, we consider this as the start of a
		 * peak.
		 */
		/*fprintf(stderr,"_state == _STATE_NORMAL\n");*/
		const double ay_prev = sc->_ay_record[(sc->_record_curr - 1) % STEPCOUNTER_HISTORY_LENGTH];
		const double ay_curr = sc->_ay_record[sc->_record_curr];
		const double ay_delta = fabs(ay_curr - ay_prev);
		/*fprintf(stderr,"ay_delta = %f - %f = %f\n", ay_curr, ay_prev, ay_delta);*/
		if (ay_delta > 3.0) {
			/*fprintf(stderr,"ay_delta (%6f) > 3.0\n", ay_delta);*/
			sc->_state = _STATE_PEAK;
			sc->_num_peak_records = 1;
		}
	} else if (sc->_state == _STATE_PEAK) {
		/*
		 * If ay drops below 0, or the peak is extending too long, stop
		 * collecting records for this peak and try using observation
		 * (c) to determine the most probable one out of walk, hop, and
		 * run.
		 */
		assert(sc->_num_peak_records > 0);
		sc->_num_peak_records++;
		const double ay_curr = sc->_ay_record[sc->_record_curr];
		const double ay_prev = sc->_ay_record[sc->_record_curr - 1];
		if (sc->_record_count > 30 || (ay_curr < ay_prev && ay_curr < 0.0)) {
			/*
			 * NOTE: _stepcounter_get_step_from_record_cache() does
			 * not modify sc.
			 */
			sc->_state = _STATE_NORMAL;
			return _stepcounter_get_step_from_record_cache(sc);
		}
	} else {
		fprintf(stderr,"stepcounter internal errror: Internal state can only be _STATE_NORMAL or _STATE_PEAK\n");
		exit(1);
	}

	return STEP_NONE;

}

void stepcounter_free(stepcounter *sc)
{
	free(sc->_ax_record);
	free(sc->_ay_record);
	free(sc->_gz_record);
	free(sc);
}

void stepcounter_print(stepcounter *sc)
{
	fprintf(stderr,"_record_count = %u\n", sc->_record_count);
	fprintf(stderr,"_record_begin = %d\n", sc->_record_begin);
	fprintf(stderr,"_record_curr  = %d\n", sc->_record_curr);
	fprintf(stderr,"_state        = %s\n", sc->_state == _STATE_NORMAL ? "NORMAL" : "PEAK");
}
