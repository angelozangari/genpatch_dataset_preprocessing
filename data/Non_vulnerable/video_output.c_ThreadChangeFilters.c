} vout_filter_t;
static void ThreadChangeFilters(vout_thread_t *vout,
                                const video_format_t *source,
                                const char *filters,
                                bool is_locked)
{
    ThreadFilterFlush(vout, is_locked);
    vlc_array_t array_static;
    vlc_array_t array_interactive;
    vlc_array_init(&array_static);
    vlc_array_init(&array_interactive);
    char *current = filters ? strdup(filters) : NULL;
    while (current) {
        config_chain_t *cfg;
        char *name;
        char *next = config_ChainCreate(&name, &cfg, current);
        if (name && *name) {
            vout_filter_t *e = xmalloc(sizeof(*e));
            e->name = name;
            e->cfg  = cfg;
            if (!strcmp(e->name, "deinterlace") ||
                !strcmp(e->name, "postproc")) {
                vlc_array_append(&array_static, e);
            } else {
                vlc_array_append(&array_interactive, e);
            }
        } else {
            if (cfg)
                config_ChainDestroy(cfg);
            free(name);
        }
        free(current);
        current = next;
    }
    if (!is_locked)
        vlc_mutex_lock(&vout->p->filter.lock);
    es_format_t fmt_target;
    es_format_InitFromVideo(&fmt_target, source ? source : &vout->p->filter.format);
    es_format_t fmt_current = fmt_target;
    for (int a = 0; a < 2; a++) {
        vlc_array_t    *array = a == 0 ? &array_static :
                                         &array_interactive;
        filter_chain_t *chain = a == 0 ? vout->p->filter.chain_static :
                                         vout->p->filter.chain_interactive;
        filter_chain_Reset(chain, &fmt_current, &fmt_current);
        for (int i = 0; i < vlc_array_count(array); i++) {
            vout_filter_t *e = vlc_array_item_at_index(array, i);
            msg_Dbg(vout, "Adding '%s' as %s", e->name, a == 0 ? "static" : "interactive");
            if (!filter_chain_AppendFilter(chain, e->name, e->cfg, NULL, NULL)) {
                msg_Err(vout, "Failed to add filter '%s'", e->name);
                config_ChainDestroy(e->cfg);
            }
            free(e->name);
            free(e);
        }
        fmt_current = *filter_chain_GetFmtOut(chain);
        vlc_array_clear(array);
    }
    if (!es_format_IsSimilar(&fmt_current, &fmt_target)) {
        msg_Dbg(vout, "Adding a filter to compensate for format changes");
        if (!filter_chain_AppendFilter(vout->p->filter.chain_interactive, NULL, NULL,
                                       &fmt_current, &fmt_target)) {
            msg_Err(vout, "Failed to compensate for the format changes, removing all filters");
            filter_chain_Reset(vout->p->filter.chain_static,      &fmt_target, &fmt_target);
            filter_chain_Reset(vout->p->filter.chain_interactive, &fmt_target, &fmt_target);
        }
    }
    es_format_Clean(&fmt_target);
    if (vout->p->filter.configuration != filters) {
        free(vout->p->filter.configuration);
        vout->p->filter.configuration = filters ? strdup(filters) : NULL;
    }
    if (source) {
        video_format_Clean(&vout->p->filter.format);
        video_format_Copy(&vout->p->filter.format, source);
    }
    if (!is_locked)
        vlc_mutex_unlock(&vout->p->filter.lock);
}
