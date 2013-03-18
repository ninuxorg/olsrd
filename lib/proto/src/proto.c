/*
 * OLSRd proto plugin
 *
 * Copyright (c) 2012 Claudio Pisa <clauz@ninux.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

#include "proto.h"

int target_proto_no = 3;
int target_table_no = -1;
int sock = -1;
struct sockaddr_nl nladdr;

void
proto_inject_hnas (int fd, void *data __attribute__ ((unused)), unsigned int flags __attribute__ ((unused)))
{
	union olsr_ip_addr proto_hna4_addr;
	uint8_t proto_hna4_netmask_length;

	int     received_bytes = 0;
    struct  nlmsghdr *nlh;
    char    destination_address[32];
    unsigned char    route_protocol = 0;
    char    gateway_address[32];
    struct  rtmsg *route_entry;  /* This struct represent a route entry \
                                    in the routing table */
    struct  rtattr *route_attribute; /* This struct contain route \
                                            attributes (route type) */
    int     route_attribute_len = 0;
    char    buffer[BUFFER_SIZE];

	olsr_printf(7, "*** PROTO: inject HNAs function\n");

    bzero(destination_address, sizeof(destination_address));
    bzero(gateway_address, sizeof(gateway_address));
    bzero(buffer, sizeof(buffer));

    /* Receiving netlink socket data */
    while (1)
    {
        received_bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (received_bytes < 0)
            ERR_RET("recv");
        /* cast the received buffer */
        nlh = (struct nlmsghdr *) buffer;
        /* If we received all data ---> break */
        if (nlh->nlmsg_type == NLMSG_DONE)
            break;
        /* We are just interested in Routing information */
        if (nladdr.nl_groups == RTMGRP_IPV4_ROUTE)
            break;
    }

    /* Reading netlink socket data */
    /* Loop through all entries */
    for ( ; NLMSG_OK(nlh, (unsigned int)received_bytes); \
                    nlh = NLMSG_NEXT(nlh, received_bytes))
    {
        /* Get the route data */
        route_entry = (struct rtmsg *) NLMSG_DATA(nlh);

        /* If we are just interested in one routing table */
        if (target_table_no >= 0 && route_entry->rtm_table != target_table_no)
            continue;

        if (route_entry->rtm_protocol != target_proto_no)
            continue;

		proto_hna4_netmask_length = (uint8_t)route_entry->rtm_dst_len;
		route_protocol = route_entry->rtm_protocol;

        /* Get attributes of route_entry */
        route_attribute = (struct rtattr *) RTM_RTA(route_entry);

        /* Get the route atttibutes len */
        route_attribute_len = RTM_PAYLOAD(nlh);
        /* Loop through all attributes */
        for ( ; RTA_OK(route_attribute, route_attribute_len); \
            route_attribute = RTA_NEXT(route_attribute, route_attribute_len))
        {
            /* Get the destination address */
            if (route_attribute->rta_type == RTA_DST)
            {
                inet_ntop(AF_INET, RTA_DATA(route_attribute), \
                        destination_address, sizeof(destination_address));
				memcpy(&proto_hna4_addr.v4.s_addr, RTA_DATA(route_attribute), sizeof(proto_hna4_addr.v4.s_addr));
            }
            /* Get the gateway (Next hop) */
            if (route_attribute->rta_type == RTA_GATEWAY)
                inet_ntop(AF_INET, RTA_DATA(route_attribute), \
                        gateway_address, sizeof(gateway_address));
        }

        /* Now we can dump the routing attributes */
        if (nlh->nlmsg_type == RTM_DELROUTE)
		{
            olsr_printf(3, "*** PROTO: Deleting HNA: %s/%d proto %d gateway %s\n", \
                destination_address, proto_hna4_netmask_length, route_protocol, gateway_address);
			ip_prefix_list_remove(&olsr_cnf->hna_entries, &proto_hna4_addr, proto_hna4_netmask_length);
		}
        if (nlh->nlmsg_type == RTM_NEWROUTE)
		{
            olsr_printf(3, "*** PROTO: Adding HNA: %s/%d proto %d and gateway %s\n", \
				destination_address, proto_hna4_netmask_length, route_protocol, gateway_address);
			ip_prefix_list_add(&olsr_cnf->hna_entries, &proto_hna4_addr, proto_hna4_netmask_length);
		}
    }

	return;

}

