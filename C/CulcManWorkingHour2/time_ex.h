
#include <time.h>

#define ONE_DAY_SEC 86400 //1day to sec

#define ONE_HOUR_SEC 3600 //1hour to sec
#define TWO_HOUR_SEC 7200 //2hour to sec
#define THREE_HOUR_SEC 10800 //3hour to sec
#define FOUR_HOUR_SEC 14400 //4hour to sec
#define FIVE_HOUR_SEC 18000 //5hour to sec
#define SIX_HOUR_SEC 21600 //6hour to sec
#define SEVEN_HOUR_SEC 25200 //7hour to sec
#define EIGHT_HOUR_SEC 28800 //8hour to sec
#define NIEN_HOUR_SEC 32400 //9hour to sec
#define TEN_HOUR_SEC 36000 //10hour to sec
#define ELEVEN_HOUR_SEC 39600 //11hour to sec
#define TWELVE_HOUR_SEC 43200 //12hour to sec
#define THIRTEEN_HOUR_SEC 46800 //13hour to sec
#define FOURTEEN_HOUR_SEC 50400 //14hour to sec
#define FIFTEEN_HOUR_SEC 54000 //15hour to sec
#define SIXTEEN_HOUR_SEC 57600 //16hour to sec
#define SEVENTEEN_HOUR_SEC 61200 //17hour to sec
#define EIGHTEEN_HOUR_SEC 64800 //18hour to sec
#define NINETEEN_HOUR_SEC 68400 //19hour to sec
#define TWENTY_HOUR_SEC 72000 //20hour to sec
#define TWENTY_ONE_HOUR_SEC 75600 //21hour to sec
#define TWENTY_TWO_HOUR_SEC 79200 //22hour to sec
#define TWENTY_THREE_HOUR_SEC 82800 //23hour to sec
#define TWENTY_FOUR_HOUR_SEC 86400 //24hour to sec
#define TWENTY_FIVE_HOUR_SEC 90000 //25hour to sec
#define TWENTY_SIX_HOUR_SEC 93600 //26hour to sec
#define TWENTY_SEVEN_HOUR_SEC 97200 //27hour to sec
#define TWENTY_EIGHT_HOUR_SEC 100800 //28hour to sec
#define TWENTY_NINE_HOUR_SEC 104400 //29hour to sec

/**
 * Round time[sec] to hour.
 * @param time_t	t	 The time[sec] from 1900-01-01 00:00:00.
 * @return int		The time[hour] from 1900-01-01 00:00:00
 */
int roundSecToHour(time_t);