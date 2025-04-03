}
static int spu_get_attachments(filter_t *filter,
                               input_attachment_t ***attachment_ptr,
                               int *attachment_count)
{
    spu_t *spu = filter->p_owner->spu;
    int ret = VLC_EGENERIC;
    if (spu->p->input)
        ret = input_Control((input_thread_t*)spu->p->input,
                            INPUT_GET_ATTACHMENTS,
                            attachment_ptr, attachment_count);
    return ret;
}
