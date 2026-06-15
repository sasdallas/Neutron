/**
 * @file neutron/icon.c
 * @brief Icon manager
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#include <neutron/neutron.h>
#include <sys/stat.h>
#include <string.h>

#ifdef BUILDING_LINUX
#define DEFAULT_ICON_PATH "resources/"
#else
#define DEFAULT_ICON_PATH "/usr/share/resources/"
#endif

const char *valid_icon_endings[] = { ".png", ".jpg", ".bmp" };

// TODO: Convert this to an actual cache
nt_icon_cache_entry_t *icon_cache = NULL;

int nt_icon_init() {
    // TODO build a cache
    return 0;
}

static nt_icon_cache_entry_t *nt_icon_get_cached(char *icon_name, size_t size) {
    nt_icon_cache_entry_t *ent = icon_cache;
    while (ent) {
        if (!strcmp(icon_name, ent->name) && ent->size == size) return ent;
        ent = ent->next;
    }

    return NULL;
}

static nt_icon_cache_entry_t *nt_icon_add_cache(char *icon_name, size_t size) {
    nt_icon_cache_entry_t *c = malloc(sizeof(nt_icon_cache_entry_t));
    c->name = strdup(icon_name);
    c->next = icon_cache;
    c->size = size;

    // Create a path
    char path[4096];
    bool ok = false;
    for (int i = 0; i < sizeof(valid_icon_endings) / sizeof(char*); i++) {
        snprintf(path, 4096, DEFAULT_ICON_PATH "%d/%s%s", size, icon_name, valid_icon_endings[i]);
        struct stat st;
        if (stat(path, &st) == 0) { ok = true; break; }
    }

    if (!ok) {
        free(c->name);
        free(c);
        return NULL;
    }

    int r = nt_render_create_image(&c->img, path);
    if (r != 0) {
        free(c->name);
        free(c);
        return NULL;
    }

    icon_cache = c;
    return c;
}

nt_image_t *nt_icon_get(char *icon_name, char **fallbacks, size_t size) {
    nt_icon_cache_entry_t *ent = nt_icon_get_cached(icon_name, size);
    if (!ent) ent = nt_icon_add_cache(icon_name, size);

    if (ent != NULL) {
        return &ent->img;
    }

    NT_WARN("Failed to get icon '%s' at size %d\n", icon_name, size);

    if (fallbacks == NULL) {
        return NULL;
    }

    int i = 0;
    while (fallbacks[i] != NULL) {
        ent = nt_icon_get_cached(fallbacks[i], size);
        if (!ent) ent = nt_icon_add_cache(fallbacks[i], size);
        if (ent) return &ent->img;
        NT_WARN("Failed to get fallback icon '%s' at size %d\n", fallbacks[i], size);
        i++;
    }

    return NULL;
}
