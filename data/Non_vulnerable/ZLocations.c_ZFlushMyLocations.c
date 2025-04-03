}
Code_t ZFlushMyLocations()
{
    return (Z_SendLocation(LOGIN_CLASS, LOGIN_USER_FLUSH, ZAUTH, ""));
}
