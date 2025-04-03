}
struct _Z_InputQ *Z_GetFirstComplete()
{
    struct _Z_InputQ *qptr;
    qptr = __Q_Head;
    while (qptr) {
	if (qptr->complete)
	    return (qptr);
	qptr = qptr->next;
    }
    return ((struct _Z_InputQ *)0);
}
