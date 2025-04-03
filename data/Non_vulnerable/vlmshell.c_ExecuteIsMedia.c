}
static bool ExecuteIsMedia( vlm_t *p_vlm, const char *psz_name )
{
    int64_t id;
    if( !psz_name || vlm_ControlInternal( p_vlm, VLM_GET_MEDIA_ID, psz_name, &id ) )
        return false;
    return true;
}
