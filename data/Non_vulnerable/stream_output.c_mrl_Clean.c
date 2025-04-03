/* mrl_Clean: clean p_mrl  after a call to mrl_Parse */
static void mrl_Clean( mrl_t *p_mrl )
{
    FREENULL( p_mrl->psz_access );
    FREENULL( p_mrl->psz_way );
    FREENULL( p_mrl->psz_name );
}
