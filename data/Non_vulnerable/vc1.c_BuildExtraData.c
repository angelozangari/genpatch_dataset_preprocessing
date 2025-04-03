/* BuildExtraData: gather sequence header and entry point */
static void BuildExtraData( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    es_format_t *p_es = &p_dec->fmt_out;
    int i_extra;
    if( !p_sys->b_sequence_header || !p_sys->b_entry_point )
        return;
    i_extra = p_sys->sh.p_sh->i_buffer + p_sys->ep.p_ep->i_buffer;
    if( p_es->i_extra != i_extra )
    {
        p_es->i_extra = i_extra;
        p_es->p_extra = xrealloc( p_es->p_extra, p_es->i_extra );
    }
    memcpy( p_es->p_extra,
            p_sys->sh.p_sh->p_buffer, p_sys->sh.p_sh->i_buffer );
    memcpy( (uint8_t*)p_es->p_extra + p_sys->sh.p_sh->i_buffer,
            p_sys->ep.p_ep->p_buffer, p_sys->ep.p_ep->i_buffer );
}
