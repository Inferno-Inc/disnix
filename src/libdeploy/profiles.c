/*
 * Disnix - A Nix-based distributed service deployment tool
 * Copyright (C) 2008-2018  Sander van der Burg
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "profiles.h"
#include <profilemapping-iterator.h>
#include <targetstable.h>
#include <client-interface.h>
#include <package-management.h>

static pid_t set_distribution_item(void *data, xmlChar *profile_name, gchar *target_name, Target *target)
{
    char *profile = (char*)data;
    g_print("[target: %s]: Setting Disnix profile: %s\n", target_name, profile_name);
    return exec_set((char*)target->client_interface, (char*)target_name, profile, (char*)profile_name);
}

static void complete_set_distribution_item(void *data, xmlChar *profile_name, gchar *target_name, ProcReact_Status status, int result)
{
    if(status != PROCREACT_STATUS_OK || !result)
        g_printerr("[target: %s]: Cannot set Disnix profile: %s\n", target_name, profile_name);
}

static int set_target_profiles(GHashTable *profile_mapping_table, GHashTable *targets_table, gchar *profile)
{
    /* Iterate over the distribution mappings, limiting concurrency to the desired concurrent transfers and distribute them */
    int success;
    ProcReact_PidIterator iterator = create_distribution_iterator(profile_mapping_table, targets_table, set_distribution_item, complete_set_distribution_item, profile);
    procreact_fork_in_parallel_and_wait(&iterator);
    success = distribution_iterator_has_succeeded(&iterator);

    destroy_distribution_iterator(&iterator);

    return success;
}

int set_profiles(const Manifest *manifest, const gchar *manifest_file, const gchar *coordinator_profile_path, char *profile, const unsigned int flags)
{
    return((flags & SET_NO_TARGET_PROFILES || set_target_profiles(manifest->profile_mapping_table, manifest->targets_table, profile)) /* First, attempt to set the target profiles */
      && (flags & SET_NO_COORDINATOR_PROFILE || pkgmgmt_set_coordinator_profile(coordinator_profile_path, manifest_file, profile))); /* Then try to set the coordinator profile */
}
