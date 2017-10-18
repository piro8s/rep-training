#include "time_ex.h"

/**
 * Get date from time_t.
 */
int getDate(time_t);

/**
 * Get month from time_t.
 */
int getMonth(time_t);

/**
 * Get year from time_t.
 */
int getYear(time_t);

/**
 * Get year-month from time_t as format:yyyymm.
 */
int getYearMonth(time_t);

/**
 * Culculate weekday number.
 * @param time_t	t	 The time[sec] from 1900-01-01 00:00:00.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int subZeller(time_t);

/**
 * Culculate whether the date is a national holiday.
 * According to the Japanese Labor Standards Law, 
 * transfer holidays of national holidays are treated as non-legal holidays.
 * Non-legal holidau is same as Saturday.
 * @param time_t	t	 The time[sec] from 1900-01-01 00:00:00.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int isNationalHoloday(time_t);