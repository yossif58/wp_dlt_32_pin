
#ifndef _CONCEPTS_H_
#define _CONCEPTS_H_

/**

@page concepts Wirepas Mesh Concepts

This page describes various <I>concepts</I> used in Wirepas Mesh networks. The
concept itself is described. The link to the services are also part of the
concept description.

@section appconfig Application Configuration Data

The application configuration service (commonly referred as <I>app config</I>)
can be used for two things:
-# Configure application-specific parameters (application configuration data) to
   the application running in the nodes (via the network)
-# Configure transmission interval of the stack diagnostic data


The application configuration data is persistent global data for the whole
network. The data format can be decided by the application. Application
configuration data can be set by the sinks' application after which it is
disseminated to the whole network and stored at every node. It can include e.g.
application parameters, such as measurement interval. The service makes it
possible to set the data, after which every new node joining the network
receives the data from its neighbors without the need for end-to-end polling.
Furthermore, new configurations can be set and updated to the network on the
run.

The Single-MCU Api contains following services for using the app config:
-# <code>@ref app_lib_data_write_app_config_f "lib_data->writeAppConfig()"</code>
   for updating the app config.
-# <code>@ref app_lib_data_read_app_config_f "lib_data->readAppConfig()"</code> for
   reading the app config.
-# <code>@ref app_lib_data_set_new_app_config_cb_f "lib_data->setNewAppConfigCb()"
   </code> for setting the callback that is called when app config changes.
-# <code>@ref app_lib_data_get_app_config_num_bytes_f "lib_data->getAppConfigNumBytes()"
   </code> for querying the amount of configuration data allowed.

\note Writing of app config can only be used in sink role.
\note In a network including multiple sinks, the same configuration data should
      be set to all sinks so that it can be guaranteed to disseminate to every
      node.
\note Application configuration data is stored in permanent memory similarly to
      the persistent attributes. To avoid memory wearing, do not write new
      values too often (e.g. more often than once per 30 minutes).


 */

#endif /* API_DOC_CONCEPTS_H_ */
