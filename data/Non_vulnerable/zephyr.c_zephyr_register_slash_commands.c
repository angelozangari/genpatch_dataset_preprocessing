}
static void zephyr_register_slash_commands(void)
{
	purple_cmd_register("msg","ws", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_msg, _("msg &lt;nick&gt; &lt;message&gt;:  Send a private message to a user"), NULL);
	purple_cmd_register("zlocate","w", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zlocate, _("zlocate &lt;nick&gt;: Locate user"), NULL);
	purple_cmd_register("zl","w", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zlocate, _("zl &lt;nick&gt;: Locate user"), NULL);
	purple_cmd_register("instance","s", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_instance, _("instance &lt;instance&gt;: Set the instance to be used on this class"), NULL);
	purple_cmd_register("inst","s", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_instance, _("inst &lt;instance&gt;: Set the instance to be used on this class"), NULL);
	purple_cmd_register("topic","s", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_instance, _("topic &lt;instance&gt;: Set the instance to be used on this class"), NULL);
	purple_cmd_register("sub", "www", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_joinchat_cir,
			  _("sub &lt;class&gt; &lt;instance&gt; &lt;recipient&gt;: Join a new chat"), NULL);
	purple_cmd_register("zi","ws", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zi, _("zi &lt;instance&gt;: Send a message to &lt;message,<i>instance</i>,*&gt;"), NULL);
	purple_cmd_register("zci","wws",PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zci,
			  _("zci &lt;class&gt; &lt;instance&gt;: Send a message to &lt;<i>class</i>,<i>instance</i>,*&gt;"), NULL);
	purple_cmd_register("zcir","wwws",PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zcir,
			  _("zcir &lt;class&gt; &lt;instance&gt; &lt;recipient&gt;: Send a message to &lt;<i>class</i>,<i>instance</i>,<i>recipient</i>&gt;"), NULL);
	purple_cmd_register("zir","wws",PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zir,
			  _("zir &lt;instance&gt; &lt;recipient&gt;: Send a message to &lt;MESSAGE,<i>instance</i>,<i>recipient</i>&gt;"), NULL);
	purple_cmd_register("zc","ws", PURPLE_CMD_P_PRPL,
			  PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY,
			  "prpl-zephyr",
			  zephyr_purple_cmd_zc, _("zc &lt;class&gt;: Send a message to &lt;<i>class</i>,PERSONAL,*&gt;"), NULL);
}
