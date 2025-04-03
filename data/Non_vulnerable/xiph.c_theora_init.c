 */
void *theora_init (demux_t *demux)
{
    (void)demux;
    return xiph_init (false);
}
