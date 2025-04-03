#undef msleep
void msleep (mtime_t delay)
{
    mwait (mdate () + delay);
}
