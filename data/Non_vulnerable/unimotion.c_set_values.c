};
static int set_values(int type, int *kernFunc, char **servMatch, int *dataType)
{
    switch ( type ) {
        case powerbook:
            *kernFunc = 21;
            *servMatch = "IOI2CMotionSensor";
            *dataType = PB_IB;
            break;
        case ibook:
            *kernFunc = 21;
            *servMatch = "IOI2CMotionSensor";
            *dataType = PB_IB;
            break;
        case highrespb:
            *kernFunc = 21;
            *servMatch = "PMUMotionSensor";
            *dataType = PB_IB;
            break;
        case macbookpro:
            *kernFunc = 5;
            *servMatch = "SMCMotionSensor";
            *dataType = MBP;
            break;
        default:
            return 0;
    }
    return 1;
}
