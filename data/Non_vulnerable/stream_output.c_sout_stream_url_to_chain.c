}
static char *sout_stream_url_to_chain( bool b_sout_display,
                                       const char *psz_url )
{
    mrl_t       mrl;
    char        *psz_chain;
    mrl_Parse( &mrl, psz_url );
    /* Check if the URLs goes to #rtp - otherwise we'll use #standard */
    static const char rtplist[] = "dccp\0sctp\0tcp\0udplite\0";
    for (const char *a = rtplist; *a; a += strlen (a) + 1)
        if (strcmp (a, mrl.psz_access) == 0)
            goto rtp;
    if (strcmp (mrl.psz_access, "rtp") == 0)
    {
        char *port;
        /* For historical reasons, rtp:// means RTP over UDP */
        strcpy (mrl.psz_access, "udp");
rtp:
        if (mrl.psz_name[0] == '[')
        {
            port = strstr (mrl.psz_name, "]:");
            if (port != NULL)
                port++;
        }
        else
            port = strchr (mrl.psz_name, ':');
        if (port != NULL)
            *port++ = '\0'; /* erase ':' */
        if (asprintf (&psz_chain,
                      "rtp{mux=\"%s\",proto=\"%s\",dst=\"%s%s%s\"}",
                      mrl.psz_way, mrl.psz_access, mrl.psz_name,
                      port ? "\",port=\"" : "", port ? port : "") == -1)
            psz_chain = NULL;
    }
    else
    {
        /* Convert the URL to a basic standard sout chain */
        if (asprintf (&psz_chain,
                      "standard{mux=\"%s\",access=\"%s\",dst=\"%s\"}",
                      mrl.psz_way, mrl.psz_access, mrl.psz_name) == -1)
            psz_chain = NULL;
    }
    /* Duplicate and wrap if sout-display is on */
    if (psz_chain && b_sout_display)
    {
        char *tmp;
        if (asprintf (&tmp, "duplicate{dst=display,dst=%s}", psz_chain) == -1)
            tmp = NULL;
        free (psz_chain);
        psz_chain = tmp;
    }
    mrl_Clean( &mrl );
    return psz_chain;
}
