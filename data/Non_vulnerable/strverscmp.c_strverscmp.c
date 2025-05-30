*/
int strverscmp (const char *s1, const char *s2)
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  /* Symbol(s)    0       [1-9]   others
     Transition   (10) 0  (01) d  (00) x   */
  static const uint8_t next_state[] =
  {
      /* state    x    d    0  */
      /* S_N */  S_N, S_I, S_Z,
      /* S_I */  S_N, S_I, S_I,
      /* S_F */  S_N, S_F, S_F,
      /* S_Z */  S_N, S_F, S_Z
  };
  static const int8_t result_type[] =
  {
      /* state   x/x  x/d  x/0  d/x  d/d  d/0  0/x  0/d  0/0  */
      /* S_N */  CMP, CMP, CMP, CMP, LEN, CMP, CMP, CMP, CMP,
      /* S_I */  CMP, -1,  -1,  +1,  LEN, LEN, +1,  LEN, LEN,
      /* S_F */  CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_Z */  CMP, +1,  +1,  -1,  CMP, CMP, -1,  CMP, CMP
  };
  if (p1 == p2)
    return 0;
  unsigned char c1 = *p1++;
  unsigned char c2 = *p2++;
  /* Hint: '0' is a digit too.  */
  int state = S_N + ((c1 == '0') + (isdigit (c1) != 0));
  int diff;
  while ((diff = c1 - c2) == 0)
    {
      if (c1 == '\0')
	return diff;
      state = next_state[state];
      c1 = *p1++;
      c2 = *p2++;
      state += (c1 == '0') + (isdigit (c1) != 0);
    }
  state = result_type[state * 3 + (((c2 == '0') + (isdigit (c2) != 0)))];
  switch (state)
  {
    case CMP:
      return diff;
    case LEN:
      while (isdigit (*p1++))
	if (!isdigit (*p2++))
	  return 1;
      return isdigit (*p2) ? -1 : diff;
    default:
      return state;
  }
}
