#include "working.h"

int main() {
	char *in_ym = (char *)malloc(TARGET_YEAR_MONTH_CHR_LEN+1);
	char *in_wh = (char *)malloc(WORKING_HOUR_PERIOD_CHR_LEN+1);
	WorkHours *total = (WorkHours *)malloc(sizeof(WorkHours));
	int i=0, j=0;

	//Target Year-Month
	int ym = scanf("%7s", in_ym);
	flush();
	if (initTotalWorkHoursStruct(in_ym, total)) goto END_OF_PROGRAM;

	while(1) {
		static time_t temp_weeklyWH = (time_t)0;
		static int lastWorkDay = 0;
		static int lastWorkWeekday = 7;
		WorkHours *daily = (WorkHours *)malloc(sizeof(WorkHours));
		scanf("%60[ 0-9/:-]", in_wh);
		flush();

		int initWH = initDailyWorkHoursStruct(in_wh, daily);

		if (initWH == 1) break;
		else if (initWH == 2) goto END_OF_PROGRAM;
		else {
			temp_weeklyWH = checkWeeklyWH(daily, lastWorkDay, lastWorkWeekday, temp_weeklyWH);
			lastWorkDay = targetDate();
			lastWorkWeekday = weekdayNumOf(daily);
			daily->weeklyWH = temp_weeklyWH;

			int culcWH = culcWorkHours(total, daily);

			if (culcWH == 1) continue;
			else if (culcWH == 2) goto END_OF_PROGRAM;

			total->nomalWH += daily->nomalWH;
			total->fixedOWH += daily->fixedOWH;
			total->legalOWH += daily->legalOWH;
			total->midnightOWH += daily->midnightOWH;
			total->nonlegalHolydayWH += daily->nonlegalHolydayWH;
			total->legalHolydayWH += daily->legalHolydayWH;

			temp_weeklyWH = weeklyWHOf(daily);

			free(daily);
		}
		i++;
	}

	// printf("%d\n", "所定労働", roundTimeToHour(total->nomalWH));
	printf("%d\n", roundTimeToHour(total->fixedOWH));
	printf("%d\n", roundTimeToHour(total->legalOWH));
	printf("%d\n", roundTimeToHour(total->midnightOWH));
	printf("%d\n", roundTimeToHour(total->nonlegalHolydayWH));
	printf("%d\n", roundTimeToHour(total->legalHolydayWH));

END_OF_PROGRAM:
	return 0;
}