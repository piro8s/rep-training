#include "working.h"

int split_count;
struct tm tm_struct;
int year, month, day, hour, minute, dayStride;
time_t today_tm;
time_t atFive;
time_t atEight;
time_t atSixTeen;
time_t atTwentyTwo;
time_t atTwentyFour;
time_t atTwentyNine;

int todayDate() {
	return day;
}

time_t weeklyWHOf(WorkHours *daily) {
	return daily->weeklyWH;
}

int weekdayNumOf(WorkHours *daily) {
	return daily->weekdayNum;
}

int isEnd(char *input) {
	if (input[0] == '0') return 1;
	return 0;
}

int isError(char *input) {
	if (input[0] == '1') return 1;
	return 0;
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

int initTotalWorkHoursStruct(char *in, WorkHours *total) {
	if (isEnd(in) || isError(in)) return 1;

	total->yearMonth = in;
	total->fixedOWH = (time_t)0;
	total->legalOWH = (time_t)0;
	total->midnightOWH = (time_t)0;
	total->nonlegalHolydayWH = (time_t)0;
	total->legalHolydayWH = (time_t)0;
	total->dailyWH = (time_t)0;
	total->weeklyWH = (time_t)0;

	return 0;
}

// 1:end, 2:error, 0:success
int initDailyWorkHoursStruct(char *in, WorkHours *daily) {
	char *splited[6];
	int j;

	if (isEnd(in)) return 1;
	else if (isError(in)) return 2;

	split_count = split(in, " ", splited);
	daily->yearMonthDay = splited[0];
	if (split_count < 1) return 2;
	for (j=1; j<split_count+1; j++) {
		daily->workTime[j-1] = splited[j];
	}

	// 曜日算出関数を利用して、Weekday NumberをweekdayNumに入れる
	char *ymd[3], ymd_temp[strlen(splited[0])+1];
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
		return 2;

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
	daily->dailyWH = (time_t)0;
	daily->weeklyWH = (time_t)0;

	return 0;
}

int culcWorkHours(WorkHours *total, WorkHours *daily) {
	time_t start_tm, end_tm;
	struct tm tm_struct;
	char *sewt[2], *shm[2], *ehm[2];
	int j;
	for (j=0; j<split_count; j++) {
		// 時刻を Time型 に変換する
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
			return 2;

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
			return 2;

		// 労働時間を計算する
		// target year-month と daily->yearMonthDayのyear-month が一致していない場合は労働時間だけ計算する
		if (strncmp(total->yearMonth, daily->yearMonthDay, 7) != 0) {
			daily->dailyWH += difftime(end_tm, start_tm);
			daily->weeklyWH += daily->dailyWH;
			return 1;
		}
		// 労働時間数を計算する
		if(start_tm < atEight) {
			printf("start < atEight, ");
			if (end_tm <= atEight) {
				printf("end <= atEight\n");//Moring(s-e)
				checkMorning(daily, start_tm, end_tm);
			}
			else if (end_tm <= atSixTeen) {
				printf("atEight < end <= atSixTeen\n");//Morning(s-8)/Daytime(8-e)
				checkMorning(daily, start_tm, atEight);
				checkDaytime(daily, atEight, end_tm);
			}
			else if (end_tm <= atTwentyTwo) {
				printf("atSixTeen < end <= atTwentyTwo \n");//Morning(s-8)/Daytime(8-16)/Night(16-e)
				checkMorning(daily, start_tm, atEight);
				checkDaytime(daily, atEight, atSixTeen);
				checkNight(daily, atSixTeen, end_tm);
			}
			else if (end_tm <= atTwentyFour) {
				printf("atTwentyTwo < end \n");//Morning(s-8)/Daytime(8-16)/Night(16-22)/LateNight(22-e)
				checkMorning(daily, start_tm, atEight);
				checkDaytime(daily, atEight, atSixTeen);
				checkNight(daily, atSixTeen, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, end_tm);
			}
			else if (atTwentyFour < end_tm) {
				printf("atTwentyFour < end \n");//Morning(s-8)/Daytime(8-16)/Night(16-22)/LateNight(22-24)/Midnight(24-e)
				checkMorning(daily, start_tm, atEight);
				checkDaytime(daily, atEight, atSixTeen);
				checkNight(daily, atSixTeen, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, atTwentyFour);
				checkMidnight(daily, atTwentyFour, end_tm);
			}
		}
		else if (atEight <= start_tm && start_tm < atSixTeen) {
			printf("atEight <= start < atSixTeen, ");
			if (end_tm <= atSixTeen) {
				printf("end <= atSixTeen\n");//Daytime(s-e)
				checkDaytime(daily, start_tm, end_tm);
			}
			else if (end_tm <= atTwentyTwo) {
				printf("atSixTeen < end <= atTwentyTwo \n");//Daytime(s-16)/Night(16-e)
				checkDaytime(daily, start_tm, atSixTeen);
				checkNight(daily, atSixTeen, end_tm);
			}
			else if (end_tm <= atTwentyFour) {
				printf("atTwentyTwo < end \n");//Daytime(s-16)/Night(16-22)/LateNight(22-e)
				checkDaytime(daily, start_tm, atSixTeen);
				checkNight(daily, atSixTeen, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, end_tm);
			}
			else if (atTwentyFour < end_tm) {
				printf("atTwentyFour < end \n");//Daytime(s-16)/Night(16-22)/LateNight(22-24)/Midnight(24-e)
				checkDaytime(daily, start_tm, atSixTeen);
				checkNight(daily, atSixTeen, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, atTwentyFour);
				checkMidnight(daily, atTwentyFour, end_tm);
			}
		}
		else if (atSixTeen <= start_tm && start_tm < atTwentyTwo) {
			printf("atSixTeen <= start < atTwentyTwo, ");
			if (end_tm <= atTwentyTwo) {
				printf("end <= atTwentyTwo \n");//Night(s-e)
				checkNight(daily, start_tm, end_tm);
			}
			else if (end_tm <= atTwentyFour) {
				printf("atTwentyTwo < end \n");//Night(s-22)/LateNight(22-e)
				checkNight(daily, start_tm, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, end_tm);
			}
			else if (atTwentyFour < end_tm) {
				printf("atTwentyFour < end \n");//Night(s-22)/LateNight(22-24)/Midnight(24-e)
				checkNight(daily, start_tm, atTwentyTwo);
				checkLateNight(daily, atTwentyTwo, atTwentyFour);
				checkMidnight(daily, atTwentyFour, end_tm);
			}
		}
		else if (atTwentyTwo <= start_tm && start_tm < atTwentyFour) {
			printf("atTwentyTwo <= start_tm < atTwentyFour, ");
			if (end_tm <= atTwentyFour) {
				printf("atTwentyTwo < end <= atTwentyFour\n");//LateNight(s-e)
				checkLateNight(daily, start_tm, end_tm);
			}
			else if (atTwentyFour < end_tm) {
				printf("atTwentyFour < end \n");//LateNight(s-24)/Midnight(24-e)
				checkLateNight(daily, start_tm, atTwentyFour);
				checkMidnight(daily, atTwentyFour, end_tm);
			}
		}
		else if (atTwentyFour <= start_tm) {
			printf("atTwentyFour < start\n");//Midnight(s-e)
			checkMidnight(daily, start_tm, end_tm);
		}
		free(worktime);
		free(swt);
		free(ewt);
	}
	return 0;
}

time_t checkWeeklyWH(WorkHours *daily, int lastWorkDay, int lastWorkWeekday, time_t temp_weeklyWH) {
	if ((todayDate() > lastWorkDay) && (daily->weekdayNum < lastWorkWeekday)) return (time_t)0;
	return temp_weeklyWH;
}

