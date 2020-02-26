/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: timefunc.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include "timefunc.h"

/* This subroutine returns 0 if a date is valid and has a valid
 * granularity. The subroutine return -1 in all other cases.
 * The granularity has a syntax like:
 *              
 *              YYYY-MM-DD
 * or
 *
 *              YYYY-MM-DDTHH:MM:SSZ
 */
int     parse_date(const char *datestr, struct date_struct *d) {
        int     year = -1,month = -1,day = -1,hour = -1,minute = -1,second = -1;
        int     n,num_parsed;
        enum granularity_type     d_type;

        if (datestr == NULL)
                return(-1);

        n = strlen(datestr);

        if (n == 10)  /* = strlen("YYYY-MM-DD") */
                d_type = DAY;
        else if (n == 20) /* = strlen("YYYY-MM-DDTHH:MM:SSZ") */
                d_type = SECOND;
        else
                return(-1);

        num_parsed = 0;

        if (d_type == DAY)  
                num_parsed = sscanf(datestr,"%4d-%2d-%2d",&year,&month,&day);
        else if (d_type == SECOND)
                num_parsed = sscanf(datestr,"%4d-%2d-%2dT%2d:%2d:%2dZ",&year,&month,&day,&hour,&minute,&second);

        if (d_type == DAY && num_parsed != 3)
                return(-1);
        else if (d_type == SECOND && num_parsed != 6)
                return(-1);

        /* We could do some more checks in the dates, but for now we return what we've parsed */

        /* The tval is a big number used to compare two dates.
         * We translate an iso8601 date into a 64-bit number. E.g
         *
         *  1971-05-05T18:55:OOZ
         *
         * becomes
         *
         *  19710505185500
         *
         * Two dates can now be easily compared by preforming a substraction of two tval's. 
         * If the result is negative, then the first date is earlier than the second.
         * If the result is zero, the two dates are equal.
         * If the result is positive, then the first date is later than the second.
         */
        d->year   = year;
        d->month  = month;        
        d->day    = day;
        d->hour   = hour;
        d->minute = minute;
        d->second = second;
        d->granularity = d_type;
        d->tval   = second + (100*minute) + (10000*hour) + (1000000*day) + (100000000*month) + (1000000000000*year);

        return(0);
}

/* This subroutine returns a date in iso8601 format using "Zulu" time. The
 * caller should take care of freeing the memory taken by the string.
 */
char    *iso8601date(void) {
        char            *datestr;
        time_t          t;
        struct tm       *t_struct; 

        datestr = (char *) malloc(sizeof(char) * 21);

        if (datestr == NULL) {
                return NULL;
        } 

        (void) time(&t);
        t_struct = gmtime(&t); 

        (void) strftime((char *)datestr, 21, "%Y-%m-%dT%H:%M:%SZ",t_struct);

        return datestr;
}
