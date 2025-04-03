 */
void sout_StreamChainDelete(sout_stream_t *p_first, sout_stream_t *p_last)
{
    while(p_first != NULL)
    {
        sout_stream_t *p_next = p_first->p_next;
        sout_StreamDelete(p_first);
        if(p_first == p_last)
           break;
        p_first = p_next;
    }
}
