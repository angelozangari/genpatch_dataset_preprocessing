}
static int probe_sms(int kernFunc, char *servMatch, int dataType, void *data)
{
    kern_return_t result;
    mach_port_t masterPort;
    io_iterator_t iterator;
    io_object_t aDevice;
    io_connect_t  dataPort;
    size_t structureInputSize;
    size_t structureOutputSize;
    union motion_data inputStructure;
    union motion_data *outputStructure;
    outputStructure = (union motion_data *)data;
    result = IOMasterPort(MACH_PORT_NULL, &masterPort);
    CFMutableDictionaryRef matchingDictionary = IOServiceMatching(servMatch);
    result = IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
    if (result != KERN_SUCCESS) {
        //fputs("IOServiceGetMatchingServices returned error.\n", stderr);
        return 0;
    }
    aDevice = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (aDevice == 0) {
        //fputs("No motion sensor available\n", stderr);
        return 0;
    }
    result = IOServiceOpen(aDevice, mach_task_self(), 0, &dataPort);
    IOObjectRelease(aDevice);
    if (result != KERN_SUCCESS) {
        //fputs("Could not open motion sensor device\n", stderr);
        return 0;
    }
    switch ( dataType ) {
        case PB_IB:
            structureInputSize = sizeof(struct pb_ib_data);
            structureOutputSize = sizeof(struct pb_ib_data);
            break;
        case MBP:
            structureInputSize = sizeof(struct mbp_data);
            structureOutputSize = sizeof(struct mbp_data);
            break;
        default:
            return 0;
    }
    memset(&inputStructure, 0, sizeof(union motion_data));
    memset(outputStructure, 0, sizeof(union motion_data));
    result = IOConnectCallStructMethod(dataPort, kernFunc, &inputStructure, 
                structureInputSize, outputStructure, &structureOutputSize );
    IOServiceClose(dataPort);
    if (result != KERN_SUCCESS) {
        //puts("no coords");
        return 0;
    }
    return 1;
}
