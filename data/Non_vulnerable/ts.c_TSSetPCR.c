}
static void TSSetPCR( block_t *p_ts, mtime_t i_dts )
{
    mtime_t i_pcr = 9 * i_dts / 100;
    p_ts->p_buffer[6]  = ( i_pcr >> 25 )&0xff;
    p_ts->p_buffer[7]  = ( i_pcr >> 17 )&0xff;
    p_ts->p_buffer[8]  = ( i_pcr >> 9  )&0xff;
    p_ts->p_buffer[9]  = ( i_pcr >> 1  )&0xff;
    p_ts->p_buffer[10]|= ( i_pcr << 7  )&0x80;
}
