/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#undef USE_DEBUG_PRINT_MODULE

#include "dsap.h"
#include "dsap_frames.h"
#include "function_codes.h"
#include "waps_private.h"
#include "waddr.h"
#include "lock_bits.h"
#include "api.h"

/**
 * \brief   Called when a WAPS data packet should be created.
 * \param   item
 *          Received request
 * \return  True, if a reply frame was constructed
 */
static bool sendPacket(waps_item_t * item);

/**
 * \brief   Called when a WAPS data packet should be created.
 * \note    Identical to \ref sendPacket, except request contains initial
 *          travel time.
 * \param   item
 *          Received request
 * \return  True, if a reply frame was constructed
 */
static bool sendPacketWithTT(waps_item_t * item);

/**
 * \brief   Update delay (callback from protocol interface)
 * \param   item
 *          Item that gets updated
 */
 /**
  * \brief   Called when a WAPS data packet should be created.
  * \param   item
  *          Received request
  * \return  True, if a reply frame was constructed
  */
static void update_packet_delay(waps_item_t * item);

#ifdef WITH_FRAG
 /**
  * \brief   Called when a WAPS data packet should be created.
  * \note    Identical to \ref sendPacket, except request contains initial
  *          travel time.
  * \param   item
  *          Received request
  * \return  True, if a reply frame was constructed
  */
static bool sendSduPacket(waps_item_t * item);
#endif

bool Dsap_handleFrame(waps_item_t * item)
{
    switch (item->frame.sfunc)
    {
        case WAPS_FUNC_DSAP_DATA_TX_REQ:
            return sendPacket(item);
        case WAPS_FUNC_DSAP_DATA_TX_TT_REQ:
            return sendPacketWithTT(item);
#ifdef WITH_FRAG
        case WAPS_FUNC_DSAP_SDU_TX_REQ:
            return sendSduPacket(item);
#endif
        default:
            return false;
    }
}

void Dsap_packetSent(pduid_t id,
                     uint8_t src_ep,
                     uint8_t dst_ep,
                     uint32_t queue_time,
                     w_addr_t dst,
                     bool success,
                     waps_item_t * output_ptr)
{
    // Build response
    Waps_item_init(output_ptr,
                   WAPS_FUNC_DSAP_DATA_TX_IND,
                   sizeof(dsap_data_tx_ind_t));
    output_ptr->time = 0;
    dsap_data_tx_ind_t * ind_ptr = &output_ptr->frame.dsap.data_tx_ind;
    ind_ptr->apdu_id = (pduid_t)id;
    ind_ptr->dst_addr = (w_addr_t)dst;
    ind_ptr->dst_endpoint = dst_ep;
    ind_ptr->queue_delay = queue_time;
    ind_ptr->queued_indications = 0;
    ind_ptr->src_endpoint = src_ep;
    ind_ptr->result = success ? DSAP_IND_SUCCESS : DSAP_IND_TIMEOUT;
}

void Dsap_packetReceived(const app_lib_data_received_t * data,
                         w_addr_t dst_addr,
                         waps_item_t * output_ptr)
{
    // Create indication
    Waps_item_init(output_ptr,
                   WAPS_FUNC_DSAP_DATA_RX_IND,
                   FRAME_DSAP_DATA_RX_IND_HEADER_SIZE + data->num_bytes);
    dsap_data_rx_ind_t * ind_ptr = &output_ptr->frame.dsap.data_rx_ind;
    ind_ptr->src_endpoint = data->src_endpoint;
    ind_ptr->src_addr = data->src_address;
    ind_ptr->dst_addr = dst_addr;
    ind_ptr->dst_endpoint = data->dest_endpoint;
    // Info field
    ind_ptr->info =
        (data->qos << RX_IND_INFO_QOS_OFFSET) & RX_IND_INFO_QOS_MASK;
    ind_ptr->info |=
        (data->hops << RX_IND_INFO_HOPCOUNT_OFFSET) & RX_IND_INFO_HOPCOUNT_MASK;

    memcpy(ind_ptr->apdu, data->bytes, data->num_bytes);
    // Initialize delay and protocol delay incrementing function callback
    output_ptr->time = lib_time->getTimestampCoarse();
    output_ptr->pre_cb = update_packet_delay;
    ind_ptr->delay = data->delay;
    ind_ptr->queued_indications = 0;
    ind_ptr->apdu_len = data->num_bytes;
}

static bool sendPacketWithTT(waps_item_t * item)
{
    app_lib_data_to_send_t data;
    size_t capacity = 0;
    app_lib_data_send_res_e result;

    dsap_data_tx_tt_req_t * req = &item->frame.dsap.data_tx_tt_req;
    if (item->frame.splen != (FRAME_DSAP_DATA_TX_TT_REQ_HEADER_SIZE +
                              req->apdu_len))
    {
        // Discard request
        return false;
    }

    // Resp code
    uint8_t resp_code = WAPS_FUNC_DSAP_DATA_TX_TT_CNF;
    if(item->frame.sfunc == WAPS_FUNC_DSAP_DATA_TX_REQ)
    {
        // Do the switcharoo
        resp_code = WAPS_FUNC_DSAP_DATA_TX_CNF;
    }

    // Check that TX feature is permitted
    if (!LockBits_isFeaturePermitted(LOCK_BITS_DSAP_DATA_TX))
    {
        result = DSAP_TX_ACCESS_DENIED;
        goto create_response;
    }

    // Check if WADDR is valid
    if(!Waddr_addrIsValid(req->dst_addr))
    {
        // Address is not valid
        result = DSAP_TX_UNKNOWN_DST;
        goto create_response;
    }

    // Check TX options
    if(req->tx_opts & TX_OPTS_IND_REQ)
    {
        data.flags = APP_LIB_DATA_SEND_FLAG_TRACK;
    }
    else
    {
        data.flags = APP_LIB_DATA_SEND_FLAG_NONE;
    }
    if (req->tx_opts & TX_OPTS_UNACK_CSMA_CA)
    {
        data.flags |= APP_LIB_DATA_SEND_FLAG_UNACK_CSMA_CA;
    }
    if (req->tx_opts & TX_OPTS_HOPLIMIT_MASK)
    {
        uint8_t hoplimit = (req->tx_opts & TX_OPTS_HOPLIMIT_MASK) >>
            TX_OPTS_HOPLIMIT_OFFSET;
        data.flags |= APP_LIB_DATA_SEND_SET_HOP_LIMITING;
        data.hop_limit = hoplimit;
    }

    // Quality of service
    switch(req->qos)
    {
        case DSAP_QOS_NORMAL:
            data.qos = APP_LIB_DATA_QOS_NORMAL;
            break;
        case DSAP_QOS_HIGH:
            data.qos = APP_LIB_DATA_QOS_HIGH;
            break;
        case DSAP_QOS_UNACKED: // TODO This should be a valid TC to use
        default:
            result = DSAP_TX_INV_QOS_PARAM;
            goto create_response;
    }

    // Add internal delay to travel time field (in addition to initial tt)
    data.delay = lib_time->getTimestampCoarse() - item->time + req->travel_time;
    data.bytes        = &req->apdu[0];
    data.num_bytes    = req->apdu_len;
    data.dest_address = Waddr_to_Addr(req->dst_addr);
    data.tracking_id  = req->apdu_id;
    data.src_endpoint = req->src_endpoint;
    data.dest_endpoint = req->dst_endpoint;

    // Send packet
    result = lib_data->sendData(&data);

    // TODO Attr_manager_getPduBuffCapacity check this
    // Get remaining buffer capacity (do not pre-check and decrement = lie)
    lib_data->getNumFreeBuffers(&capacity);

    // Response generation
create_response:
    Waps_item_init(item, resp_code, sizeof(dsap_data_tx_cnf_t));
    item->frame.dsap.data_tx_cnf.apdu_id = req->apdu_id;
    item->frame.dsap.data_tx_cnf.buff_cap = (uint8_t) capacity;
    item->frame.dsap.data_tx_cnf.result = (uint8_t) result;
    return true;
}

static bool sendPacket(waps_item_t * item)
{
    dsap_data_tx_req_t * req = &item->frame.dsap.data_tx_req;
    dsap_data_tx_tt_req_t * tt_req = (dsap_data_tx_tt_req_t *)req;
    if (item->frame.splen != (FRAME_DSAP_DATA_TX_REQ_HEADER_SIZE +
                              req->apdu_len))
    {
        return false;
    }
    // Marshal to tt_req_t type
    uint8_t apdu_len = req->apdu_len;
    memmove(&tt_req->apdu[0], &req->apdu[0], apdu_len);
    tt_req->apdu_len = apdu_len;
    tt_req->travel_time = 0;
    // Fake size to be a bit higher
    item->frame.splen += sizeof(uint32_t);
    return sendPacketWithTT(item);
}

static void update_packet_delay(waps_item_t * item)
{
    if (item != NULL)
    {
        uint32_t now = lib_time->getTimestampCoarse();
        uint32_t local_delay = now - item->time;
        item->time = now;
        if (item->frame.sfunc == WAPS_FUNC_DSAP_DATA_RX_IND)
        {
            item->frame.dsap.data_rx_ind.delay += local_delay;
        }
#ifdef WITH_FRAG
        else if (item->frame.sfunc == WAPS_FUNC_DSAP_SDU_RX_IND)
        {
            item->frame.dsap.sdu_rx_ind.delay += local_delay;
        }
#endif
    }
}

#ifdef WITH_FRAG
static app_res_e
sendSdu(uint8_t * data, uint32_t len, app_addr_t peer, uint8_t pid)
{
    app_lib_sdu_to_send_t sdu;
    app_res_e stat;

    sdu.bytes = data;
    sdu.num_bytes = len;
    sdu.dest_address = peer;
    sdu.payload_id = pid;

    if((stat = lib_sdu->send(&sdu)) == APP_RES_OK)
    {
        //VAPRINT("Reply SDU to %06lx\n", (unsigned long)peer);
    }
    else
    {
        //VAPRINT("send() failed: %u\n", (unsigned int)stat);
    }

    return stat;
}

static bool sendSduPacket(waps_item_t * item)
{
    app_res_e apl_result;
    app_lib_data_send_res_e result = APP_LIB_DATA_SEND_RES_SUCCESS;
    dsap_sdu_tx_req_t * req = &item->frame.dsap.sdu_tx_req;

    if (item->frame.splen != (FRAME_DSAP_SDU_TX_REQ_HEADER_SIZE +
                              req->apsdu_len))
    {
        // Discard request
        return false;
    }

    // Check that TX feature is permitted
    if (!LockBits_isFeaturePermitted(LOCK_BITS_DSAP_DATA_TX))
    {
        result = APP_LIB_DATA_SEND_RES_ACCESS_DENIED;
        goto create_response;
    }

    // Check if WADDR is valid
    if(!Waddr_addrIsValid(req->addr))
    {
        // Address is not valid
        result = DSAP_TX_UNKNOWN_DST;
        goto create_response;
    }
    // Send packet
    apl_result = sendSdu(&req->apsdu[0], req->apsdu_len,
                            Waddr_to_Addr(req->addr),
                            req->payload_id);

    if (apl_result == APP_RES_RESOURCE_UNAVAILABLE)
    {
        result = APP_LIB_DATA_SEND_RES_OUT_OF_MEMORY;
    }
    else if (apl_result != APP_RES_OK)
    {
        result = APP_LIB_DATA_SEND_RES_ACCESS_DENIED;
    }

    // Response generation
create_response:
    Waps_item_init(item, WAPS_FUNC_DSAP_SDU_TX_CNF, sizeof(dsap_sdu_tx_cnf_t));
    item->frame.dsap.sdu_tx_cnf.reserved = 0;
    item->frame.dsap.sdu_tx_cnf.result = (uint8_t) result;
    return true;
}


void Dsap_sduPacketSent(uint8_t payload_id,
                        uint32_t queue_time,
                        w_addr_t addr,
                        bool success,
                        waps_item_t * output_ptr)
{
    // Build response
    (void)queue_time;
    Waps_item_init(output_ptr,
                   WAPS_FUNC_DSAP_SDU_TX_IND,
                   sizeof(dsap_data_tx_ind_t));
    output_ptr->time = 0;
    dsap_sdu_tx_ind_t * ind_ptr = &output_ptr->frame.dsap.sdu_tx_ind;
    ind_ptr->payload_id = payload_id;
    ind_ptr->addr = (w_addr_t)addr;
    ind_ptr->queued_indications = 0;
    ind_ptr->result = success ? DSAP_IND_SUCCESS : DSAP_IND_TIMEOUT;
}

void Dsap_sduPacketReceived(const uint8_t * bytes,
                            uint16_t size,
                            w_addr_t addr,
                            uint8_t payload_id,
                            uint32_t delay,
                            waps_item_t * output_ptr)
{
    // Create indication

    if(size > APSDU_MAX_SIZE )
    {
        //TODO:Make this better
        return;
    }

    Waps_item_init(output_ptr,
                   WAPS_FUNC_DSAP_SDU_RX_IND,
                   FRAME_DSAP_SDU_RX_IND_HEADER_SIZE + size);
    dsap_sdu_rx_ind_t * ind_ptr = &output_ptr->frame.dsap.sdu_rx_ind;
    ind_ptr->addr = addr;
    memcpy(ind_ptr->apsdu, bytes, size);
    ind_ptr->queued_indications = 0;
    ind_ptr->payload_id = payload_id;
    ind_ptr->delay = delay;
    ind_ptr->apsdu_len = size;
}

#endif
