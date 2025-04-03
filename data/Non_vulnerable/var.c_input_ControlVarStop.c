 *****************************************************************************/
void input_ControlVarStop( input_thread_t *p_input )
{
    if( !p_input->b_preparsing )
        InputDelCallbacks( p_input, p_input_callbacks );
    if( p_input->p->i_title > 0 )
    {
        char name[sizeof("title ") + 5 ];
        int i;
        InputDelCallbacks( p_input, p_input_navigation_callbacks );
        InputDelCallbacks( p_input, p_input_title_callbacks );
        for( i = 0; i < p_input->p->i_title; i++ )
        {
            snprintf( name, sizeof(name), "title %2i", i );
            var_DelCallback( p_input, name, NavigationCallback, (void *)(intptr_t)i );
        }
    }
}
