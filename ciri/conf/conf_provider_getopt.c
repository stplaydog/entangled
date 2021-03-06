/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "ciri/conf/conf.h"
#include "ciri/conf/usage.h"

static struct option* build_options() {
  size_t nbr = 0;
  while (ciri_arguments_g[nbr].desc) {
    nbr++;
  }
  struct option* options = malloc((nbr + 1) * sizeof(struct option));
  size_t i;
  for (i = 0; i < nbr; i++) {
    options[i].name = ciri_arguments_g[i].name;
    if (ciri_arguments_g[i].has_arg == NO_ARG) {
      options[i].has_arg = no_argument;
    } else if (ciri_arguments_g[i].has_arg == REQUIRED_ARG) {
      options[i].has_arg = required_argument;
    } else if (ciri_arguments_g[i].has_arg == OPTIONAL_ARG) {
      options[i].has_arg = optional_argument;
    }
    options[i].flag = NULL;
    options[i].val = ciri_arguments_g[i].val;
  }
  options[i].name = NULL;
  options[i].has_arg = 0;
  options[i].flag = NULL;
  options[i].val = 0;
  return options;
}

static logger_level_t get_log_level(char const* const log_level) {
  static struct log_level_map {
    char* str;
    logger_level_t level;
  } map[] = {{"debug", LOGGER_DEBUG},   {"info", LOGGER_INFO},
             {"notice", LOGGER_NOTICE}, {"warning", LOGGER_WARNING},
             {"error", LOGGER_ERR},     {"critical", LOGGER_CRIT},
             {"alert", LOGGER_ALERT},   {"emergency", LOGGER_EMERG},
             {NULL, LOGGER_INFO}};
  size_t i;
  for (i = 0; map[i].str != NULL && strcmp(map[i].str, log_level) != 0; i++)
    ;
  return map[i].level;
}

retcode_t ciri_conf_parse(ciri_conf_t* conf, int argc, char** argv) {
  retcode_t ret = RC_OK;
  int arg;
  struct option* long_options = build_options();

  if ((ret = ciri_conf_init(conf))) {
    return ret;
  }
  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (arg) {
      case 'h':  // --help
        iota_usage();
        free(long_options);
        exit(EXIT_SUCCESS);
        break;
      case 'l':  // --log-level
        conf->log_level = get_log_level(optarg);
        break;
      case 'p':  // --port
        conf->api_port = atoi(optarg);
        break;
      case 'n':  // --neighbors
        conf->neighbors = optarg;
        break;
      case 'c':  // --config
        conf->conf_file = optarg;
        break;
      case 'u':  // --udp-receiver-port
        conf->udp_receiver_port = atoi(optarg);
        break;
      case 't':  // --tcp-receiver-port
        conf->tcp_receiver_port = atoi(optarg);
        break;
      case 'e':  // --testnet
        conf->testnet = true;
        break;
      case 'r':  // --remote
        conf->remote = true;
        break;
      case 'a':  // --remote-auth
        conf->remote_auth_token = optarg;
        break;
      case 'i':  // --remote-limit-api
        conf->remote_limit_api = optarg;
        break;
      case 's':  // --send-limit
        conf->send_limit = atof(optarg);
        break;
      case 'm':  // --max-peers
        conf->max_peers = atoi(optarg);
        break;
      case 'd':  // --dns-resolution-false
        conf->dns_resolution = false;
        break;
      default:
        iota_usage();
        free(long_options);
        return RC_CIRI_CONF_INVALID_ARGUMENTS;
    }
  }
  free(long_options);
  return RC_OK;
};
