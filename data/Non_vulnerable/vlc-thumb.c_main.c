}
int main(int argc, const char **argv)
{
    const char *in;
    char *out, *out_with_ext;
    int width;
    pthread_condattr_t attr;
    libvlc_instance_t *libvlc;
    libvlc_media_player_t *mp;
    libvlc_media_t *m;
    /* mandatory to support UTF-8 filenames (provided the locale is well set)*/
    setlocale(LC_ALL, "");
    cmdline(argc, argv, &in, &out, &out_with_ext, &width);
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&wait, &attr);
    pthread_condattr_destroy(&attr);
    /* starts vlc */
    libvlc = create_libvlc();
    assert(libvlc);
    m = libvlc_media_new_path(libvlc, in);
    assert(m);
    mp = libvlc_media_player_new_from_media(m);
    assert(mp);
    libvlc_media_player_play(mp);
    /* takes snapshot */
    set_position(mp);
    snapshot(mp, width, out_with_ext);
    libvlc_media_player_stop(mp);
    /* clean up */
    if (out != out_with_ext) {
        rename(out_with_ext, out);
        free(out_with_ext);
    }
    free(out);
    libvlc_media_player_release(mp);
    libvlc_media_release(m);
    libvlc_release(libvlc);
    pthread_cond_destroy(&wait);
    return 0;
}
