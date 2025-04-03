static int
count_caps(guint64 caps)
{
	int set_bits = 0;
	while (caps) {
		set_bits += caps & 1;
		caps >>= 1;
	}
	return set_bits;
}
