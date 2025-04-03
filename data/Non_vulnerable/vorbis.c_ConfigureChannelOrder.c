 *****************************************************************************/
static void ConfigureChannelOrder(uint8_t *pi_chan_table, int i_channels,
                                  uint32_t i_channel_mask, bool b_decode)
{
    const uint32_t *pi_channels_in;
    switch( i_channels )
    {
        case 8:
            pi_channels_in = pi_8channels_in;
            break;
        case 7:
            pi_channels_in = pi_7channels_in;
            break;
        case 6:
        case 5:
            pi_channels_in = pi_6channels_in;
            break;
        case 4:
            pi_channels_in = pi_4channels_in;
            break;
        case 3:
            pi_channels_in = pi_3channels_in;
            break;
        default:
            {
                int i;
                for( i = 0; i< i_channels; ++i )
                {
                    pi_chan_table[i] = i;
                }
                return;
            }
    }
    if( b_decode )
        aout_CheckChannelReorder( pi_channels_in, NULL,
                                  i_channel_mask, pi_chan_table );
    else
        aout_CheckChannelReorder( NULL, pi_channels_in,
                                  i_channel_mask, pi_chan_table );
}
