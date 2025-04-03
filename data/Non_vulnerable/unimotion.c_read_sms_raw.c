}
int read_sms_raw(int type, int *x, int *y, int *z)
{
    int kernFunc;
    char *servMatch;
    int dataType;
    union motion_data data;
    if ( !set_values(type, &kernFunc, &servMatch, &dataType) )
        return 0;
    if ( probe_sms(kernFunc, servMatch, dataType, &data) ) {
        switch ( dataType ) {
            case PB_IB:
                if ( x ) *x = data.pb_ib.x;
                if ( y ) *y = data.pb_ib.y;
                if ( z ) *z = data.pb_ib.z;
                break;
            case MBP:
                if ( x ) *x = data.mbp.x;
                if ( y ) *y = data.mbp.y;
                if ( z ) *z = data.mbp.z;
                break;
            default:
                return 0;
        }
        return 1;
    }
    return 0;
}
