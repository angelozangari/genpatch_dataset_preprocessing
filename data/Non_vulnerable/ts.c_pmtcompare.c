}
static int pmtcompare( const void *pa, const void *pb )
{
    int id1 = ((pmt_map_t *)pa)->i_pid;
    int id2 = ((pmt_map_t *)pb)->i_pid;
    return id1 - id2;
}
