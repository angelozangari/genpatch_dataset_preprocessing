static bool done;
static void callback(const libvlc_event_t *ev, void *param)
{
    float new_position;
    (void)param;
    pthread_mutex_lock(&lock);
    switch (ev->type) {
    case libvlc_MediaPlayerPositionChanged:
        new_position = ev->u.media_player_position_changed.new_position;
        if (new_position < VLC_THUMBNAIL_POSITION * .9 /* 90% margin */)
            break;
    case libvlc_MediaPlayerSnapshotTaken:
        done = true;
        pthread_cond_signal(&wait);
        break;
    default:
        assert(0);
    }
    pthread_mutex_unlock(&lock);
}
