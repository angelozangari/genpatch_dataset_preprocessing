}
int read_sms_real(int type, double *x, double *y, double *z)
{
    int _x, _y, _z;
    int xscale, yscale, zscale;
    int ret;
    Boolean ok;
    ret = read_sms_raw(type, &_x, &_y, &_z);
    if ( !ret )
        return 0;
    static CFStringRef app = CFSTR("com.ramsayl.UniMotion");
    static CFStringRef xscalestr = CFSTR("x_scale");
    static CFStringRef yscalestr = CFSTR("y_scale");
    static CFStringRef zscalestr = CFSTR("z_scale");
    xscale = CFPreferencesGetAppIntegerValue(xscalestr, app, &ok);
    if ( !ok ) return 0;
    yscale = CFPreferencesGetAppIntegerValue(yscalestr, app, &ok);
    if ( !ok ) return 0;
    zscale = CFPreferencesGetAppIntegerValue(zscalestr, app, &ok);
    if ( !ok ) return 0;
    *x = _x / (double)xscale;
    *y = _y / (double)yscale;
    *z = _z / (double)zscale;
    return 1;
}
