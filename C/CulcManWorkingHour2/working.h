#include <stdlib.h>
#include "stdio_ex.h"
#include "string_ex.h"
#include "calender_ex.h"
#include "time_ex.h"

#define SUCCESS_CHR 48 //character code: '0'
#define FAIL_CHR 49 //character code: '1'

#define TARGET_YEAR_MONTH_CHR_LEN 7
#define WORKING_HOUR_PERIOD_CHR_LEN 60

#define DAIRY_LEGAL_WORKING_HOUR_SEC 28800 //sec = 8 Hours
#define WEEKLY_LEGAL_WORKING_HOUR_SEC 144000 //sec = 40 Hours
#define MAX_DAYS 37
#define MAX_BREAK_TIMES 4

/**
* The working hours structure.
* @member char *	yearMonth			Target Month: YYYY/MM
* @member char *	yearMonthDay		Working date: YYYY/MM/DD
* @member int 		weekdayNum			Weekday number of today
* @member int 		tmorrowWeekdayNum	Weekday number of tmorrow
* @member char *[]	workPeriod			Array of working hours by period
* @member time_t 	dailyWH				Daily Statutory Working Hours [sec]
* @member time_t 	weeklyWH			Weekly Statutory Working Hours [sec]
* @member time_t 	nomalWH				Normal Working Hours [sec]
* @member time_t 	fixedOWH			Fixed Overtime Working Hours [sec]
* @member time_t 	legalOWH			Legal Overtime Working Hours [sec]
* @member time_t 	midnightOWH			Midnight Overtime Working Hours [sec]
* @member time_t 	nonlegalHolydayWH	Non-Legal Holiday Working Hours [sec]
* @member time_t 	legalHolydayWH		Legal Holiday Working Hours [sec]
*/
typedef struct workhours {
	time_t today;		//新しく追加しようとしている。yearMonthDayはこちらに変更
	char *yearMonth;	//ターゲット年月。yearMonthDayのyear-monthと一致しているかを判断する -> int型の201710に変更(yyyy*100+mm)
	char *yearMonthDay;	//労働年月日。yearMonthのyear-monthと一致しているかを判断する -> time_t todayに変更(yyyy*100+mmはcalender_exの中のgetYearMonthを使う)
	int weekdayNum;
	int tmorrowWeekdayNum;
	char *workPeriod[MAX_BREAK_TIMES];	//労働年月日・労働時間ピリオド

	time_t dailyWH;
	time_t weeklyWH;

	time_t nomalWH;
	time_t fixedOWH;
	time_t legalOWH;
	time_t midnightOWH;
	time_t nonlegalHolydayWH;
	time_t legalHolydayWH;
} WorkHours;

extern int split_count;
extern struct tm tm_struct;
extern int year, month, day, hour, minute, dayStride;
extern time_t today_tm;
extern time_t atFive;
extern time_t atOpeningTime;
extern time_t atClosingTime;
extern time_t atTwentyTwo;
extern time_t atTwentyFour;
extern time_t atTwentyNine;

/**
 * Get how many times which target row string is splited with white space.
 * @return int 		The split count.
 */
int splitCount();

/**
 * Get the target date.
 * @return int		The target date.
 */
int targetDate();

/**
 * Return the working hours[sec] of WorkHours.
 * @param WorkHours *	wh		The working hours structure.
 * @return time_t		The working hours[sec] of WorkHours.
 */
time_t weeklyWHOf(WorkHours *daily);

/**
 * Return the weekday number of WorkHours.
 * @param WorkHours *	wh		The working hours structure.
 * @return int		The weekday number of WorkHours.
 */
int weekdayNumOf(WorkHours *daily);

/**
 * Check the input string means that it is end.
 * @param char *	input		The input string.
 * @return int		If it is true, return 1.
 */
int isEnd(char *);

/**
 * Check the input string means that it is error.
 * @param char *	input		The input string.
 * @return int		If it is true, return 1.
 */
int isError(char *);


/**
 * Culculate how many hours work overtime.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		diff	The diff time[sec] to be added to working hours.
 * @return time_t		The time[sec] how many hours work overtime.
 */
time_t isOvertimeWorking(WorkHours *, time_t);

/**
 * Culculate how many hours work at 5:00 to 8:00.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		s		The time of start[sec] to work.
 * @param time_t		e		The time of end[sec] to work.
 */
void checkMorning(WorkHours *, time_t, time_t);

/**
 * Culculate how many hours work at 8:00 to 16:00.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		s		The time of start[sec] to work.
 * @param time_t		e		The time of end[sec] to work.
 */
void checkDaytime(WorkHours *, time_t, time_t);

/**
 * Culculate how many hours work at 16:00 to 22:00.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		s		The time of start[sec] to work.
 * @param time_t		e		The time of end[sec] to work.
 */
void checkNight(WorkHours *, time_t, time_t);

/**
 * Culculate how many hours work at 22:00 to 24:00.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		s		The time of start[sec] to work.
 * @param time_t		e		The time of end[sec] to work.
 */
void checkLateNight(WorkHours *, time_t, time_t);

/**
 * Culculate how many hours work at 24:00 to 29:00.
 * @param WorkHours *	wh		The working hours structure.
 * @param time_t		s		The time of start[sec] to work.
 * @param time_t		e		The time of end[sec] to work.
 */
void checkMidnight(WorkHours *, time_t, time_t);


/**
 * Initialize the total working hours structure.
 * @param char *		in		The input string.
 * @param WorkHours *	total		The total working hours structure.
 * @return int		The code; 1:end, 2:error, 0:success.
 */
int initTotalWorkHoursStruct(char *in, WorkHours *total);

/**
 * Initialize the daily working hours structure.
 * @param char *		in			The input string.
 * @param WorkHours *	daily		The daily working hours structure.
 * @return int		The code; 1:end, 2:error, 0:success.
 */
int initDailyWorkHoursStruct(char *in, WorkHours *daily);

/**
 * Culculate how many hours work overtime.
 * @param WorkHours *	total		The total working hours structure.
 * @param WorkHours *	daily		The daily working hours structure.
 * @return int		The code; 1:end, 2:error, 0:success.
 */
int culcWorkHours(WorkHours *total, WorkHours *daily);

/**
 * Check weekly working hours on today.
 * @param WorkHours *	daily			The daily working hours structure.
 * @param int			lastWorkDay		The date of working last time.
 * @param int			lastWorkWeekday	The weekday number of working last time.
 * @param time_t		temp_weeklyWH	Weekly working hours.
 * @return time_t		Updated weekly working hours.
 */
time_t checkWeeklyWH(WorkHours *daily, int lastWorkDay, int lastWorkWeekday, time_t temp_weeklyWH);
