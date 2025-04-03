}
static uint32_t GetDescriptorLength24b( int i_length )
{
    uint32_t i_l1, i_l2, i_l3;
    i_l1 = i_length&0x7f;
    i_l2 = ( i_length >> 7 )&0x7f;
    i_l3 = ( i_length >> 14 )&0x7f;
    return( 0x808000 | ( i_l3 << 16 ) | ( i_l2 << 8 ) | i_l1 );
}
