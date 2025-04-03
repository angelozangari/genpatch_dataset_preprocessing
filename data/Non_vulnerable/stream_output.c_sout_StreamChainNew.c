 */
sout_stream_t *sout_StreamChainNew(sout_instance_t *p_sout, char *psz_chain,
                                sout_stream_t *p_next, sout_stream_t **pp_last)
{
    if(!psz_chain || !*psz_chain)
    {
        if(pp_last) *pp_last = NULL;
        return p_next;
    }
    char *psz_parser = strdup(psz_chain);
    if(!psz_parser)
        return NULL;
    vlc_array_t cfg, name;
    vlc_array_init(&cfg);
    vlc_array_init(&name);
    /* parse chain */
    while(psz_parser)
    {
        config_chain_t *p_cfg;
        char *psz_name;
        psz_chain = config_ChainCreate( &psz_name, &p_cfg, psz_parser );
        free( psz_parser );
        psz_parser = psz_chain;
        vlc_array_append(&cfg, p_cfg);
        vlc_array_append(&name, psz_name);
    }
    int i = vlc_array_count(&name);
    vlc_array_t module;
    vlc_array_init(&module);
    while(i--)
    {
        p_next = sout_StreamNew( p_sout, vlc_array_item_at_index(&name, i),
            vlc_array_item_at_index(&cfg, i), p_next);
        if(!p_next)
            goto error;
        if(i == vlc_array_count(&name) - 1 && pp_last)
            *pp_last = p_next;   /* last module created in the chain */
        vlc_array_append(&module, p_next);
    }
    vlc_array_clear(&name);
    vlc_array_clear(&cfg);
    vlc_array_clear(&module);
    return p_next;
error:
    i++;    /* last module couldn't be created */
    /* destroy all modules created, starting with the last one */
    int modules = vlc_array_count(&module);
    while(modules--)
        sout_StreamDelete(vlc_array_item_at_index(&module, modules));
    vlc_array_clear(&module);
    /* then destroy all names and config which weren't destroyed by
     * sout_StreamDelete */
    while(i--)
    {
        free(vlc_array_item_at_index(&name, i));
        config_ChainDestroy(vlc_array_item_at_index(&cfg, i));
    }
    vlc_array_clear(&name);
    vlc_array_clear(&cfg);
    return NULL;
}
