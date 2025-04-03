 *****************************************************************************/
static void SubpicDestroyClone( subpicture_t *p_subpic )
{
    p_subpic->p_region = NULL; /* don't destroy region */
    subpicture_Delete( p_subpic );
}
