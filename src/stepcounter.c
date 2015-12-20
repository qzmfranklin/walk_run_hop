#include "stepcounter.h"
#include <stdlib.h>

/*
 * With a frequency of 104Hz, this history length is roughly 0.2 seconds, which
 * is long enough to cover the length any peak. Ideally, this number should be
 * configured as is adjustable with customized user input. But for the purpose
 * of prototyping, this is good enough.
 */
static const STEPCOUNTER_HISTORY_LENGTH = 200;

/*
 * Append record to the end of cache. If the cache is full, the oldest record
 * is overwritten.
 *
 * Same with stepcounter_next(), the record is the starting address of six
 * double numbers: [ax, ay, az, gx, gy, gz]
 */
_stepcounter_record_append(stepcounter *sc, const double *record)
{
	_record_curr = (_record_curr + 1) % STEPCOUNTER_HISTORY_LENGTH;

	sc->_ax_record[sc->_record_curr] = record[0];
	sc->_ay_record[sc->_record_curr] = record[1];
	sc->_gz_record[sc->_record_curr] = record[5];

	/*
	 * >= could be replaced by == because only one record is appended at a
	 * time. But keeping it as >= so that this code fragment can be applied
	 * to broader use later.
	 */
	if (sc->_record_begin >= sc->_record_curr)
		_record_begin = sc->_record_curr + 1;
	else
		sc->_record_count++;
}

/*
 * Lookback(k)
 *
 * Return the offset in the history buffer for displacement records before the
 * lastest record. The latest record has offset 0 for this purpose.
 *
 * Example: lookback(10) returns the offset for 10 records before the last
 * record.
 *
 * Return -1 if the specified position is invalid.
 */
_stepcounter_record_lookback(stepcounter *sc, const uint16_t displacement)
{
	return displacement >= sc->_record_count ?
		-1 : (sc->_record_curr - displacement) % STEPCOUNTER_HISTORY_LENGTH;
}

stepcounter *stepcounter_create()
{
	stepcounter *sc = (stepcounter*) malloc(sizeof(_stepcounter));
	if (sc == NULL)
		goto sc_fail:

	sc->_ax_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_ax_record == NULL)
		goto _ax_fail;

	sc->_ay_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_ay_record == NULL)
		goto _ay_fail;

	sc->_gz_record = (double*) malloc(sizeof(double) * STEPCOUNTER_HISTORY_LENGTH);
	if (sc->_gz_record == NULL)
		goto _gz_fail;

	sc->_record_begin = 0;
	sc->_record_curr = -1;

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

static const double STEPCOUNTER_AY_THRESHOLD_DELTA = 5.0;
int stepcounter_next(stepcounter *sc, const double *record)
{
	_stepcounter_record_append(sc, record);

	/* If this is the first record, just put it into the history cache */
	if (sc->_record_count == 0)
		return STEP_NONE;

	/*
	 * Otherwise, check the ay_delta, if the ay_delta is greater than the
	 * threshold value, as defined by STEPCOUNTER_AY_THRESHOLD_DELTA, we
	 * consider this as the start of a peak, which very likely would lead to
	 * the detection of a walk/run/hop step.
	 */
}

void stepcounter_free(stepcounter *sc)
{
	free(sc->_ax_record);
	free(sc->_ay_record);
	free(sc->_gz_record);
	free(sc);
}
