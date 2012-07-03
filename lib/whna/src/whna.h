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


#ifndef _WHNA_H
#define _WHNA_H

#define PLUGIN_NAME "OLSRD Weighted HNA plugin"
#define PLUGIN_NAME_SHORT "OLSRD WHNA"
#define PLUGIN_VERSION "0.0.1 (" __DATE__ " " __TIME__ ")"
#define PLUGIN_COPYRIGHT "  (C) Ninux.org"
#define PLUGIN_AUTHOR "  Claudio Pisa (clauz@ninux.org)"
#define MOD_DESC PLUGIN_NAME " " PLUGIN_VERSION "\n" PLUGIN_COPYRIGHT "\n" PLUGIN_AUTHOR
#define PLUGIN_INTERFACE_VERSION 5

/* System includes */
#include <arpa/inet.h>

/* OLSR includes */
#include "olsr_types.h"         
#include "olsrd_plugin.h"      
#include "olsr.h"
#include "ifnet.h"
#include "lq_packet.h"
#include "process_package.h"
#include "ipcalc.h"
#include "olsr_protocol.h"
#include "parser.h"


extern float whna_target_etx;
extern union olsr_ip_addr whna_hna4_addr;
extern union olsr_ip_addr whna_hna4_netmask;
extern union olsr_ip_addr whna_fakenode_address;
extern uint16_t fake_seqno;
extern uint16_t fake_msg_seqno;

struct fake_lq_hello {
  uint16_t olsr_packlen;
  uint16_t olsr_seqno;
  /*lq hello*/
  uint8_t olsr_msgtype;
  uint8_t olsr_vtime;
  uint16_t olsr_msgsize;
  uint32_t originator;
  uint8_t ttl;
  uint8_t hopcnt;
  uint16_t seqno;
  uint16_t reserved0;
  uint8_t etime;
  uint8_t willingness;
  uint8_t link_type;
  uint8_t reserved1;
  uint16_t size;
  uint32_t neigh_addr;
  uint8_t lq;
  uint8_t nlq;
  uint16_t reserved2;
} __attribute__((packed));

struct fake_hna {
  uint16_t olsr_packlen;
  uint16_t olsr_seqno;
  /*hna*/
  uint8_t olsr_msgtype;
  uint8_t olsr_vtime;
  uint16_t olsr_msgsize;
  uint32_t originator;
  uint8_t ttl;
  uint8_t hopcnt;
  uint16_t seqno;
  uint32_t address;
  uint32_t netmask;
} __attribute__((packed));

void whna_build_hello(struct fake_lq_hello *hello);
void whna_build_hna(struct fake_hna *hna);

void whna_inject_hello(void *foo);
void whna_inject_hna(void *foo);

#endif

