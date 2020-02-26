/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: timefunc.h,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#ifndef __timefunc_h
#define __timefunc_h

#include <time.h>

enum    granularity_type { 
        DAY=0, 
        SECOND=1
};

struct  date_struct {
        int     year;
        int     month;
        int     day;
        int     hour;
        int     minute;
        int     second;
        enum granularity_type granularity;
        long long tval;
};

/* This subroutine returns 0 if a date is valid and has a valid
 * granularity. The subroutine return -1 in all other cases.
 * The granularity has a syntax like:
 *              
 *              YYYY-MM-DD
 * or
 *
 *              YYYY-MM-DDTHH:MM:SSZ
 */
int     parse_date(const char *datestr,struct date_struct *);

/* This subroutine returns a date in iso8601 format using "Zulu" time. The
 * caller should take care of freeing the memory taken by the string.
 */
char    *iso8601date(void);

#endif
