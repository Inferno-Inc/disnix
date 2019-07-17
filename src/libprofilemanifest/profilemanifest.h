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

#ifndef __DISNIX_PROFILEMANIFEST_H
#define __DISNIX_PROFILEMANIFEST_H
#include <stdio.h>
#include <glib.h>

typedef struct
{
    GHashTable *services_table;

    GPtrArray *service_mapping_array;

    GPtrArray *snapshot_mapping_array;
}
ProfileManifest;

/**
 * Composes an array of profile manifest entries from the manifest configuration
 * file stored in the given profile.
 *
 * @param result An NULL-terminated array of text lines
 * @return An array of pointers to profile manifest entries or NULL if an error has occured
 */
ProfileManifest *create_profile_manifest_from_string(char *result);

/**
 * Composes an array of profile manifest entries from the provide manifest
 * configuration file.
 *
 * @param manifest_file Path to a profile manifest file
 * @return An array of pointers to profile manifest entries or NULL if an error has occured
 */
ProfileManifest *create_profile_manifest_from_file(const gchar *profile_manifest_file);

/**
 * Composes an array of profile manifest entries from the manifest configuration
 * file stored in the given profile.
 *
 * @param localstatedir Directory if which Nix's state files are stored
 * @param profile Name of the profile to take the manifest from
 * @return An array of pointers to profile manifest entries or NULL if an error has occured
 */
ProfileManifest *create_profile_manifest_from_current_deployment(gchar *localstatedir, gchar *profile);

/**
 * Deletes a profile manifest array and its contents from heap memory.
 *
 * @param profile_manifest_array An array of profile manifest entries
 */
void delete_profile_manifest(ProfileManifest *profile_manifest);

/**
 * Prints the deployed services for the given profile manifest.
 *
 * @param profile_manifest_array An array of profile manifest entries
 */
void print_services_in_profile_manifest(const ProfileManifest *profile_manifest);

/**
 * Prints the deployed services grouped by container for the given profile
 * manifest.
 *
 * @param profile_manifest_array An array of profile manifest entries
 */
void print_services_per_container_in_profile_manifest(ProfileManifest *profile_manifest);

/**
 * Prints a raw textual representation of the profile manifest file.
 *
 * @param profile_manifest_array An array of profile manifest entries
 * @param fd File descriptor to print to
 */
void print_text_from_profile_manifest(gchar *localstatedir, gchar *profile, int fd);

/**
 * Prints a Nix expression containing all properties of all deployed services
 * for the given profile manifest to a given file descriptor
 *
 * @param profile_manifest_array An array of profile manifest entries
 */
void print_profile_manifest_nix(FILE *file, const void *value, const int indent_level, void *userdata);

void print_profile_manifest_xml(FILE *file, const void *value, const int indent_level, const char *type_property_name, void *userdata);

#endif
