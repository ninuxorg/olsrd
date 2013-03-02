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
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the UniK olsr daemon nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

 /*
  * Example plugin for olsrd.org OLSR daemon
  * Only the bare minimum
  */

#include "proto.h"

#include <stdio.h>
#include <string.h>

#include "../../../src/olsrd_plugin.h"

#include "olsrd_plugin.h"
#include "olsr.h"
#include "scheduler.h"

#define PLUGIN_INTERFACE_VERSION 5


int set_plugin_proto(const char *proto_no, void *data, set_plugin_parameter_addon addon);

/****************************************************************************
 *                Functions that the plugin MUST provide                    *
 ****************************************************************************/

/**
 * Plugin interface version
 * Used by main olsrd to check plugin interface version
 */
int
olsrd_plugin_interface_version(void)
{
  return PLUGIN_INTERFACE_VERSION;
}

int
set_plugin_proto(const char *proto_no, void *data __attribute__ ((unused)), set_plugin_parameter_addon addon __attribute__ ((unused)))
{
		target_proto_no = atoi(proto_no);
		return 0;
}

/**
 * Register parameters from config file
 * Called for all plugin parameters
 */
static const struct olsrd_plugin_parameters plugin_parameters[] = {
  {.name = "proto_no",.set_plugin_parameter = &set_plugin_proto,.data = NULL},
};

void
olsrd_get_plugin_parameters(const struct olsrd_plugin_parameters **params, int *size)
{
  *params = plugin_parameters;
  *size = sizeof(plugin_parameters) / sizeof(*plugin_parameters);
}


/**
 * Initialize plugin
 * Called after all parameters are passed
 */
int
olsrd_plugin_init(void)
{

  printf("*** PROTO: plugin_init\n");

  /* call a function from main olsrd */
  olsr_printf(2, "*** PROTO: printed this with olsr_printf\n");

  /* Zeroing nladdr */
  bzero (&nladdr, sizeof(nladdr));

  if ((sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
  {
	 olsr_printf(1, "*** PROTO: problem initializing netlink socket\n");
	 return 0;
  }

  nladdr.nl_family = AF_NETLINK;
  nladdr.nl_groups = RTMGRP_IPV4_ROUTE;

  if (bind(sock,(struct sockaddr *)&nladdr,sizeof(nladdr)) < 0)
  {
	 olsr_printf(1, "*** PROTO: problem binding socket\n");
	 return 0;
  }

  olsr_start_timer(2*ifnet->hello_etime, 0, OLSR_TIMER_PERIODIC, &proto_inject_hnas, NULL, 0);

  return 1;
}

/****************************************************************************
 *       Optional private constructor and destructor functions              *
 ****************************************************************************/

/* attention: make static to avoid name clashes */

static void my_init(void) __attribute__ ((constructor));
static void my_fini(void) __attribute__ ((destructor));

/**
 * Optional Private Constructor
 */
static void
my_init(void)
{
  printf("%s\n", MOD_DESC);
}

/**
 * Optional Private Destructor
 */
static void
my_fini(void)
{
  printf("*** PROTO: destructor\n");

  /* Close socket */
  close(sock);
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
