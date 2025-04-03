static void
pidgin_blist_theme_edit(PurplePluginAction *unused)
{
	GtkWidget *dialog;
	GtkWidget *box;
	GtkSizeGroup *group;
	PidginBlistTheme *theme;
	GObjectClass *klass;
	int i, j;
	static struct {
		const char *header;
		const char *props[12];
	} sections[] = {
		{N_("Contact"), {
					"contact-color",
					"contact",
					"online",
					"away",
					"offline",
					"idle",
					"message",
					"message_nick_said",
					"status",
					NULL
				}
		},
		{N_("Group"), {
				      "expanded-color",
				      "expanded-text",
				      "collapsed-color",
				      "collapsed-text",
				      NULL
			      }
		},
