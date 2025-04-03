}
static void set_position(libvlc_media_player_t *mp)
{
    libvlc_event_manager_t *em = libvlc_media_player_event_manager(mp);
    assert(em);
    libvlc_event_attach(em, libvlc_MediaPlayerPositionChanged, callback, NULL);
    done = false;
    libvlc_media_player_set_position(mp, VLC_THUMBNAIL_POSITION);
    event_wait("Couldn't set position");
    libvlc_event_detach(em, libvlc_MediaPlayerPositionChanged, callback, NULL);
}
