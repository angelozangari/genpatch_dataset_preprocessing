 */
void vlc_UrlClean (vlc_url_t *restrict url)
{
    free (url->psz_host);
    free (url->psz_buffer);
}
