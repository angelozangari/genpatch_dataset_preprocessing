 *****************************************************************************/
static int SubsdelayGetWordRank( int i_length )
{
    /* p_rank[0] = p_rank[1] = p_rank[2] = 300;
    for( i = 3; i < 20; i++ ) p_rank[i] = (int) ( 1.1 * p_rank[i - 1] ); */
    static const int p_rank[20] = { 300, 300, 300, 330, 363, 399, 438, 481, 529, 581,
                                    639, 702, 772, 849, 933, 1026, 1128, 1240, 1364, 1500 };
    if( i_length < 1 )
    {
        return 0;
    }
    if( i_length > 20 )
    {
        i_length = 20;
    }
    return p_rank[i_length - 1];
}
