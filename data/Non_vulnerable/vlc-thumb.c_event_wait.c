}
static void event_wait(const char *error)
{
    int ret;
    struct timespec ts;
#define VLC_THUMBNAIL_TIMEOUT   5.0 /* 5 secs */
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += VLC_THUMBNAIL_TIMEOUT;
    pthread_mutex_lock(&lock);
    ret = done ? 0 : pthread_cond_timedwait(&wait, &lock, &ts);
    pthread_mutex_unlock(&lock);
    assert(!ret || ret == ETIMEDOUT);
    if (ret) {
        fprintf(stderr,
                "%s (timeout after %.2f secs!\n", error, VLC_THUMBNAIL_TIMEOUT);
        exit(1);
    }
}
