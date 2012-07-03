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

#include "whna.h"

#include <stdio.h>
#include <string.h>

#include "../../../src/olsrd_plugin.h"

#include "olsrd_plugin.h"
#include "olsr.h"
#include "scheduler.h"

#define PLUGIN_INTERFACE_VERSION 5


int set_plugin_hna_weight(const char *hna_weight, void *data, set_plugin_parameter_addon addon);
int set_plugin_fakenode_addr(const char *value, void *data, set_plugin_parameter_addon addon);

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

static int
set_plugin_hna(const char *value, void *data __attribute__ ((unused)), set_plugin_parameter_addon addon __attribute__ ((unused)))
{
  union olsr_ip_addr temp_addr;
  union olsr_ip_addr temp_mask;
  char s_addr[128];
  char s_mask[128];
  
  //Example: 192.168.1.0  255.255.255.0
  int i = sscanf(value, "%127s %127s", s_addr, s_mask);
  if (i != 2) {
    OLSR_PRINTF(0, "Cannot get IP address and netmask from \"%s\"", value);
    return 1;
  }

  if (inet_pton(olsr_cnf->ip_version, s_addr, &temp_addr) <= 0) {
    OLSR_PRINTF(0, "Illegal IP address \"%s\"", s_addr);
    return 1;
  }

  if (inet_pton(olsr_cnf->ip_version, s_mask, &temp_mask) <= 0) {
    OLSR_PRINTF(0, "Illegal netmask \"%s\"", s_mask);
    return 1;
  }

  memcpy(&whna_hna4_addr, &temp_addr, sizeof(temp_addr));
  memcpy(&whna_hna4_netmask, &temp_mask, sizeof(temp_mask));

  return 0;
}

int
set_plugin_hna_weight(const char *hna_weight, void *data __attribute__ ((unused)), set_plugin_parameter_addon addon __attribute__ ((unused)))
{
		whna_target_etx = atof(hna_weight);
		if (whna_target_etx < 1.0f)
		{
			OLSR_PRINTF(0, "Target ETX must be >=1.0");
			return 1;
		}
		return 0;
}

int
set_plugin_fakenode_addr(const char *value, void *data __attribute__ ((unused)), set_plugin_parameter_addon addon __attribute__ ((unused)))
{
  union olsr_ip_addr temp_addr;
  char s_addr[128];
  
  int i = sscanf(value, "%127s", s_addr);
  if (i != 1) {
    OLSR_PRINTF(0, "Cannot get IP address from \"%s\"", value);
    return 1;
  }

  if (inet_pton(olsr_cnf->ip_version, s_addr, &temp_addr) <= 0) {
    OLSR_PRINTF(0, "Illegal IP address \"%s\"", s_addr);
    return 1;
  }

  memcpy(&whna_fakenode_address, &temp_addr, sizeof(temp_addr));
  return 0;
}

/**
 * Register parameters from config file
 * Called for all plugin parameters
 */
static const struct olsrd_plugin_parameters plugin_parameters[] = {
  {.name = "WHna4",.set_plugin_parameter = &set_plugin_hna,.data = NULL},
  {.name = "WHnaTargetETX",.set_plugin_parameter = &set_plugin_hna_weight,.data = NULL},
  {.name = "WHnaFakeNodeAddress",.set_plugin_parameter = &set_plugin_fakenode_addr,.data = NULL},
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
  printf("*** WHNA: plugin_init\n");

  /* call a function from main olsrd */
  olsr_printf(2, "*** WHNA: printed this with olsr_printf\n");
  
  fake_seqno = random() & 0xFFFF;
  fake_msg_seqno = random() & 0xFFFF;
  olsr_start_timer(ifnet->hello_etime, 0, OLSR_TIMER_PERIODIC, &whna_inject_hello, NULL, 0);
  //olsr_start_timer(olsr_cnf->interfaces->cnf->hna_params.emission_interval, 0, OLSR_TIMER_PERIODIC, &whna_inject_hna, NULL, 0);
  olsr_start_timer(2*ifnet->hello_etime, 0, OLSR_TIMER_PERIODIC, &whna_inject_hna, NULL, 0);
  //olsr_start_timer(ifnet->valtimes.tc, 0, OLSR_TIMER_PERIODIC, &whna_inject_tc, NULL, 0);

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
  printf("*** WHNA: destructor\n");
}

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
