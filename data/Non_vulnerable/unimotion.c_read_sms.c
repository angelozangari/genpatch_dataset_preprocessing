}
int read_sms(int type, int *x, int *y, int *z)
{
    int _x, _y, _z;
    int xoff, yoff, zoff;
    Boolean ok;
    int ret;
    ret = read_sms_raw(type, &_x, &_y, &_z);
    if ( !ret )
        return 0;
    static CFStringRef app = CFSTR("com.ramsayl.UniMotion");
    static CFStringRef xoffstr = CFSTR("x_offset");
    static CFStringRef yoffstr = CFSTR("y_offset");
    static CFStringRef zoffstr = CFSTR("z_offset");
    xoff = CFPreferencesGetAppIntegerValue(xoffstr, app, &ok);
    if ( ok ) _x += xoff;
    yoff = CFPreferencesGetAppIntegerValue(yoffstr, app, &ok);
    if ( ok ) _y += yoff;
    zoff = CFPreferencesGetAppIntegerValue(zoffstr, app, &ok);
    if ( ok ) _z += zoff;
    *x = _x;
    *y = _y;
    *z = _z;
    return ret;
}
