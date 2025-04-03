#define MAX_LINE_LENGTH 1024
static bool ReadWin32( HANDLE *hConsoleIn, char *p_buffer, int *pi_size )
{
    INPUT_RECORD input_record;
    DWORD i_dw;
    while( *pi_size < MAX_LINE_LENGTH &&
           ReadConsoleInput( hConsoleIn, &input_record, 1, &i_dw ) )
    {
        if( input_record.EventType != KEY_EVENT ||
            !input_record.Event.KeyEvent.bKeyDown ||
            input_record.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT ||
            input_record.Event.KeyEvent.wVirtualKeyCode == VK_CONTROL||
            input_record.Event.KeyEvent.wVirtualKeyCode == VK_MENU ||
            input_record.Event.KeyEvent.wVirtualKeyCode == VK_CAPITAL )
        {
            /* nothing interesting */
            continue;
        }
        p_buffer[ *pi_size ] = input_record.Event.KeyEvent.uChar.AsciiChar;
        /* Echo out the command */
        putc( p_buffer[ *pi_size ], stdout );
        /* Handle special keys */
        if( p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
        {
            if ( p_buffer[ *pi_size ] == '\r' )
                p_buffer[ *pi_size ] = '\n';
            (*pi_size)++; /* We want the \n to be in the output string */
            putc( '\n', stdout );
            break;
        }
        switch( p_buffer[ *pi_size ] )
        {
        case '\b':
            if( *pi_size )
            {
                *pi_size -= 2;
                putc( ' ', stdout );
                putc( '\b', stdout );
            }
            break;
        //case '\r':
        //    (*pi_size) --;
        //    break;
        }
        (*pi_size)++;
    }
    if( *pi_size == MAX_LINE_LENGTH )
      // p_buffer[ *pi_size ] == '\r' || p_buffer[ *pi_size ] == '\n' )
    {
        p_buffer[ *pi_size ] = 0;
        return true;
    }
    return false;
}
