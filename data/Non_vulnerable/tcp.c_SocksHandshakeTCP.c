 *****************************************************************************/
static int SocksHandshakeTCP( vlc_object_t *p_obj,
                              int fd,
                              int i_socks_version,
                              const char *psz_user, const char *psz_passwd,
                              const char *psz_host, int i_port )
{
    uint8_t buffer[128+2*256];
    if( i_socks_version != 4 && i_socks_version != 5 )
    {
        msg_Warn( p_obj, "invalid socks protocol version %d", i_socks_version );
        i_socks_version = 5;
    }
    if( i_socks_version == 5 &&
        SocksNegotiate( p_obj, fd, i_socks_version,
                        psz_user, psz_passwd ) )
        return VLC_EGENERIC;
    if( i_socks_version == 4 )
    {
        /* v4 only support ipv4 */
        static const struct addrinfo hints = {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM,
            .ai_protocol = IPPROTO_TCP,
            .ai_flags = AI_IDN,
        };
        struct addrinfo *res;
        if (vlc_getaddrinfo (psz_host, 0, &hints, &res))
            return VLC_EGENERIC;
        buffer[0] = i_socks_version;
        buffer[1] = 0x01;               /* CONNECT */
        SetWBE( &buffer[2], i_port );   /* Port */
        memcpy (&buffer[4],             /* Address */
                &((struct sockaddr_in *)(res->ai_addr))->sin_addr, 4);
        freeaddrinfo (res);
        buffer[8] = 0;                  /* Empty user id */
        if( net_Write( p_obj, fd, NULL, buffer, 9 ) != 9 )
            return VLC_EGENERIC;
        if( net_Read( p_obj, fd, NULL, buffer, 8, true ) != 8 )
            return VLC_EGENERIC;
        msg_Dbg( p_obj, "socks: v=%d cd=%d",
                 buffer[0], buffer[1] );
        if( buffer[1] != 90 )
            return VLC_EGENERIC;
    }
    else if( i_socks_version == 5 )
    {
        int i_hlen = __MIN(strlen( psz_host ), 255);
        int i_len;
        buffer[0] = i_socks_version;    /* Version */
        buffer[1] = 0x01;               /* Cmd: connect */
        buffer[2] = 0x00;               /* Reserved */
        buffer[3] = 3;                  /* ATYP: for now domainname */
        buffer[4] = i_hlen;
        memcpy( &buffer[5], psz_host, i_hlen );
        SetWBE( &buffer[5+i_hlen], i_port );
        i_len = 5 + i_hlen + 2;
        if( net_Write( p_obj, fd, NULL, buffer, i_len ) != i_len )
            return VLC_EGENERIC;
        /* Read the header */
        if( net_Read( p_obj, fd, NULL, buffer, 5, true ) != 5 )
            return VLC_EGENERIC;
        msg_Dbg( p_obj, "socks: v=%d rep=%d atyp=%d",
                 buffer[0], buffer[1], buffer[3] );
        if( buffer[1] != 0x00 )
        {
            msg_Err( p_obj, "socks: CONNECT request failed" );
            return VLC_EGENERIC;
        }
        /* Read the remaining bytes */
        if( buffer[3] == 0x01 )
            i_len = 4-1 + 2;
        else if( buffer[3] == 0x03 )
            i_len = buffer[4] + 2;
        else if( buffer[3] == 0x04 )
            i_len = 16-1+2;
        else
            return VLC_EGENERIC;
        if( net_Read( p_obj, fd, NULL, buffer, i_len, true ) != i_len )
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
