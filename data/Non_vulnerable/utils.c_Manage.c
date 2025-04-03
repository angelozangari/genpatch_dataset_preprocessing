extern void jni_getMouseCoordinates(int *, int *, int *, int *);
void Manage(vout_display_t *vd)
{
    int x, y, button, action;
    jni_getMouseCoordinates(&action, &button, &x, &y);
    if (x >= 0 && y >= 0)
    {
        switch( action )
        {
            case AMOTION_EVENT_ACTION_DOWN:
                vout_display_SendEventMouseMoved(vd, x, y);
                vout_display_SendEventMousePressed(vd, button); break;
            case AMOTION_EVENT_ACTION_UP:
                vout_display_SendEventMouseMoved(vd, x, y);
                vout_display_SendEventMouseReleased(vd, button); break;
            case AMOTION_EVENT_ACTION_MOVE:
                vout_display_SendEventMouseMoved(vd, x, y); break;
        }
    }
}
