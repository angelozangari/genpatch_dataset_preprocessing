static mtime_t (*mdate_selected) (void) = mdate_giveup;
mtime_t mdate (void)
{
    return mdate_selected ();
}
