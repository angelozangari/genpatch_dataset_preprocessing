#define reportResult(result,operation) (_reportResult((result),(operation),strrchr(__FILE__, '/')+1,__LINE__))
static inline bool _reportResult(kern_return_t result, const char *operation, const char* file, int line) {
    if ( result != ERR_SUCCESS ) {
        printf("%s:%d: %s: %s\n", file, line, operation, mach_error_string(result));
        return false;
    }
    return true;
}
#define reportResult(result,operation) (_reportResult((result),(operation),strrchr(__FILE__, '/')+1,__LINE__))
static inline bool _reportResult(kern_return_t result, const char *operation, const char* file, int line) {
    if ( result != ERR_SUCCESS ) {
        printf("%s:%d: %s: %s\n", file, line, operation, mach_error_string(result));
        return false;
    }
    return true;
}
