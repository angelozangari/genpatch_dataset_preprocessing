#include <assert.h>
LRESULT DecodeGesture( vlc_object_t *p_this, win32_gesture_sys_t *p_gesture,
                       HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( !p_gesture )
        return DefWindowProc( hWnd, message, wParam, lParam );
    GESTUREINFO gi;
    ZeroMemory( &gi, sizeof( GESTUREINFO ) );
    gi.cbSize = sizeof( GESTUREINFO );
    BOOL bResult  = p_gesture->OurGetGestureInfo((HGESTUREINFO)lParam, &gi);
    BOOL bHandled = FALSE; /* Needed to release the handle */
    if( bResult )
    {
        switch ( gi.dwID )
        {
            case GID_BEGIN:
                /* Set the win32_gesture_sys_t values */
                p_gesture->i_beginx      = gi.ptsLocation.x;
                p_gesture->i_beginy      = gi.ptsLocation.y;
                p_gesture->i_lasty       = p_gesture->i_beginy;
                p_gesture->b_2fingers    = false;
                break;
            case GID_END:
                if( p_gesture->i_type != 0 &&
                    p_gesture->i_action == GESTURE_ACTION_JUMP )
                {
                    int action_id;
                    if( p_gesture->i_beginx > gi.ptsLocation.x )
                    {
                        if( p_gesture->b_2fingers )
                            action_id = ACTIONID_JUMP_BACKWARD_MEDIUM;
                        else
                            action_id = ACTIONID_JUMP_BACKWARD_SHORT;
                    }
                    else
                    {
                        if( p_gesture->b_2fingers )
                            action_id = ACTIONID_JUMP_FORWARD_MEDIUM;
                        else
                            action_id = ACTIONID_JUMP_FORWARD_SHORT;
                    }
                    var_SetInteger( p_this->p_libvlc, "key-action", action_id );
                }
                /* Reset the values */
                p_gesture->i_action = GESTURE_ACTION_UNDEFINED;
                p_gesture->i_type = p_gesture->i_beginx = p_gesture->i_beginy = -1;
                p_gesture->b_2fingers = false;
                break;
            case GID_PAN:
                p_gesture->i_type = GID_PAN;
                bHandled = TRUE;
                if( (DWORD)gi.ullArguments > 0 )
                    p_gesture->b_2fingers = true;
                if( p_gesture->i_action == GESTURE_ACTION_UNDEFINED )
                {
                    if( abs( p_gesture->i_beginx - gi.ptsLocation.x ) +
                        abs( p_gesture->i_beginy - gi.ptsLocation.y ) > 50 )
                    {
                        if( abs( p_gesture->i_beginx - gi.ptsLocation.x ) >
                            abs( p_gesture->i_beginy - gi.ptsLocation.y ) )
                           p_gesture->i_action =  GESTURE_ACTION_JUMP;
                        else if ( p_gesture->b_2fingers )
                           p_gesture->i_action = GESTURE_ACTION_BRIGHTNESS;
                        else
                           p_gesture->i_action =  GESTURE_ACTION_VOLUME;
                    }
                }
                if( p_gesture->i_action == GESTURE_ACTION_VOLUME )
                {
                    int offset = p_gesture->i_lasty - gi.ptsLocation.y;
                    if( offset > 100)
                        var_SetInteger( p_this->p_libvlc, "key-action", ACTIONID_VOL_UP );
                    else if( offset < -100)
                        var_SetInteger( p_this->p_libvlc, "key-action", ACTIONID_VOL_DOWN );
                    else
                        break;
                    p_gesture->i_lasty = gi.ptsLocation.y;
                }
                else if ( p_gesture->i_action == GESTURE_ACTION_BRIGHTNESS )
                {
                    /* Currently unimplemented
                    if( p_gesture->i_lasty == -1 )
                        p_gesture->i_lasty = p_gesture->i_beginy;
                    if( p_gesture->i_lasty - p_gesture->i_beginy > 80 )
                    {
                        var_SetInteger( p_this->p_libvlc, "key-action", ACTIONID_BRIGHTNESS_DOWN );
                        p_gesture->i_lasty = gi.ptsLocation.y;
                    }
                    else if ( p_gesture->i_lasty - p_gesture->i_beginy < 80 )
                    {
                        var_SetInteger( p_this->p_libvlc, "key-action", ACTIONID_BRIGHTNESS_UP );
                        p_gesture->i_lasty = gi.ptsLocation.y;
                    } */
                }
                break;
            case GID_TWOFINGERTAP:
                p_gesture->i_type = GID_TWOFINGERTAP;
                var_SetInteger( p_this->p_libvlc, "key-action", ACTIONID_PLAY_PAUSE );
                bHandled = TRUE;
                break;
            case GID_ZOOM:
                p_gesture->i_type = GID_ZOOM;
                switch( gi.dwFlags )
                {
                    case GF_BEGIN:
                        p_gesture->i_ullArguments = gi.ullArguments;
                        break;
                    case GF_END:
                        {
                            double k = (double)(gi.ullArguments) /
                                       (double)(p_gesture->i_ullArguments);
                            if( k > 1 )
                                var_SetInteger( p_this->p_libvlc, "key-action",
                                        ACTIONID_TOGGLE_FULLSCREEN );
                            else
                                var_SetInteger( p_this->p_libvlc, "key-action",
                                        ACTIONID_LEAVE_FULLSCREEN );
                        }
                        break;
                    default:
                        msg_Err( p_this, "Unmanaged dwFlag: %lx", gi.dwFlags );
                }
                bHandled = TRUE;
                break;
            case WM_VSCROLL:
                bHandled = TRUE;
                break;
            default:
                break;
        }
    }
    else
    {
        DWORD dwErr = GetLastError();
        if( dwErr > 0 )
            msg_Err( p_this, "Could not retrieve a valid GESTUREINFO structure" );
    }
    if( bHandled )
    {
        /* Close the Handle, if we handled the gesture, a contrario
         * from the doc example */
        p_gesture->OurCloseGestureInfoHandle((HGESTUREINFO)lParam);
        return 0;
    }
    else
        return DefWindowProc( hWnd, message, wParam, lParam );
}
