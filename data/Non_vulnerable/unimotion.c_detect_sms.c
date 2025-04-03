}
int detect_sms()
{
    int kernFunc;
    char *servMatch;
    int dataType;
    union motion_data data;
    int i;
    for ( i = 1; ; i++ ) {
        if ( !set_values(i, &kernFunc, &servMatch, &dataType) )
            break;
        if ( probe_sms(kernFunc, servMatch, dataType, &data) )
            return i;
    }
    return unknown;
}
