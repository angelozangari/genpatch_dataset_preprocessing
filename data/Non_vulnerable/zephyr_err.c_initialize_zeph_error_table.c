static struct et_list link = { 0, 0 };
void initialize_zeph_error_table (NOARGS) {
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}
static struct et_list link = { 0, 0 };
void initialize_zeph_error_table (NOARGS) {
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}
