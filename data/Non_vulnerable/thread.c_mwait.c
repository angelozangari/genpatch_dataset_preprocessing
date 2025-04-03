#undef mwait
void mwait (mtime_t deadline)
{
    mtime_t delay;
    vlc_testcancel();
    while ((delay = (deadline - mdate())) > 0)
    {
        delay /= 1000;
        if (unlikely(delay > 0x7fffffff))
            delay = 0x7fffffff;
        vlc_Sleep (delay);
        vlc_testcancel();
    }
}
