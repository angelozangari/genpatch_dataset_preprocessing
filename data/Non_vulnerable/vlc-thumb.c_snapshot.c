}
static void snapshot(libvlc_media_player_t *mp, int width, char *out_with_ext)
{
    libvlc_event_manager_t *em = libvlc_media_player_event_manager(mp);
    assert(em);
    libvlc_event_attach(em, libvlc_MediaPlayerSnapshotTaken, callback, NULL);
    done = false;
    libvlc_video_take_snapshot(mp, 0, out_with_ext, width, 0);
    event_wait("Snapshot has not been written");
    libvlc_event_detach(em, libvlc_MediaPlayerSnapshotTaken, callback, NULL);
}
