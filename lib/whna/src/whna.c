/*
 * OLSRd weighted HNA plugin
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

#include "whna.h"

float whna_target_etx = 1.0;
union olsr_ip_addr whna_hna4_addr;
union olsr_ip_addr whna_hna4_netmask;
union olsr_ip_addr whna_fakenode_address;
uint16_t fake_seqno;
uint16_t fake_msg_seqno;

void whna_build_hello(struct fake_lq_hello *hello)
{
	struct interface *ifp = ifnet;

	hello->olsr_packlen = htons(32);
	hello->olsr_seqno = htons(fake_seqno++);
	hello->olsr_msgtype = LQ_HELLO_MESSAGE;
	hello->olsr_vtime = ifp->valtimes.hello;
	hello->olsr_msgsize = htons(28); 
	hello->originator = whna_fakenode_address.v4.s_addr;
	hello->ttl = 1;
	hello->hopcnt = 0;
	hello->seqno = htons(fake_msg_seqno++); //TODO: handle rollover?
	hello->etime = reltime_to_me(ifp->hello_etime);
	hello->willingness = 3;
	hello->link_type = 0x0a;
	hello->size = htons(12);
	hello->neigh_addr = ifp->int_addr.sin_addr.s_addr;
	hello->lq = 255 / whna_target_etx;
	hello->nlq = 0xff;
}

void whna_build_hna(struct fake_hna *hna)
{
	struct interface *ifp = ifnet;

	hna->olsr_packlen = htons(24);
	hna->olsr_seqno = htons(fake_seqno++);
	hna->olsr_msgtype = HNA_MESSAGE;
	hna->olsr_vtime = ifp->valtimes.hna;
	hna->olsr_msgsize = htons(20); 
	hna->originator = whna_fakenode_address.v4.s_addr;
	hna->ttl = 255;
	hna->hopcnt = 0;
	hna->seqno = htons(fake_msg_seqno++); //TODO: handle rollover?
	hna->address = whna_hna4_addr.v4.s_addr;
	hna->netmask = whna_hna4_netmask.v4.s_addr;
}

void
whna_inject_hello (void *foo __attribute__ ((unused)))
{
	struct fake_lq_hello *hello;
	struct interface *ifp = ifnet;

	hello = olsr_malloc(sizeof(struct fake_lq_hello), "WHNA hello");
	memset(hello, 0, sizeof(struct fake_lq_hello));

	/* inject a fake hello message from an imaginary node */
	whna_build_hello(hello);
	parse_packet((struct olsr *)hello, ntohs(hello->olsr_packlen), ifp, &whna_fakenode_address);

	free(hello);
	return;

}

void
whna_inject_hna (void *foo __attribute__ ((unused)))
{
	struct fake_hna *hna;
	struct interface *ifp = ifnet;

	hna = olsr_malloc(sizeof(struct fake_hna), "WHNA HNA");
	memset(hna, 0, sizeof(struct fake_hna));

	whna_build_hna(hna);

	parse_packet((struct olsr *)hna, ntohs(hna->olsr_packlen), ifp, &whna_fakenode_address);

	free(hna);

	return;

}

