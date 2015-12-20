#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#include "stepcounter.h"*/

/*
 * Coding style is the same as the Linux kernel, per this document:
 * https://www.kernel.org/doc/Documentation/CodingStyle
 */
int main(int argc, char *argv[])
{
	FILE *fin = argc < 2 ? stdout : fopen(argv[1], "r");

	size_t bufsize = 1000;
	char *buf = (char*) malloc(bufsize);
	if (buf == NULL) {
		fprintf(stderr,"Cannot allocate buffer for reading the input file\n");
		exit(1);
	}

	ssize_t bytesread;

	/* skip the two header rows */
	bytesread = getline(&buf, &bufsize, fin);
	bytesread = getline(&buf, &bufsize, fin);

	while( (bytesread = getline(&buf, &bufsize, fin)) != -1 ) {
		/* strip the trailing \n and make it C string */
		buf[bytesread - 1] = 0;
		printf("%s\n", buf);

		size_t offset = 0;

	 	{ /* Skip first four rows */
		size_t comma_count = 0;
		while(comma_count < 4) {
			if (buf[offset] == ','  &&  offset > 0  &&  buf[offset - 1] != '\\')
				comma_count++;
			offset++;
		}
		}

		/* Hereby offset points to one char before first char for ax. */

		{ /* Parse six decimal numbers, store them in record[6] */
		/* ax, ay, az, gx, gy, gz */
		double record[6];
		size_t record_offset = 0;

		/* string buffer for the decimal number */
		char tokbuf[32];
		size_t last = offset + 1;

		while(offset <= bytesread - 1) {
			if (buf[offset] != ',' && buf[offset] != 0) {
				offset++;
				continue;
			}

			memset(tokbuf, 0, sizeof(tokbuf));
			memcpy(tokbuf, buf + last, offset - last);
			tokbuf[offset - last] = 0;

			double tmp;
			sscanf(tokbuf, "%lf", &tmp);
			record[record_offset] = tmp;

			last = offset + 1;
			record_offset++;
			offset++;
		}
		}

		/*
		 * Feed record to the step counter and print the resulting
		 * walk/hop/run information if this record triggers a step
		 */
		/*const step_t step = stepcounter_next(sc, record);*/
		/*step_print(&step);*/
	}

	return 0;
}
