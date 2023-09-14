
/** MSAP neighbor structure */
typedef struct __attribute__ ((__packed__))
{
    /** Neighbor ID */
    w_addr_t    addr;
    /** Link reliability for this node (for next hop) */
    uint8_t     link_rel;
    /** Normalized RSSI */
    uint8_t     rssi_norm;
    /** TC 0 cost */
    uint8_t     cost_0;
    /** Cluster channel index */
    uint8_t     channel;
    /** Neighbor type \see msap_neighbor_type_e */
    uint8_t     type;
    /** Used power level for transmissions (for next hop) */
    uint8_t     tx_power_level;
    /** Received power level (for next hop) */
    uint8_t     rx_power_level;
    /** Time since last update time in seconds */
    uint16_t    last_update;
} msap_neighbor_entry_t;

static bool getNbors(msap_neighbor_entry_t * nbor_entry,uint32_t idx )  
    app_res_e result = APP_RES_OK;
    app_lib_state_nbor_list_t nbors_list;
    msap_neighbor_entry_t nbor_entry;
    result = lib_state->getNbors(&nbors_list);
    if (result == APP_RES_OK)
    {
        uint32_t idx = 0;
        app_lib_state_nbor_info_t * info = nbors_list.nbors;
        for(idx = 0; idx < nbors_list.number_nbors; idx++)
        {
            // Clear
            memset(&nbor_entry, 0, sizeof(msap_neighbor_entry_t));
            // Copy
            nbor_entry.addr = Addr_to_Waddr((app_addr_t)info->address);
            nbor_entry.channel = info->channel;
            nbor_entry.cost_0 = info->cost;
            // Convert last update time-stamp to time since last update
            nbor_entry.last_update = info->last_update;
            nbor_entry.link_rel = info->link_reliability;
            nbor_entry.rssi_norm = info->norm_rssi;
            nbor_entry.rx_power_level = info->rx_power;
            nbor_entry.tx_power_level = info->tx_power;
            // Do the switcharoo
            if(info->type == APP_LIB_STATE_NEIGHBOR_IS_NEXT_HOP)
            {
                nbor_entry.type = APP_LIB_STATE_NEIGHBOR_IS_NEXT_HOP;
            }
            else if(info->type == APP_LIB_STATE_NEIGHBOR_IS_MEMBER)
            {
                nbor_entry.type = NEIGHBOR_IS_MEMBER;
            }
            else
            {
                nbor_entry.type = NEIGHBOR_IS_CLUSTER;
            }
            // Copy out
            memcpy(&item->frame.msap.nbor_cnf.neighbors[idx],
                   &nbor_entry,
                   sizeof(msap_neighbor_entry_t));
            // Don't leak memory like a dummy
            if(idx == MSAP_MAX_NBORS)
            {
                // No more entries fit the buffer
                break;
            }
            info++;
        }

        item->frame.msap.nbor_cnf.result = nbors_list.number_nbors;
    }
    else
    {
        item->frame.msap.nbor_cnf.result = 0;
        return false;
    }
    return true;
}
