/*
 *  API - DVR
 *
 *  Copyright (C) 2014 Jaroslav Kysela
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tvheadend.h"
#include "dvr/dvr.h"
#include "lang_codes.h"
#include "epg.h"
#include "api.h"

/*
 *
 */

static void
api_dvr_config_grid
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_config_t *cfg;

  LIST_FOREACH(cfg, &dvrconfigs, config_link)
    if (!idnode_perm((idnode_t *)cfg, perm, NULL)) {
      idnode_set_add(ins, (idnode_t*)cfg, &conf->filter, perm->aa_lang_ui);
      idnode_perm_unset((idnode_t *)cfg);
    }
}

static int
api_dvr_config_create
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  dvr_config_t *cfg;
  htsmsg_t *conf;
  const char *s;

  if (!(conf = htsmsg_get_map(args, "conf")))
    return EINVAL;
  if (!(s = htsmsg_get_str(conf, "name")))
    return EINVAL;
  if (s[0] == '\0')
    return EINVAL;

  tvh_mutex_lock(&global_lock);
  if ((cfg = dvr_config_create(NULL, NULL, conf))) {
    api_idnode_create(resp, &cfg->dvr_id);
    dvr_config_changed(cfg);
  }
  tvh_mutex_unlock(&global_lock);

  return 0;
}

static void
api_dvr_entry_grid
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_entry_t *de;

  LIST_FOREACH(de, &dvrentries, de_global_link)
    idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
}

static void
api_dvr_entry_grid_upcoming
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_entry_t *de;
  int duplicates = htsmsg_get_s32_or_default(args, "duplicates", 1);

  if (duplicates) {
    LIST_FOREACH(de, &dvrentries, de_global_link)
      if (dvr_entry_is_upcoming(de))
        idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
  } else {
    LIST_FOREACH(de, &dvrentries, de_global_link)
      if (dvr_entry_is_upcoming_nodup(de))
        idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
  }
}

static void
api_dvr_entry_grid_finished
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_entry_t *de;

  LIST_FOREACH(de, &dvrentries, de_global_link)
    if (dvr_entry_is_finished(de, DVR_FINISHED_SUCCESS))
      idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
}

static void
api_dvr_entry_grid_failed
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_entry_t *de;

  LIST_FOREACH(de, &dvrentries, de_global_link)
    if (dvr_entry_is_finished(de, DVR_FINISHED_FAILED))
      idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
}

static void
api_dvr_entry_grid_removed
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_entry_t *de;

  LIST_FOREACH(de, &dvrentries, de_global_link)
    if (dvr_entry_is_finished(de, DVR_FINISHED_REMOVED_SUCCESS | DVR_FINISHED_REMOVED_FAILED))
      idnode_set_add(ins, (idnode_t*)de, &conf->filter, perm->aa_lang_ui);
}

static int
api_dvr_entry_create
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  dvr_entry_t *de;
  dvr_config_t *cfg;
  htsmsg_t *conf, *m;
  char *s, *lang;
  const char *s1;
  int res = EPERM;

  if (!(conf = htsmsg_get_map(args, "conf")))
    return EINVAL;

  tvh_mutex_lock(&global_lock);
  s1 = htsmsg_get_str(conf, "config_name");
  cfg = dvr_config_find_by_list(perm->aa_dvrcfgs, s1);
  if (cfg) {
    htsmsg_set_uuid(conf, "config_name", &cfg->dvr_id.in_uuid);
    htsmsg_set_str2(conf, "owner", perm->aa_username);
    htsmsg_set_str2(conf, "creator", perm->aa_representative);

    lang = access_get_lang(perm, htsmsg_get_str(conf, "lang"));
    if (lang) {
      for (s = (char *)lang; *s && *s != ','; s++);
      *s = '\0';
    } else {
      lang = strdup(lang_code_preferred());
    }

    s1 = htsmsg_get_str(conf, "disp_title");
    if (s1 && !htsmsg_get_map(conf, "title")) {
      m = htsmsg_create_map();
      htsmsg_add_str(m, lang, s1);
      htsmsg_add_msg(conf, "title", m);
    }

    s1 = htsmsg_get_str(conf, "disp_subtitle");
    if (s1 == NULL || s1[0] == '\0')
      s1 = htsmsg_get_str(conf, "disp_extratext");
    if (s1 && !htsmsg_get_map(conf, "subtitle")) {
      m = htsmsg_create_map();
      htsmsg_add_str(m, lang, s1);
      htsmsg_add_msg(conf, "subtitle", m);
    }
    if ((de = dvr_entry_create(NULL, conf, 0)))
      api_idnode_create(resp, &de->de_id);

    res = 0;
    free(lang);
  }
  tvh_mutex_unlock(&global_lock);

  return res;
}

static htsmsg_t *
api_dvr_entry_create_from_single(htsmsg_t *args)
{
  htsmsg_t *entries, *m;
  const char *s1, *s2, *s3;

  if (!(s1 = htsmsg_get_str(args, "config_uuid")))
    return NULL;
  if (!(s2 = htsmsg_get_str(args, "event_id")))
    return NULL;
  s3 = htsmsg_get_str(args, "comment");
  entries = htsmsg_create_list();
  m = htsmsg_create_map();
  htsmsg_add_str(m, "config_uuid", s1);
  htsmsg_add_str(m, "event_id", s2);
  if (s3)
    htsmsg_add_str(m, "comment", s3);
  htsmsg_add_msg(entries, NULL, m);
  return entries;
}

static int
api_dvr_entry_create_by_event
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  dvr_entry_t *de;
  epg_broadcast_t *e;
  htsmsg_t *entries, *entries2 = NULL, *m, *l = NULL, *conf;
  htsmsg_field_t *f;
  const char *s, *config_uuid;
  int count = 0;

  if (!(entries = htsmsg_get_list(args, "entries"))) {
    entries = entries2 = api_dvr_entry_create_from_single(args);
    if (!entries)
      return EINVAL;
  }

  HTSMSG_FOREACH(f, entries) {
    if (!(m = htsmsg_get_map_by_field(f))) continue;

    if (!(s = htsmsg_get_str(m, "event_id")))
      continue;

    conf = htsmsg_create_map();
    htsmsg_copy_field(conf, "enabled", m, NULL);
    htsmsg_copy_field(conf, "comment", m, NULL);
    htsmsg_add_str2(conf, "owner", perm->aa_username);
    htsmsg_add_str2(conf, "creator", perm->aa_representative);
    config_uuid = htsmsg_get_str(m, "config_uuid");
    de = NULL;

    tvh_mutex_lock(&global_lock);
    if ((e = epg_broadcast_find_by_id(strtoll(s, NULL, 10)))) {
      dvr_config_t *cfg = dvr_config_find_by_list(perm->aa_dvrcfgs, config_uuid);
      if (cfg) {
        htsmsg_add_uuid(conf, "config_name", &cfg->dvr_id.in_uuid);
        de = dvr_entry_create_from_htsmsg(conf, e);
        if (de)
          idnode_changed(&de->de_id);
      }
    }
    tvh_mutex_unlock(&global_lock);

    htsmsg_destroy(conf);

    if (de) {
      if (l == NULL)
        l = htsmsg_create_list();
      htsmsg_add_uuid(l, NULL, &de->de_id.in_uuid);
    }

    count++;
  }

  htsmsg_destroy(entries2);

  api_idnode_create_list(resp, l);

  return !count ? EINVAL : 0;
}

static void
api_dvr_rerecord_toggle(access_t *perm, idnode_t *self)
{
  dvr_entry_set_rerecord((dvr_entry_t *)self, -1);
}

static int
api_dvr_entry_rerecord_toggle
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_rerecord_toggle, "rerecord", 0);
}

static void
api_dvr_rerecord_deny(access_t *perm, idnode_t *self)
{
  dvr_entry_set_rerecord((dvr_entry_t *)self, 0);
}

static int
api_dvr_entry_rerecord_deny
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_rerecord_deny, "rerecord", 0);
}

static void
api_dvr_rerecord_allow(access_t *perm, idnode_t *self)
{
  dvr_entry_set_rerecord((dvr_entry_t *)self, 1);
}

static int
api_dvr_entry_rerecord_allow
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_rerecord_allow, "rerecord", 0);
}

static void
api_dvr_stop(access_t *perm, idnode_t *self)
{
  dvr_entry_stop((dvr_entry_t *)self);
}

static int
api_dvr_entry_stop
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_stop, "stop", 0);
}

static void
api_dvr_cancel(access_t *perm, idnode_t *self)
{
  dvr_entry_cancel((dvr_entry_t *)self, 0);
}

static int
api_dvr_entry_cancel
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_cancel, "cancel", 0);
}

static void
api_dvr_remove(access_t *perm, idnode_t *self)
{
  dvr_entry_t *de = (dvr_entry_t *)self;
  if (de->de_sched_state != DVR_SCHEDULED && de->de_sched_state != DVR_NOSTATE)
    dvr_entry_cancel_remove(de, 0);
}

static int
api_dvr_entry_remove
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_remove, "remove", 0);
}

static void
api_dvr_prevrec_toggle(access_t *perm, idnode_t *self)
{
  dvr_entry_set_prevrec((dvr_entry_t *)self, -1);
}

static int
api_dvr_entry_prevrec_toggle
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_prevrec_toggle, "prevrec", 0);
}

static void
api_dvr_prevrec_unset(access_t *perm, idnode_t *self)
{
  dvr_entry_set_prevrec((dvr_entry_t *)self, 0);
}

static int
api_dvr_entry_prevrec_unset
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_prevrec_unset, "prevrec", 0);
}

static void
api_dvr_prevrec_set(access_t *perm, idnode_t *self)
{
  dvr_entry_set_prevrec((dvr_entry_t *)self, 1);
}

static int
api_dvr_entry_prevrec_set
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_prevrec_set, "prevrec", 0);
}

static void
api_dvr_move_finished(access_t *perm, idnode_t *self)
{
  dvr_entry_move((dvr_entry_t *)self, 0);
}

static int
api_dvr_entry_move_finished
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_move_finished, "move finished", 0);
}

static void
api_dvr_move_failed(access_t *perm, idnode_t *self)
{
  dvr_entry_move((dvr_entry_t *)self, 1);
}

static int
api_dvr_entry_move_failed
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  return api_idnode_handler(&dvr_entry_class, perm, args, resp, api_dvr_move_failed, "move failed", 0);
}

static void
api_dvr_autorec_grid
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_autorec_entry_t *dae;

  TAILQ_FOREACH(dae, &autorec_entries, dae_link)
    idnode_set_add(ins, (idnode_t*)dae, &conf->filter, perm->aa_lang_ui);
}

static int
api_dvr_autorec_create
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  htsmsg_t *conf;
  dvr_config_t *cfg;
  dvr_autorec_entry_t *dae;
  const char *s1;

  if (!(conf  = htsmsg_get_map(args, "conf")))
    return EINVAL;

  htsmsg_set_str2(conf, "owner", perm->aa_username);
  htsmsg_set_str2(conf, "creator", perm->aa_representative);

  s1 = htsmsg_get_str(conf, "config_uuid");
  if (s1 == NULL)
    s1 = htsmsg_get_str(conf, "config_name");

  tvh_mutex_lock(&global_lock);
  cfg = dvr_config_find_by_list(perm->aa_dvrcfgs, s1);
  if (cfg) {
    htsmsg_set_uuid(conf, "config_name", &cfg->dvr_id.in_uuid);
    dae = dvr_autorec_create(NULL, conf);
    if (dae) {
      api_idnode_create(resp, &dae->dae_id);
      dvr_autorec_changed(dae, 0);
      dvr_autorec_completed(dae, 0);
    }
  }
  tvh_mutex_unlock(&global_lock);

  return 0;
}

static int
api_dvr_autorec_create_by_series
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  dvr_autorec_entry_t *dae;
  epg_broadcast_t *e;
  htsmsg_t *entries, *entries2 = NULL, *m, *l = NULL;
  htsmsg_field_t *f;
  const char *config_uuid, *s, *title;
  int count = 0;
  char ubuf[UUID_HEX_SIZE];
  const char * msg = " - Created from EPG query";
  char *comment;

  if (!(entries = htsmsg_get_list(args, "entries"))) {
    entries = entries2 = api_dvr_entry_create_from_single(args);
    if (!entries)
      return EINVAL;
  }

  HTSMSG_FOREACH(f, entries) {
    if (!(m = htsmsg_get_map_by_field(f))) continue;

    if (!(s = htsmsg_get_str(m, "event_id")))
      continue;

    config_uuid = htsmsg_get_str(m, "config_uuid");

    tvh_mutex_lock(&global_lock);
    if ((e = epg_broadcast_find_by_id(strtoll(s, NULL, 10)))) {
      dvr_config_t *cfg = dvr_config_find_by_list(perm->aa_dvrcfgs, config_uuid);
      if (cfg) {
        title = epg_broadcast_get_title(e, NULL);
        comment = alloca(strlen(title) + strlen(msg) + 1);
        sprintf(comment, "%s%s", title, msg);
        dae = dvr_autorec_add_series_link(idnode_uuid_as_str(&cfg->dvr_id, ubuf),
                                          e,
                                          perm->aa_username,
                                          perm->aa_representative,
                                          comment);
        if (dae) {
          if (l == NULL)
            l = htsmsg_create_list();
          htsmsg_add_uuid(l, NULL, &dae->dae_id.in_uuid);
          idnode_changed(&dae->dae_id);
        }
      }
    }
    tvh_mutex_unlock(&global_lock);
    count++;
  }

  htsmsg_destroy(entries2);

  api_idnode_create_list(resp, l);

  return !count ? EINVAL : 0;
}

static void
api_dvr_timerec_grid
  ( access_t *perm, idnode_set_t *ins, api_idnode_grid_conf_t *conf, htsmsg_t *args )
{
  dvr_timerec_entry_t *dte;

  TAILQ_FOREACH(dte, &timerec_entries, dte_link)
    idnode_set_add(ins, (idnode_t*)dte, &conf->filter, perm->aa_lang_ui);
}

static int
api_dvr_timerec_create
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  htsmsg_t *conf;
  dvr_timerec_entry_t *dte;

  if (!(conf  = htsmsg_get_map(args, "conf")))
    return EINVAL;

  htsmsg_set_str2(conf, "owner", perm->aa_username);
  htsmsg_set_str2(conf, "creator", perm->aa_representative);

  tvh_mutex_lock(&global_lock);
  dte = dvr_timerec_create(NULL, conf);
  if (dte) {
    api_idnode_create(resp, &dte->dte_id);
    dvr_timerec_check(dte);
  }
  tvh_mutex_unlock(&global_lock);

  return 0;
}

static int
api_dvr_entry_file_moved
  ( access_t *perm, void *opaque, const char *op, htsmsg_t *args, htsmsg_t **resp )
{
  const char *src, *dst;
  if (!(src = htsmsg_get_str(args, "src")))
    return -EINVAL;
  if (!(dst = htsmsg_get_str(args, "dst")))
    return -EINVAL;
  if (dvr_entry_file_moved(src, dst))
    return -ENOENT;
  return 0;
}


void api_dvr_init ( void )
{
  static api_hook_t ah[] = {
    { "dvr/config/class",          ACCESS_OR|ACCESS_ADMIN|ACCESS_RECORDER,
                                     api_idnode_class, (void*)&dvr_config_class },
    { "dvr/config/grid",           ACCESS_OR|ACCESS_ADMIN|ACCESS_RECORDER,
                                     api_idnode_grid, api_dvr_config_grid },
    { "dvr/config/create",         ACCESS_ADMIN, api_dvr_config_create, NULL },

    { "dvr/entry/class",           ACCESS_RECORDER, api_idnode_class, (void*)&dvr_entry_class },
    { "dvr/entry/grid",            ACCESS_RECORDER, api_idnode_grid, api_dvr_entry_grid },
    { "dvr/entry/grid_upcoming",   ACCESS_RECORDER, api_idnode_grid, api_dvr_entry_grid_upcoming },
    { "dvr/entry/grid_finished",   ACCESS_RECORDER, api_idnode_grid, api_dvr_entry_grid_finished },
    { "dvr/entry/grid_failed",     ACCESS_RECORDER, api_idnode_grid, api_dvr_entry_grid_failed },
    { "dvr/entry/grid_removed",    ACCESS_RECORDER, api_idnode_grid, api_dvr_entry_grid_removed },
    { "dvr/entry/create",          ACCESS_RECORDER, api_dvr_entry_create, NULL },
    { "dvr/entry/create_by_event", ACCESS_RECORDER, api_dvr_entry_create_by_event, NULL },
    { "dvr/entry/rerecord/toggle", ACCESS_RECORDER, api_dvr_entry_rerecord_toggle, NULL },
    { "dvr/entry/rerecord/deny",   ACCESS_RECORDER, api_dvr_entry_rerecord_deny, NULL },
    { "dvr/entry/rerecord/allow",  ACCESS_RECORDER, api_dvr_entry_rerecord_allow, NULL },
    { "dvr/entry/stop",            ACCESS_RECORDER, api_dvr_entry_stop, NULL },   /* Stop active recording gracefully */
    { "dvr/entry/cancel",          ACCESS_RECORDER, api_dvr_entry_cancel, NULL }, /* Cancel scheduled or active recording */
    { "dvr/entry/prevrec/toggle",  ACCESS_RECORDER, api_dvr_entry_prevrec_toggle, NULL },
    { "dvr/entry/prevrec/set",     ACCESS_RECORDER, api_dvr_entry_prevrec_set, NULL },
    { "dvr/entry/prevrec/unset",   ACCESS_RECORDER, api_dvr_entry_prevrec_unset, NULL },
    { "dvr/entry/remove",          ACCESS_RECORDER, api_dvr_entry_remove, NULL }, /* Remove recorded files from storage */
    { "dvr/entry/filemoved",       ACCESS_ADMIN,    api_dvr_entry_file_moved, NULL },
    { "dvr/entry/move/finished",   ACCESS_RECORDER, api_dvr_entry_move_finished, NULL },
    { "dvr/entry/move/failed",     ACCESS_RECORDER, api_dvr_entry_move_failed, NULL },

    { "dvr/autorec/class",         ACCESS_RECORDER, api_idnode_class, (void*)&dvr_autorec_entry_class },
    { "dvr/autorec/grid",          ACCESS_RECORDER, api_idnode_grid,  api_dvr_autorec_grid },
    { "dvr/autorec/create",        ACCESS_RECORDER, api_dvr_autorec_create, NULL },
    { "dvr/autorec/create_by_series", ACCESS_RECORDER, api_dvr_autorec_create_by_series, NULL },

    { "dvr/timerec/class",         ACCESS_RECORDER, api_idnode_class, (void*)&dvr_timerec_entry_class },
    { "dvr/timerec/grid",          ACCESS_RECORDER, api_idnode_grid,  api_dvr_timerec_grid },
    { "dvr/timerec/create",        ACCESS_RECORDER, api_dvr_timerec_create, NULL },

    { NULL },
  };

  api_register_all(ah);
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 491,8;491,15

