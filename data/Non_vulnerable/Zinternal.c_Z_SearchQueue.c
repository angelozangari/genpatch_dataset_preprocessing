 */
static struct _Z_InputQ *Z_SearchQueue(ZUnique_Id_t *uid, ZNotice_Kind_t kind)
{
    register struct _Z_InputQ *qptr;
    struct _Z_InputQ *next;
    struct timeval tv;
    (void) gettimeofday(&tv, (struct timezone *)0);
    qptr = __Q_Head;
    while (qptr) {
	if (ZCompareUID(uid, &qptr->uid) && qptr->kind == kind)
	    return (qptr);
	next = qptr->next;
	if (qptr->timep && ((time_t)qptr->timep+Z_NOTICETIMELIMIT < tv.tv_sec))
	    Z_RemQueue(qptr);
	qptr = next;
    }
    return (NULL);
}
