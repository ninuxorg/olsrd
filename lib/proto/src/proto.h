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
#define PLUGIN_VERSION "0.0.2 (" __DATE__ " " __TIME__ ")"
#define PLUGIN_COPYRIGHT "  (C) Ninux.org"
#define PLUGIN_AUTHOR "  Claudio Pisa (clauz@ninux.org)"
#define MOD_DESC PLUGIN_NAME " " PLUGIN_VERSION "\n" PLUGIN_COPYRIGHT "\n" PLUGIN_AUTHOR
#define PLUGIN_INTERFACE_VERSION 5

/* System includes */
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <unistd.h>

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

extern int target_proto_no;
extern int target_table_no;
extern int sock;
extern struct sockaddr_nl nladdr;

#define ERR_RET(x) do { perror(x); return; } while (0);
#define BUFFER_SIZE 8192

struct nl_req_s {
  struct nlmsghdr hdr;
  struct rtgenmsg gen;
};

void proto_inject_hnas (int fd, void *data, unsigned int flags);
void existing_routes_hna_injection (int fd);

#endif

