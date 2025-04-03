 *****************************************************************************/
static bool SubsdelayIsTextEmpty( char *psz_text )
{
    if( !psz_text )
    {
        return false;
    }
    psz_text += strspn( psz_text, " " );
    return !( *psz_text );
}
