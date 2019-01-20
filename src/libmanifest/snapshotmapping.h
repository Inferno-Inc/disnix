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

#ifndef __DISNIX_SNAPSHOTMAPPING_H
#define __DISNIX_SNAPSHOTMAPPING_H
#include <glib.h>
#include "targets.h"

/**
 * @brief Contains the values that constitute a key uniquely referring to a snapshot mapping.
 */
typedef struct
{
    /** Name of the mutable component */
    gchar *component;
    
    /** Container in which the mutable component is deployed */
    gchar *container;
    
    /** Target property referring to the target machine to which the service is deployed */
    gchar *target;
}
SnapshotMappingKey;

/**
 * @brief Contains all properties to snapshot state on a specific machine.
 * This struct maps (component,container,target) -> (transferred)
 */
typedef struct
{
    /** Name of the mutable component */
    gchar *component;
    
    /** Container in which the mutable component is deployed */
    gchar *container;
    
    /** Target property referring to the target machine to which the service is deployed */
    gchar *target;
    
    /** Full Nix store path to the corresponding service */
    gchar *service;
    
    /** Activation type */
    gchar *type;
    
    /** Indicates whether the snapshot has been transferred or not */
    gboolean transferred;
}
SnapshotMapping;

/**
 * Function that spawns a process for a snapshot mapping.
 *
 * @param mapping A snapshot mapping from a snapshots array
 * @param target Target machine to which the snapshot is mapped
 * @param arguments Arguments passed to the client interface
 * @param arguments_length Length of the arguments array
 * @return PID of the spawned process
 */
typedef pid_t (*map_snapshot_item_function) (SnapshotMapping *mapping, Target *target, gchar **arguments, unsigned int arguments_length);

/**
 * Function that gets executed when a mapping function completes.
 *
 * @param mapping A snapshot mapping from a snapshots array
 * @param status Indicates whether the process terminated abnormally or not
 * @param result TRUE if the mapping operation succeeded, else FALSE
 */
typedef void (*complete_snapshot_item_mapping_function) (SnapshotMapping *mapping, ProcReact_Status status, int result);

/**
 * Creates an array with activation mappings from a manifest XML file.
 *
 * @param manifest_file Path to the manifest XML file
 * @param container_filter Name of the container to filter on, or NULL to parse all containers
 * @param component_filter Name of the component to filter on, or NULL to parse all components
 * @return GPtrArray containing activation mappings
 */
GPtrArray *create_snapshots_array(const gchar *manifest_file, const gchar *container_filter, const gchar *component_filter);

/**
 * Deletes an array with snapshot mappings including its contents.
 *
 * @param snapshots_array Snapshots array to delete
 */
void delete_snapshots_array(GPtrArray *snapshots_array);

/**
 * Returns the snapshots mapping with the given key in the snapshots array.
 *
 * @param snapshots_array Snapshots array
 * @param key Key of the snapshots mapping to find
 * @return The snapshot mapping with the specified key, or NULL if it cannot be found
 */
SnapshotMapping *find_snapshot_mapping(const GPtrArray *snapshots_array, const SnapshotMappingKey *key);

/**
 * Subtract the snapshots from array1 that are in array2.
 *
 * @param snapshots_array1 Array to substract from
 * @param snapshots_array2 Array to substract
 * @return An array with snapshot mappings with elements from array1 that are not in array2
 */
GPtrArray *subtract_snapshot_mappings(const GPtrArray *snapshots_array1, const GPtrArray *snapshots_array2);

/**
 * Finds all snapshot mappings that map to a specific target.
 *
 * @param snapshots_array Snapshots array
 * @param target Key that identifies a target machine
 * @return An array with snapshot mappings linked to the given target
 */
GPtrArray *find_snapshot_mappings_per_target(const GPtrArray *snapshots_array, const gchar *target);

/**
 * Maps over each snapshot mapping, asynchronously executes a function for each
 * item and ensures that for each machine only the allowed number of processes
 * are executed concurrently.
 *
 * @param snapshots_array Snapshots array
 * @param target_array Targets array
 * @param map_snapshot_item Function that gets executed for each snapshot item
 * @param complete_snapshot_item_mapping Function that gets executed when a mapping function completes
 * @return TRUE if all mappings were successfully executed, else FALSE
 */
int map_snapshot_items(const GPtrArray *snapshots_array, const GPtrArray *target_array, map_snapshot_item_function map_snapshot_item, complete_snapshot_item_mapping_function complete_snapshot_item_mapping);

void clear_snapshot_items_transferred_status(GPtrArray *snapshots_array);

/**
 * Opens the provided manifest or (if NULL) it attempts to open the manifest of
 * the last deployed configuration and returns the snapshot entries in it.
 *
 * @param manifest_file Manifest file to open
 * @param coordinator_profile_path Path to the coordinator profile or NULL to consult the default profile path
 * @param profile Name of the Disnix profile that identifies the deployment (typically: default)
 * @param container_filter Name of the container to filter on, or NULL to parse all containers
 * @param component_filter Name of the component to filter on, or NULL to parse all components
 * @return An array with the snapshots in the provided manifest, or previous manifest, or NULL in case of an error
 */
GPtrArray *open_provided_or_previous_snapshots_array(const gchar *manifest_file, const gchar *coordinator_profile_path, gchar *profile, const gchar *container_filter, const gchar *component_filter);

#endif
