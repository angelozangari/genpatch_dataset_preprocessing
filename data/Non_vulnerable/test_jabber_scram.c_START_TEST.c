}
START_TEST(test_mech)
{
	assert_successful_exchange("password", "H7yDYKAWBCrM2Fa5SxGa4iez",
			"n=paul,r=H7yDYKAWBCrM2Fa5SxGa4iez",
			"r=H7yDYKAWBCrM2Fa5SxGa4iezFPVDPpDUcGxPkH3RzP,s=3rXeErP/os7jUNqU,i=4096",
			"c=biws,r=H7yDYKAWBCrM2Fa5SxGa4iezFPVDPpDUcGxPkH3RzP,p=pXkak78EuwwOEwk2/h/OzD7NkEI=",
			"v=ldX4EBNnOgDnNTOCmbSfBHAUCOs=");
#ifdef USE_IDN
	assert_successful_exchange("pass½word", "GNb2HsNI7VnTv8ABsE5AnY8W",
			"n=paul,r=GNb2HsNI7VnTv8ABsE5AnY8W",
			"r=GNb2HsNI7VnTv8ABsE5AnY8W/w/I3eRKM0I7jxFWOH,s=ysAriUjPzFqOXnMQ,i=4096",
			"c=biws,r=GNb2HsNI7VnTv8ABsE5AnY8W/w/I3eRKM0I7jxFWOH,p=n/CtgdWjOYnLQ4m9Na+wPn9D2uY=",
			"v=4TkZwKWy6JHNmrUbU2+IdAaXtos=");
#endif
}
