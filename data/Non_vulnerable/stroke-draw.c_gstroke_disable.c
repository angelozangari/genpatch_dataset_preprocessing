void
gstroke_disable(GtkWidget *widget)
{
  g_signal_handlers_disconnect_by_func(G_OBJECT(widget), G_CALLBACK(process_event), NULL);
}
