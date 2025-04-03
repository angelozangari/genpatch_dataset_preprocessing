#include <time.h>
static const char *format_12hour_hour(const struct tm *tm)
{
	static char hr[3];
	int hour = tm->tm_hour % 12;
	if (hour == 0)
		hour = 12;
	g_snprintf(hr, sizeof(hr), "%d", hour);
	return hr;
}
