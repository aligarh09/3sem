#pragma once

#include "ParseCmdArgs.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libwebsockets.h>

#ifndef MAX_SERVER_REQUEST_LEN
#define MAX_SERVER_REQUEST_LEN 500
#endif

#ifndef MAX_SERVER_ADRESS_LEN
#define MAX_SERVER_ADRESS_LEN  300
#endif

#ifndef MAX_SERVER_PATH_LEN
#define MAX_SERVER_PATH_LEN    300
#endif

#ifndef MAX_ORIGIN_LEN
#define MAX_ORIGIN_LEN         200
#endif

#ifndef MAX_RECORD_LEN
#define MAX_RECORD_LEN         100
#endif

#ifndef MAX_PROTOCOL_LEN
#define MAX_PROTOCOL_LEN        10
#endif

class ConnectionData
{
  private:

    char first_query[MAX_SERVER_REQUEST_LEN];
    char connection_origin[MAX_ORIGIN_LEN];
    char server_path[MAX_SERVER_PATH_LEN];
    const char* connection_protocol;
    const char* server_address;
    int  is_there_first_query;
    int  ietf_version;
    int  port_used;
    int  ssl_used;
    int  log_level;

    int record_iput_data(char* record_name, char* filename_to_record_in);

    int load_record(FILE* file_get_from, char* record_name);

    void delete_bracket(const char* str_to_clean);

  public:

    void data_dump(FILE* dump, const ConnectionData* current_connection);

    void print_usage();

    ConnectionData();

    void LoadSession (std::string record_to_load, char* filename_to_open);
   
    void CreateSession(ParseCmdArgs args_parsed, char* file_to_record_in);

    int check_name_existance (char* record_name, FILE* file_check_in);

    int is_there_query();
   
    int get_first_query(std::string* str_to_write);

    int get_first_query(char* str_to_write);

    int construct_creation_info(struct lws_client_connect_info* client_connect_info, struct lws_context * context);
    
};