Code_t ZGetLocations(ZLocations_t *location, int *numlocs);
Code_t ZGetLocations(ZLocations_t *location, int *numlocs)
{
    int i;
    if (!__locate_list)
	return (ZERR_NOLOCATIONS);
    if (__locate_next == __locate_num)
	return (ZERR_NOMORELOCS);
    for (i=0;i<min(*numlocs, __locate_num-__locate_next);i++)
	location[i] = __locate_list[i+__locate_next];
    if (__locate_num-__locate_next < *numlocs)
	*numlocs = __locate_num-__locate_next;
    __locate_next += *numlocs;
    return (ZERR_NONE);
}
