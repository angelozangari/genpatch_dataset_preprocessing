 *****************************************************************************/
static subpicture_t *SubpicClone( subpicture_t *p_source, subpicture_updater_t *updater )
{
    subpicture_t *p_subpic;
    subpicture_updater_t subpic_updater;
    subpicture_private_t *p_subpic_private;
    p_subpic = subpicture_New( updater );
    if( !p_subpic )
    {
        return NULL;
    }
    /* save private members */
    subpic_updater = p_subpic->updater;
    p_subpic_private = p_subpic->p_private;
    /* copy the entire struct */
    memcpy( p_subpic, p_source, sizeof( subpicture_t ) );
    /* restore private members */
    p_subpic->updater = subpic_updater;
    p_subpic->p_private = p_subpic_private;
    return p_subpic;
}
