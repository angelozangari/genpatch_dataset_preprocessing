}
static void zephyr_login(PurpleAccount * account)
{
	PurpleConnection *gc;
	zephyr_account *zephyr;
	gboolean read_anyone;
	gboolean read_zsubs;
	gchar *exposure;
	gc = purple_account_get_connection(account);
	read_anyone = purple_account_get_bool(gc->account,"read_anyone",TRUE);
	read_zsubs = purple_account_get_bool(gc->account,"read_zsubs",TRUE);
	exposure = (gchar *)purple_account_get_string(gc->account, "exposure_level", EXPOSE_REALMVIS);
#ifdef WIN32
	username = purple_account_get_username(account);
#endif
	gc->flags |= PURPLE_CONNECTION_AUTO_RESP | PURPLE_CONNECTION_HTML | PURPLE_CONNECTION_NO_BGCOLOR | PURPLE_CONNECTION_NO_URLDESC;
	gc->proto_data = zephyr=g_new0(zephyr_account,1);
	zephyr->account = account;
	/* Make sure that the exposure (visibility) is set to a sane value */
	zephyr->exposure=g_strdup(normalize_zephyr_exposure(exposure));
	if (purple_account_get_bool(gc->account,"use_tzc",0)) {
		zephyr->connection_type = PURPLE_ZEPHYR_TZC;
	} else {
		zephyr->connection_type = PURPLE_ZEPHYR_KRB4;
	}
	zephyr->encoding = (char *)purple_account_get_string(gc->account, "encoding", ZEPHYR_FALLBACK_CHARSET);
	purple_connection_update_progress(gc, _("Connecting"), 0, 8);
	/* XXX z_call_s should actually try to report the com_err determined error */
	if (use_tzc(zephyr)) {
		pid_t pid;
		/*		  purple_connection_error(gc,"tzc not supported yet"); */
		if ((pipe(zephyr->totzc) != 0) || (pipe(zephyr->fromtzc) != 0)) {
			purple_debug_error("zephyr", "pipe creation failed. killing\n");
			exit(-1);
		}
		pid = fork();
		if (pid == -1) {
			purple_debug_error("zephyr", "forking failed\n");
			exit(-1);
		}
		if (pid == 0) {
			unsigned int i=0;
			gboolean found_ps = FALSE;
			gchar ** tzc_cmd_array = g_strsplit(purple_account_get_string(gc->account,"tzc_command","/usr/bin/tzc -e %s")," ",0);
			if (close(1) == -1) {
				exit(-1);
			}
			if (dup2(zephyr->fromtzc[1], 1) == -1) {
				exit(-1);
			}
			if (close(zephyr->fromtzc[1]) == -1) {
				exit(-1);
			}
			if (close(0) == -1) {
				exit(-1);
			}
			if (dup2(zephyr->totzc[0], 0) == -1) {
				exit(-1);
			}
			if (close(zephyr->totzc[0]) == -1) {
				exit(-1);
			}
			/* tzc_command should really be of the form
			   path/to/tzc -e %s
			   or
			   ssh username@hostname pathtotzc -e %s
			   -- this should not require a password, and ideally should be kerberized ssh --
			   or
			   fsh username@hostname pathtotzc -e %s
			*/
			while(tzc_cmd_array[i] != NULL){
				if (!g_ascii_strncasecmp(tzc_cmd_array[i],"%s",2)) {
					/*					fprintf(stderr,"replacing %%s with %s\n",zephyr->exposure); */
					tzc_cmd_array[i] = g_strdup(zephyr->exposure);
					found_ps = TRUE;
				} else {
					/*					fprintf(stderr,"keeping %s\n",tzc_cmd_array[i]); */
				}
				i++;
			}
			if (!found_ps) {
				exit(-1);
			}
			execvp(tzc_cmd_array[0], tzc_cmd_array);
			exit(-1);
		}
		else {
			fd_set rfds;
			int bufsize = 2048;
			char *buf = (char *)calloc(bufsize, 1);
			char *bufcur = buf;
			struct timeval tv;
			char *ptr;
			int parenlevel=0;
			char* tempstr;
			int tempstridx;
			int select_status;
			zephyr->tzc_pid = pid;
			/* wait till we have data to read from ssh */
			FD_ZERO(&rfds);
			FD_SET(zephyr->fromtzc[ZEPHYR_FD_READ], &rfds);
			tv.tv_sec = 10;
			tv.tv_usec = 0;
			purple_debug_info("zephyr", "about to read from tzc\n");
			if (waitpid(pid, NULL, WNOHANG) == 0) { /* Only select if tzc is still running */
				purple_debug_info("zephyr", "about to read from tzc\n");
				select_status = select(zephyr->fromtzc[ZEPHYR_FD_READ] + 1, &rfds, NULL, NULL, NULL);
			}
			else {
				purple_debug_info("zephyr", "tzc exited early\n");
				select_status = -1;
			}
			FD_ZERO(&rfds);
			FD_SET(zephyr->fromtzc[ZEPHYR_FD_READ], &rfds);
			while (select_status > 0 &&
			       select(zephyr->fromtzc[ZEPHYR_FD_READ] + 1, &rfds, NULL, NULL, &tv) > 0) {
				if (read(zephyr->fromtzc[ZEPHYR_FD_READ], bufcur, 1) != 1) {
					purple_debug_error("zephyr", "couldn't read\n");
					purple_connection_error(gc, "couldn't read");
					free(buf);
					return;
				}
				bufcur++;
				if ((bufcur - buf) > (bufsize - 1)) {
					if ((buf = realloc(buf, bufsize * 2)) == NULL) {
						exit(-1);
					} else {
						bufcur = buf + bufsize;
						bufsize *= 2;
					}
				}
				FD_ZERO(&rfds);
				FD_SET(zephyr->fromtzc[ZEPHYR_FD_READ], &rfds);
				tv.tv_sec = 10;
				tv.tv_usec = 0;
			}
			/*			  fprintf(stderr, "read from tzc\n"); */
			*bufcur = '\0';
			ptr = buf;
			/* ignore all tzcoutput till we've received the first (*/
			while (ptr < bufcur && (*ptr !='(')) {
				ptr++;
			}
			if (ptr >=bufcur) {
				purple_connection_error(gc,"invalid output by tzc (or bad parsing code)");
				free(buf);
				return;
			}
			while(ptr < bufcur) {
				if (*ptr == '(') {
					parenlevel++;
				}
				else if (*ptr == ')') {
					parenlevel--;
				}
				purple_debug_info("zephyr","tzc parenlevel is %d\n",parenlevel);
				switch (parenlevel) {
				case 0:
					break;
				case 1:
					/* Search for next beginning (, or for the ending */
					ptr++;
					while((*ptr != '(') && (*ptr != ')') && (ptr <bufcur))
						ptr++;
					if (ptr >= bufcur)
						purple_debug_error("zephyr","tzc parsing error\n");
					break;
				case 2:
					/* You are probably at
					   (foo . bar ) or (foo . "bar") or (foo . chars) or (foo . numbers) or (foo . () )
					   Parse all the data between the first and last f, and move past )
					*/
					tempstr = g_malloc0(20000);
					tempstridx=0;
					while(parenlevel >1) {
						ptr++;
						if (*ptr == '(')
							parenlevel++;
						if (*ptr == ')')
							parenlevel--;
						if (parenlevel > 1) {
							tempstr[tempstridx++]=*ptr;
						} else {
							ptr++;
						}
					}
					purple_debug_info("zephyr","tempstr parsed\n");
					/* tempstr should now be a tempstridx length string containing all characters
					   from that after the first ( to the one before the last paren ). */
					/* We should have the following possible lisp strings but we don't care
					   (tzcspew . start) (version . "something") (pid . number)*/
					/* We care about 'zephyrid . "username@REALM.NAME"' and 'exposure . "SOMETHING"' */
					tempstridx=0;
					if (!g_ascii_strncasecmp(tempstr,"zephyrid",8)) {
						gchar* username = g_malloc0(100);
						int username_idx=0;
						char *realm;
						purple_debug_info("zephyr","zephyrid found\n");
						tempstridx+=8;
						while(tempstr[tempstridx] !='"' && tempstridx < 20000)
							tempstridx++;
						tempstridx++;
						while(tempstr[tempstridx] !='"' && tempstridx < 20000)
							username[username_idx++]=tempstr[tempstridx++];
						zephyr->username = g_strdup_printf("%s",username);
						if ((realm = strchr(username,'@')))
							zephyr->realm = g_strdup_printf("%s",realm+1);
						else {
							realm = (gchar *)purple_account_get_string(gc->account,"realm","");
							if (!*realm) {
								realm = "local-realm";
							}
							zephyr->realm = g_strdup(realm);
							g_strlcpy(__Zephyr_realm, (const char*)zephyr->realm, REALM_SZ-1);
						}
						/* else {
						   zephyr->realm = g_strdup("local-realm");
						   }*/
						g_free(username);
					}  else {
						purple_debug_info("zephyr", "something that's not zephyr id found %s\n",tempstr);
					}
					/* We don't care about anything else yet */
					g_free(tempstr);
					break;
				default:
					purple_debug_info("zephyr","parenlevel is not 1 or 2\n");
					/* This shouldn't be happening */
					break;
				}
				if (parenlevel==0)
					break;
			} /* while (ptr < bufcur) */
			purple_debug_info("zephyr", "tzc startup done\n");
		free(buf);
		}
	}
	else if ( use_zeph02(zephyr)) {
		gchar* realm;
		z_call_s(ZInitialize(), "Couldn't initialize zephyr");
		z_call_s(ZOpenPort(&(zephyr->port)), "Couldn't open port");
		z_call_s(ZSetLocation((char *)zephyr->exposure), "Couldn't set location");
		realm = (gchar *)purple_account_get_string(gc->account,"realm","");
		if (!*realm) {
			realm = ZGetRealm();
		}
		zephyr->realm = g_strdup(realm);
		g_strlcpy(__Zephyr_realm, (const char*)zephyr->realm, REALM_SZ-1);
		zephyr->username = g_strdup(ZGetSender());
		/*		zephyr->realm = g_strdup(ZGetRealm()); */
		purple_debug_info("zephyr","realm: %s\n",zephyr->realm);
	}
	else {
		purple_connection_error(gc,"Only ZEPH0.2 supported currently");
		return;
	}
	purple_debug_info("zephyr","does it get here\n");
	purple_debug_info("zephyr"," realm: %s username:%s\n", zephyr->realm, zephyr->username);
	/* For now */
	zephyr->galaxy = NULL;
	zephyr->krbtkfile = NULL;
	zephyr_inithosts(zephyr);
	if (zephyr_subscribe_to(zephyr,"MESSAGE","PERSONAL",zephyr->username,NULL) != ZERR_NONE) {
		/* XXX don't translate this yet. It could be written better */
		/* XXX error messages could be handled with more detail */
		purple_notify_error(account->gc, NULL,
				  "Unable to subscribe to messages", "Unable to subscribe to initial messages");
		return;
	}
	purple_connection_set_state(gc, PURPLE_CONNECTED);
	if (read_anyone)
		process_anyone(gc);
	if (read_zsubs)
		process_zsubs(zephyr);
	if (use_zeph02(zephyr)) {
		zephyr->nottimer = purple_timeout_add(100, check_notify_zeph02, gc);
	} else if (use_tzc(zephyr)) {
		zephyr->nottimer = purple_timeout_add(100, check_notify_tzc, gc);
	}
	zephyr->loctimer = purple_timeout_add_seconds(20, check_loc, gc);
}
