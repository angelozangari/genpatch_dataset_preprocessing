}
static bool BetterFormat (vlc_fourcc_t a, const vlc_fourcc_t *tab,
                          unsigned *rankp)
{
    for (unsigned i = 0, max = *rankp; i < max && tab[i] != 0; i++)
        if (tab[i] == a)
        {
            *rankp = i;
            return true;
        }
    return false;
}
