 *****************************************************************************/
static void Interleave( INTERLEAVE_TYPE *p_out, const INTERLEAVE_TYPE **pp_in,
                        int i_nb_channels, int i_samples, uint8_t *pi_chan_table)
{
    for( int j = 0; j < i_samples; j++ )
        for( int i = 0; i < i_nb_channels; i++ )
        {
#ifdef MODULE_NAME_IS_tremor
            union { int32_t i; uint32_t u;} spl;
            spl.u = ((uint32_t)pp_in[i][j]) << 8;
            p_out[j * i_nb_channels + pi_chan_table[i]] = spl.i;
#else
            p_out[j * i_nb_channels + pi_chan_table[i]] = pp_in[i][j];
#endif
        }
}
