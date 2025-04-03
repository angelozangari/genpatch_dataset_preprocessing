}
static int ExecuteHelp( vlm_message_t **pp_status )
{
    vlm_message_t *message_child;
#define MessageAdd( a ) \
        vlm_MessageAdd( *pp_status, vlm_MessageSimpleNew( a ) );
#define MessageAddChild( a ) \
        vlm_MessageAdd( message_child, vlm_MessageSimpleNew( a ) );
    *pp_status = vlm_MessageSimpleNew( "help" );
    message_child = MessageAdd( "Commands Syntax:" );
    MessageAddChild( "new (name) vod|broadcast|schedule [properties]" );
    MessageAddChild( "setup (name) (properties)" );
    MessageAddChild( "show [(name)|media|schedule]" );
    MessageAddChild( "del (name)|all|media|schedule" );
    MessageAddChild( "control (name) [instance_name] (command)" );
    MessageAddChild( "save (config_file)" );
    MessageAddChild( "export" );
    MessageAddChild( "load (config_file)" );
    message_child = MessageAdd( "Media Proprieties Syntax:" );
    MessageAddChild( "input (input_name)" );
    MessageAddChild( "inputdel (input_name)|all" );
    MessageAddChild( "inputdeln input_number" );
    MessageAddChild( "output (output_name)" );
    MessageAddChild( "option (option_name)[=value]" );
    MessageAddChild( "enabled|disabled" );
    MessageAddChild( "loop|unloop (broadcast only)" );
    MessageAddChild( "mux (mux_name)" );
    message_child = MessageAdd( "Schedule Proprieties Syntax:" );
    MessageAddChild( "enabled|disabled" );
    MessageAddChild( "append (command_until_rest_of_the_line)" );
    MessageAddChild( "date (year)/(month)/(day)-(hour):(minutes):"
                     "(seconds)|now" );
    MessageAddChild( "period (years_aka_12_months)/(months_aka_30_days)/"
                     "(days)-(hours):(minutes):(seconds)" );
    MessageAddChild( "repeat (number_of_repetitions)" );
    message_child = MessageAdd( "Control Commands Syntax:" );
    MessageAddChild( "play [input_number]" );
    MessageAddChild( "pause" );
    MessageAddChild( "stop" );
    MessageAddChild( "seek [+-](percentage) | [+-](seconds)s | [+-](milliseconds)ms" );
    return VLC_SUCCESS;
}
