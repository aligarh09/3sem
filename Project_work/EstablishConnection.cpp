#include "EstablishConnection.h"

int EstablishConnection::set_context_creation_info(struct lws_context_creation_info* context_creation_info, struct lws_protocols* protocols)
{
  if(!context_creation_info || !protocols)
    return -1;

  memset(context_creation_info, 0, sizeof *context_creation_info);
  context_creation_info -> extensions = lws_get_internal_extensions();
  context_creation_info -> port = CONTEXT_PORT_NO_LISTEN;
  context_creation_info -> ssl_private_key_filepath = NULL;
  context_creation_info -> ssl_cert_filepath = NULL;
  context_creation_info -> protocols = protocols;
  context_creation_info -> iface = NULL;
  context_creation_info -> options = 0;
  context_creation_info -> gid = -1;
  context_creation_info -> uid = -1;
}

int EstablishConnection::set_protocol_data(struct lws_protocols* protocols, int prot_num, 
                          const char * name, lws_callback_function * callback,  
                          size_t per_session_data_size, size_t rx_buffer_size,
                          unsigned int id, void * user)
{
  if(!protocols || !callback)
    return -1;

  protocols[prot_num].name = name;
  protocols[prot_num].per_session_data_size = per_session_data_size;
  protocols[prot_num].callback = callback;
  protocols[prot_num].rx_buffer_size = rx_buffer_size;
  protocols[prot_num].user = user;
  protocols[prot_num].id = id;
}

EstablishConnection::EstablishConnection()
{
  context_creation_info = {0};
  client_connect_info = {0};
  protocols[1] = {0};
  protocols[0] = {0};
  force_exit = 0;
  context = NULL;
  wsi = NULL;
}

int EstablishConnection::connect(int (*ws_service_callback_pointer)(struct lws *, enum lws_callback_reasons, void* , void* , size_t),
            ConnectionData user_data, void* thread_args_struct, void (*thread_routine_function)(void*, struct lws_context*, struct lws*))
{
  set_protocol_data(protocols, 0, "", ws_service_callback_pointer, 0, 0, 0, NULL);

  set_context_creation_info(&context_creation_info, protocols);

  context = lws_create_context(&context_creation_info);           //creates the listening socket and takes care of all initialization in one step

  user_data.construct_creation_info(&client_connect_info, context);

  if (context == NULL) 
  {
    printf("Context is NULL.\n");
    return -1;
  }
  else
    printf("Context created.\n");

  wsi = lws_client_connect_via_info(&client_connect_info);        //This function creates a connection to a remote server
  
  if (wsi == NULL) 
  {
      printf("Wsi create error.\n");
      return -1;
  }
  
  std::thread server_responce_service(thread_routine_function, thread_args_struct, context, wsi);
  server_responce_service.detach();

  while(!force_exit)
  {
    lws_service(context, 10);                                     //lws_service - Service any pending websocket activity
  }

  lws_context_destroy(context);
}

int EstablishConnection::close_connection()
{
  force_exit = 1;
}