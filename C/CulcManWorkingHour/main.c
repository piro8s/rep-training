#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SUCCESS_CHR 48
#define FAIL_CHR 49

#define YEAR_MONTH_LEN 7
#define WORK_HOURS_LEN 60

#define DAIRY_LEGAL_WORKING_HOURS 28800 //sec = 8 Hours
#define WEEKLY_LEGAL_WORKING_HOURS 144000 //sec = 40 Hours

#define ONE_DAY 86400 //sec
#define FIVE_HOUR 18000 //sec
#define EIGHT_HOUR 28800 //sec
#define SIXTEEN_HOUR 57600 //sec
#define TWENTY_TWO_HOUR 79200 //sec
#define TWENTY_FOUR_HOUR 86400 //sec
#define TWENTY_NINE_HOUR 104400 //sec

#define MAX_DAYS 37

/* static values
- Target Month: YYYY/MM
- Working date: YYYY/MM/DD
- Weekday number of today
- Weekday number of tmorrow
- Array of working hours by period.

- Daily Statutory Working Hours [sec]
- Weekly Statutory Working Hours [sec]

- Normal Working Hours [sec]
- Fixed Overtime Working Hours [sec]
- Legal Overtime Working Hours [sec]
- Late-Night Overtime Working Hours [sec]
- Non-Legal Holiday Working Hours [sec]
- Legal Holiday Working Hours [sec]

*/

typedef struct workhours {
	char *yearMonth;
	char *yearMonthDay;
	int weekdayNum;
	int tmorrowWeekdayNum;
	char *workTime[5];

	time_t dailyWH;
	time_t weeklyWH;

	time_t nomalWH;
	time_t fixedOWH;
	time_t legalOWH;
	time_t midnightOWH;
	time_t nonlegalHolydayWH;
	time_t legalHolydayWH;
} WorkHours;

/**
 * Reset buffer on stdin stream.
 */
void flush() {
	int c;
	while(1) {
		c = getchar();
		if (c == 10 || c == EOF) break;
	}
}

/**
 * Delete '\n' from string.
 * @param char *	str		The target string.
 */
void lntrim(char *str) {
	int i = 0;
	while(1) {
	if(str[i] == '\n') {
		str[i] = '\0';
		break;
	}
	i++;
	}
}

/**
 * Count how many character in the string
 * @param char *	str		The original string.
 * @param char	c	The target characters.
 * @return int	num		The number of characters.
 */
int search(char *str, const char *c){
	int i;
	int num = 0;

	for(i=0; i<strlen(str); i++){
		if(str[i] == *c) num++;
	}
	return num;
}

/**
 * Split string by separater string
 * @param char *	str		The original string.
 * @param const char *	delim	The separater string.
 * @param char *[]	outlist		The output array of strings.
 * @return int	cnt		The split count.
 */
int split(char *str, const char *delim, char *outlist[] ) {
	char *tk;
	int cnt = search(str, delim);
	int i;

	if (cnt < 1) return cnt;
	tk = strtok(str, delim);
	outlist[0] = tk;
	for (i=1; i<cnt+1; i++) {
		tk = strtok(NULL, delim);
		outlist[i] = tk;
	}

	return cnt;
}

/**
 * Culculate whether the date is a national holiday.
 * According to the Japanese Labor Standards Law, 
 * transfer holidays of national holidays are treated as non-legal holidays.
 * Non-legal holidau is same as Saturday.
 * @param time_t	t	 The time.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int isNationalHoloday(time_t t) {
	struct tm *tm;
	tm = localtime(&t);
	int y = tm->tm_year + 1900;
	int m = tm->tm_mon +1;
	int d = tm->tm_mday; 
	int wd = tm->tm_wday;

	int vernalED = (20.8431 + 0.242194*(y - 1980) - (y - 1980)/4);
	int autumnalED = (23.2488 + 0.242194*(y - 1980) - (y - 1980)/4);

	//In the case, it is a Substitute Holiday on Monday.
	if ((m == 1 && d == 2) ||
		(m == 2 && d == 12) ||
		(m == 3 && d == vernalED+1) ||
		(m == 4 && d == 30) ||
		(m == 8 && d == 12) ||
		(m == 9 && d == autumnalED+1) ||
		(m == 11 && d == 3) ||
		(m == 11 && d == 24) ||
		(m == 12 && d == 24)) {
		if (wd == 1) return 6;
	}
	if ((m == 10 && ((double)d/7.0 > 1.0 && (double)d/7.0 <= 2.0))
		|| ((m == 1 || m == 9) && ((double)d/7.0 > 2.0 && (double)d/7.0 <= 3.0))
		|| (m == 7 && (double)d/7.0 > 3.0 && (double)d/7.0 <= 4.0)) {
		if (wd == 1) return 6;
		
	}
	if ((m == 5 && d == 6) && (wd >= 1 && wd <= 3)) return 6;

	//In the case, weekdays will be a national holiday.
	//According to the Japanese Labor Standards Law, 
	// transfer holidays of national holidays are treated as non-legal holidays.
	if ((m == 1 && d == 1) ||
		(m == 2 && d == 11) ||
		(m == 3 && d == vernalED) ||
		(m == 4 && d == 29) ||
		(m == 5 && d == 3) ||
		(m == 5 && d == 4) ||
		(m == 5 && d == 5) ||
		(m == 8 && d == 11) ||
		(m == 9 && d == autumnalED) ||
		(m == 11 && d == 3) ||
		(m == 11 && d == 23) ||
		(m == 12 && d == 23)) {
		if (wd >= 1 && wd <= 5) return 0;
	}

	return wd;
}

/**
 * Culculate weekday number.
 * @param time_t	t	 The time.
 * @return int		The weekday number; 0:Sunday - 6:Saurday.
 */
int subZeller(time_t t) {
	int weekday = (localtime(&t))->tm_wday;
	weekday = isNationalHoloday(t);
	return weekday;
}

int roundTimeToHour(time_t t) {
	return t/60/60;
}

time_t isOvertimeWorking(WorkHours *wh, time_t diff) {
	time_t daily = wh->dailyWH;
	time_t weekly = wh->weeklyWH;
	time_t overtime = (time_t)0;

	//すでに超えている場合
	if (daily > DAIRY_LEGAL_WORKING_HOURS || weekly > WEEKLY_LEGAL_WORKING_HOURS) {
		overtime = diff;
	}
	else {
		daily += diff;
		weekly += diff;
		//この時間で超えた場合
		if (daily > DAIRY_LEGAL_WORKING_HOURS || weekly > WEEKLY_LEGAL_WORKING_HOURS) {
			overtime = (daily - DAIRY_LEGAL_WORKING_HOURS) < (weekly - WEEKLY_LEGAL_WORKING_HOURS) ? (weekly - WEEKLY_LEGAL_WORKING_HOURS) : (daily - DAIRY_LEGAL_WORKING_HOURS);
		}
	}

	return overtime;
}

//5-8{法定内/法定外}/{所定休日/法定休日}
void checkMorning(WorkHours *wh, time_t s, time_t e) {
	// printf("checkMorning(%d to %d)\n", (localtime(&s))->tm_hour, (localtime(&e))->tm_hour);
	int wd = wh->weekdayNum;
	time_t diff = difftime(e, s);
	time_t overtime;

	switch(wd) {
		case 0:
			wh->legalHolydayWH += diff;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			overtime = isOvertimeWorking(wh, diff);
			wh->fixedOWH += diff - overtime;
			wh->legalOWH += overtime;
			break;
		case 6:
			wh->nonlegalHolydayWH += diff;
			break;
	}
	wh->dailyWH += diff;
	wh->weeklyWH += diff;
}

//8-16{所定/法定外}/{所定休日/法定休日}
void checkDaytime(WorkHours *wh, time_t s, time_t e) {
	// printf("checkDaytime(%d to %d)\n", (localtime(&s))->tm_hour, (localtime(&e))->tm_hour);
	int wd = wh->weekdayNum;
	time_t diff = difftime(e, s);
	time_t overtime;

	switch(wd) {
		case 0:
			wh->legalHolydayWH += diff;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			overtime = isOvertimeWorking(wh, diff);
			wh->nomalWH += diff - overtime;
			wh->legalOWH += overtime;
			break;
		case 6:
			wh->nonlegalHolydayWH += diff;
			break;
	}
	wh->dailyWH += diff;
	wh->weeklyWH += diff;
}


//17-22{法定内/法定外}/{所定休日/法定休日}
void checkNight(WorkHours *wh, time_t s, time_t e) {
	// printf("checkNight(%d to %d)\n", (localtime(&s))->tm_hour, (localtime(&e))->tm_hour);
	int wd = wh->weekdayNum;
	time_t diff = difftime(e, s);
	time_t overtime;

	switch(wd) {
		case 0:
			wh->legalHolydayWH += diff;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			overtime = isOvertimeWorking(wh, diff);
			wh->fixedOWH += diff - overtime;
			wh->legalOWH += overtime;
			break;
		case 6:
			wh->nonlegalHolydayWH += diff;
			break;
	}
	wh->dailyWH += diff;
	wh->weeklyWH += diff;
}

//22-24{法定内/法定外}/{所定休日/法定休日} +{深夜}
void checkLateNight(WorkHours *wh, time_t s, time_t e) {
	// printf("checkLateNight(%d to %d)\n", (localtime(&s))->tm_hour, (localtime(&e))->tm_hour);
	int wd = wh->weekdayNum;
	time_t diff = difftime(e, s);
	time_t overtime;

	wh->midnightOWH += diff;
	switch(wd) {
		case 0:
			wh->legalHolydayWH += diff;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			overtime = isOvertimeWorking(wh, diff);
			wh->fixedOWH += diff - overtime;
			wh->legalOWH += overtime;
			break;
		case 6:
			wh->nonlegalHolydayWH += diff;
			break;
	}
	wh->dailyWH += diff;
	wh->weeklyWH += diff;
}

//24-29<<翌日判定>>{法定内/法定外}/{所定休日/法定休日} +{深夜}
void checkMidnight(WorkHours *wh, time_t s, time_t e) {
	// printf("checkMidnight(%d to %d)\n", (localtime(&s))->tm_hour, (localtime(&e))->tm_hour);
	int wd = wh->tmorrowWeekdayNum;
	time_t diff = difftime(e, s);
	time_t overtime;

	wh->midnightOWH += diff;
	switch(wd) {
		case 0:
			wh->legalHolydayWH += diff;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			overtime = isOvertimeWorking(wh, diff);
			wh->fixedOWH += diff - overtime;
			wh->legalOWH += overtime;
			break;
		case 6:
			wh->nonlegalHolydayWH += diff;
			break;
	}
	wh->dailyWH += diff;
	wh->weeklyWH += diff;
}

int isEnd(char *input) {
	if (input[0] == '0') return 1;
	return 0;
}

int isError(char *input) {
	if (input[0] == '1') return 1;
	return 0;
}

int main() {
	char *in_ym = (char *)malloc(YEAR_MONTH_LEN+1);
	char *in_wh = (char *)malloc(WORK_HOURS_LEN+1);

	int i=0, j=0;


	WorkHours *total = (WorkHours *)malloc(sizeof(WorkHours));

	//Target Year-Month
	int ym = scanf("%7s", in_ym);
	flush();
	if (isEnd(in_ym) || isError(in_ym)) goto END_OF_PROGRAM;
	total->yearMonth = in_ym;

	total->fixedOWH = (time_t)0;
	total->legalOWH = (time_t)0;
	total->midnightOWH = (time_t)0;
	total->nonlegalHolydayWH = (time_t)0;
	total->legalHolydayWH = (time_t)0;
	total->dailyWH = (time_t)0;
	total->weeklyWH = (time_t)0;

	time_t temp_weeklyWH = (time_t)0;
	int lastWorkDay = 0;
	int lastWorkWeekday = 7;
	while(1) {
		WorkHours *daily = (WorkHours *)malloc(sizeof(WorkHours));
		scanf("%60[ 0-9/:-]", in_wh);
		flush();

		if (isEnd(in_wh)) break;
		else if (isError(in_wh)) goto END_OF_PROGRAM;
		else {
			char *splited[6];
			struct tm tm_struct;
			time_t start_tm;
			time_t end_tm;
			time_t today_tm;
			time_t atFive;
			time_t atEight;
			time_t atSixTeen;
			time_t atTwentyTwo;
			time_t atTwentyFour;
			time_t atTwentyNine;

			int split_count = split(in_wh, " ", splited);
			daily->yearMonthDay = splited[0];
			if (split_count < 1) goto END_OF_PROGRAM;
			for (j=1; j<split_count+1; j++) {
				daily->workTime[j-1] = splited[j];
			}

			char *ymd[3], ymd_temp[strlen(splited[0])+1];
			int year, month, day, hour, minute, dayStride;
			strcpy(ymd_temp, splited[0]);
			split(ymd_temp, "/", ymd);
			year = atoi(ymd[0]);
			month = atoi(ymd[1]);
			day = atoi(ymd[2]);

			tm_struct.tm_year = year - 1900;
			tm_struct.tm_mon = month - 1;
			tm_struct.tm_mday = day;
			tm_struct.tm_hour = 0;
			tm_struct.tm_min = 0;

			if ((today_tm = mktime(&tm_struct)) == (time_t)-1)
				goto END_OF_PROGRAM;

			atFive = today_tm + FIVE_HOUR;
			atEight = today_tm + EIGHT_HOUR;
			atSixTeen = today_tm + SIXTEEN_HOUR;
			atTwentyTwo = today_tm + TWENTY_TWO_HOUR;
			atTwentyFour = today_tm + TWENTY_FOUR_HOUR;
			atTwentyNine = today_tm + TWENTY_NINE_HOUR;

			daily->weekdayNum = subZeller(today_tm);
			daily->tmorrowWeekdayNum = subZeller(today_tm + ONE_DAY);

			daily->nomalWH = (time_t)0;
			daily->fixedOWH = (time_t)0;
			daily->legalOWH = (time_t)0;
			daily->midnightOWH = (time_t)0;
			daily->nonlegalHolydayWH = (time_t)0;
			daily->legalHolydayWH = (time_t)0;

			if ((day > lastWorkDay) && (daily->weekdayNum < lastWorkWeekday)) temp_weeklyWH = (time_t)0;
			lastWorkDay = day;
			lastWorkWeekday = daily->weekdayNum;
			daily->dailyWH = (time_t)0;
			daily->weeklyWH = temp_weeklyWH;


			char *sewt[2], *shm[2], *ehm[2];
			for (j=0; j<split_count; j++) {
				char *worktime = (char *)malloc(12);
				strcpy(worktime, daily->workTime[j]);
				split(worktime, "-", sewt);
				char *swt = (char *)malloc(6);
				char *ewt = (char *)malloc(6);
				strcpy(swt, sewt[0]);
				strcpy(ewt, sewt[1]);
				split(swt, ":", shm);
				split(ewt, ":", ehm);


				//start time
				hour = atoi(shm[0]);
				minute = atoi(shm[1]);
				dayStride = 0;
				if (hour > 23) {
					dayStride = 1;
					hour -= 24;
				}
				tm_struct.tm_mday = day + dayStride;
				tm_struct.tm_hour = hour;
				tm_struct.tm_min = minute;
				if ((start_tm = mktime(&tm_struct)) == (time_t)-1)
					goto END_OF_PROGRAM;

				//end time
				hour = atoi(ehm[0]);
				minute = atoi(ehm[1]);
				dayStride = 0;
				if (hour > 23) {
					dayStride = 1;
					hour -= 24;
				}
				tm_struct.tm_mday = day + dayStride;
				tm_struct.tm_hour = hour;
				tm_struct.tm_min = minute;
				if ((end_tm = mktime(&tm_struct)) == (time_t)-1)
					goto END_OF_PROGRAM;

				// 労働時間を計算する
				if (strncmp(total->yearMonth, daily->yearMonthDay, 7) != 0) {
					daily->dailyWH += difftime(end_tm, start_tm);
					daily->weeklyWH += daily->dailyWH;
					continue;
				}
				// 労働時間数を計算する
				if(start_tm < atEight) {
					// printf("start < atEight, ");
					if (end_tm <= atEight) {
						// printf("end <= atEight\n");//Moring(s-e)
						checkMorning(daily, start_tm, end_tm);
					}
					else if (end_tm <= atSixTeen) {
						// printf("atEight < end <= atSixTeen\n");//Morning(s-8)/Daytime(8-e)
						checkMorning(daily, start_tm, atEight);
						checkDaytime(daily, atEight, end_tm);
					}
					else if (end_tm <= atTwentyTwo) {
						// printf("atSixTeen < end <= atTwentyTwo \n");//Morning(s-8)/Daytime(8-16)/Night(16-e)
						checkMorning(daily, start_tm, atEight);
						checkDaytime(daily, atEight, atSixTeen);
						checkNight(daily, atSixTeen, end_tm);
					}
					else if (end_tm <= atTwentyFour) {
						// printf("atTwentyTwo < end \n");//Morning(s-8)/Daytime(8-16)/Night(16-22)/LateNight(22-e)
						checkMorning(daily, start_tm, atEight);
						checkDaytime(daily, atEight, atSixTeen);
						checkNight(daily, atSixTeen, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, end_tm);
					}
					else if (atTwentyFour < end_tm) {
						// printf("atTwentyFour < end \n");//Morning(s-8)/Daytime(8-16)/Night(16-22)/LateNight(22-24)/Midnight(24-e)
						checkMorning(daily, start_tm, atEight);
						checkDaytime(daily, atEight, atSixTeen);
						checkNight(daily, atSixTeen, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, atTwentyFour);
						checkMidnight(daily, atTwentyFour, end_tm);
					}
				}
				else if (atEight <= start_tm && start_tm < atSixTeen) {
					// printf("atEight <= start < atSixTeen, ");
					if (end_tm <= atSixTeen) {
						// printf("end <= atSixTeen\n");//Daytime(s-e)
						checkDaytime(daily, start_tm, end_tm);
					}
					else if (end_tm <= atTwentyTwo) {
						// printf("atSixTeen < end <= atTwentyTwo \n");//Daytime(s-16)/Night(16-e)
						checkDaytime(daily, start_tm, atSixTeen);
						checkNight(daily, atSixTeen, end_tm);
					}
					else if (end_tm <= atTwentyFour) {
						// printf("atTwentyTwo < end \n");//Daytime(s-16)/Night(16-22)/LateNight(22-e)
						checkDaytime(daily, start_tm, atSixTeen);
						checkNight(daily, atSixTeen, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, end_tm);
					}
					else if (atTwentyFour < end_tm) {
						// printf("atTwentyFour < end \n");//Daytime(s-16)/Night(16-22)/LateNight(22-24)/Midnight(24-e)
						checkDaytime(daily, start_tm, atSixTeen);
						checkNight(daily, atSixTeen, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, atTwentyFour);
						checkMidnight(daily, atTwentyFour, end_tm);
					}
				}
				else if (atSixTeen <= start_tm && start_tm < atTwentyTwo) {
					// printf("atSixTeen <= start < atTwentyTwo, ");
					if (end_tm <= atTwentyTwo) {
						// printf("end <= atTwentyTwo \n");//Night(s-e)
						checkNight(daily, start_tm, end_tm);
					}
					else if (end_tm <= atTwentyFour) {
						// printf("atTwentyTwo < end \n");//Night(s-22)/LateNight(22-e)
						checkNight(daily, start_tm, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, end_tm);
					}
					else if (atTwentyFour < end_tm) {
						// printf("atTwentyFour < end \n");//Night(s-22)/LateNight(22-24)/Midnight(24-e)
						checkNight(daily, start_tm, atTwentyTwo);
						checkLateNight(daily, atTwentyTwo, atTwentyFour);
						checkMidnight(daily, atTwentyFour, end_tm);
					}
				}
				else if (atTwentyTwo <= start_tm && start_tm < atTwentyFour) {
					// printf("atTwentyTwo <= start_tm < atTwentyFour, ");
					if (end_tm <= atTwentyFour) {
						// printf("atTwentyTwo < end <= atTwentyFour\n");//LateNight(s-e)
						checkLateNight(daily, start_tm, end_tm);
					}
					else if (atTwentyFour < end_tm) {
						// printf("atTwentyFour < end \n");//LateNight(s-24)/Midnight(24-e)
						checkLateNight(daily, start_tm, atTwentyFour);
						checkMidnight(daily, atTwentyFour, end_tm);
					}
				}
				else if (atTwentyFour <= start_tm) {
					// printf("atTwentyFour < start\n");//Midnight(s-e)
					checkMidnight(daily, start_tm, end_tm);
				}
			}

			total->nomalWH += daily->nomalWH;
			total->fixedOWH += daily->fixedOWH;
			total->legalOWH += daily->legalOWH;
			total->midnightOWH += daily->midnightOWH;
			total->nonlegalHolydayWH += daily->nonlegalHolydayWH;
			total->legalHolydayWH += daily->legalHolydayWH;

			temp_weeklyWH = daily->weeklyWH;

			free(daily);
		}
		i++;
	}

	// printf("%d\n", roundTimeToHour(total->nomalWH));
	printf("%d\n", roundTimeToHour(total->fixedOWH));
	printf("%d\n", roundTimeToHour(total->legalOWH));
	printf("%d\n", roundTimeToHour(total->midnightOWH));
	printf("%d\n", roundTimeToHour(total->nonlegalHolydayWH));
	printf("%d\n", roundTimeToHour(total->legalHolydayWH));

	goto END_OF_PROGRAM;


END_OF_PROGRAM:
	printf("END_OF_PROGRAM\n");
	return 0;
}