long
wpurple_get_tz_offset() {
	TIME_ZONE_INFORMATION tzi;
	DWORD ret;
	long off = -1;
	if ((ret = GetTimeZoneInformation(&tzi)) != TIME_ZONE_ID_INVALID)
	{
		off = -(tzi.Bias * 60);
		if (ret == TIME_ZONE_ID_DAYLIGHT)
			off -= tzi.DaylightBias * 60;
	}
	return off;
}
