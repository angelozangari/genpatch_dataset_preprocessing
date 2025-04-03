 *****************************************************************************/
static bool SubpicIsEmpty( subpicture_t* p_subpic )
{
    return ( p_subpic->p_region && ( SubsdelayIsTextEmpty( p_subpic->p_region->psz_html ) ||
                                     SubsdelayIsTextEmpty( p_subpic->p_region->psz_text ) ) );
}
