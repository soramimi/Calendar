
#include <chrono>
#include <climits>
#include <cstdio>

#ifdef _WIN32
#include <Windows.h>
int tz_seconds_east()
{
	return -*__timezone();
}
#else
#include <sys/time.h>
int tz_seconds_east()
{
	return -timezone;
}
#endif

#define JD_EPOCH 2440588

struct YMD {
	int y = 0;
	int m = 0;
	int d = 0;
	YMD() = default;
	YMD(int y, int m, int d)
		: y(y)
		, m(m)
		, d(d)
	{
	}
};

int calGregorianToJulian(YMD const &ymd)
{
	int y = ymd.y;
	int m = ymd.m;
	int d = ymd.d;
	if (m < 3) {
		m += 9;
		y--;
	} else {
		m -= 3;
	}
	y += 4800;
	int c = (y / 100);
	return (c * 146097 / 4) + ((y - c * 100) * 1461 / 4) + ((153 * m + 2) / 5) + d - 32045;
}

YMD calJulianToGregorian(int jd)
{
	YMD r;
	r.y = ((jd * 4 + 128179) / 146097);
	r.d = ((jd * 4 - r.y * 146097 + 128179) / 4) * 4 + 3;
	jd = (r.d / 1461);
	r.d = ((r.d - jd * 1461) / 4) * 5 + 2;
	r.m = (r.d / 153);
	r.d = ((r.d - r.m * 153) / 5) + 1;
	r.y = (r.y - 48) * 100 + jd;
	if (r.m < 10) {
		r.m += 3;
	} else {
		r.m -= 9;
		r.y++;
	}
	return r;
}

int weekday(int julian)
{
	return (julian + 1) % 7; // sun == 0
}

int main()
{
	tzset();
	int tz = tz_seconds_east();
	auto now = std::chrono::system_clock::now();
	auto epoch_now = std::chrono::system_clock::to_time_t(now);
	auto local_now = epoch_now + tz_seconds_east();
	int julian = int(local_now / 86400) + JD_EPOCH;
	YMD gregorian = calJulianToGregorian(julian);
	int s = local_now % 86400;
	int h = s / 3600;
	int m = s / 60 % 60;
	s %= 60;
	printf("%d-%02d-%02d, %02d:%02d:%02d, %c%02d%02d, %d\n"
		   , gregorian.y, gregorian.m, gregorian.d
		   , h, m, s
		   , tz < 0 ? '-' : '+'
		   , tz / 3600
		   , tz / 60 % 60
		   , weekday(julian)
		   );
	return 0;
}
