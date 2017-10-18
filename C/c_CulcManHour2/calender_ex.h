#include <time.h>

/**
 * Culculate whether the date is a national holiday.
 * According to the Japanese Labor Standards Law, 
 * transfer holidays of national holidays are treated as non-legal holidays.
 * Non-legal holidau is same as Saturday.
 * @param time_t	t	 The time.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int isNationalHoloday(time_t);

/**
 * Culculate weekday number.
 * @param time_t	t	 The time.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int subZeller(time_t);
