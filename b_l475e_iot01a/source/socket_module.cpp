#include <cstdio>
#include <cstring>
#include "socket_module.h"

SocketModule::SocketModule(Semaphore &sem) : net_(NetworkInterface::get_default_instance()), sem_(&sem) {
}

SocketModule::~SocketModule() {
    if (net_) {
        net_->disconnect();
    }
}

void SocketModule::run() {
    if (!net_) {
        sem_->acquire();
        printf("Error! No network interface found.\r\n");
        sem_->release();
        return;
    } else {
        sem_->acquire();
        printf("\r\nSocket Initialized.\r\n"); 
        sem_->release();
    }
    /* if we're using a wifi interface run a quick scan */
    if (net_->wifiInterface()) {
        /* the scan is not required to connect and only serves to show visible access points */
        // _net->wifiInterface();
        wifi_scan();

        /* in this example we use credentials configured at compile time which are used by
            * NetworkInterface::connect() but it's possible to do this at runtime by using the
            * WiFiInterface::connect() which takes these parameters as arguments */
    }

    /* connect will perform the action appropriate to the interface type to connect to the network */
    sem_->acquire();
    printf("Connecting to the network...\r\n");
    sem_->release();

    nsapi_size_or_error_t result = net_->connect();
    if (result != 0) {
        sem_->acquire();
        printf("Error! _net->connect() returned: %d\r\n", result);
        sem_->release();
        return;
    }

    print_network_info();

    /* opening the socket only allocates resources */
    result = _socket.open(net_);
    if (result != 0) {
        sem_->acquire();
        printf("Error! _socket.open() returned: %d\r\n", result);
        sem_->release();
        return;
    }

#if MBED_CONF_APP_USE_TLS_SOCKET
    result = _socket.set_root_ca_cert(root_ca_cert);
    if (result != NSAPI_ERROR_OK) {
        sem_->acquire();
        printf("Error: _socket.set_root_ca_cert() returned %d\n", result);
        sem_->release();
        return;
    }
    _socket.set_hostname(MBED_CONF_APP_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

    /* now we have to find where to connect */

    SocketAddress address;

    if (!resolve_hostname(address)) {
        return;
    }

    address.set_port(REMOTE_PORT);

    /* we are connected to the network but since we're using a connection oriented
        * protocol we still need to open a connection on the socket */
    sem_->acquire();
    printf("Opening connection to remote port %d...\r\n", REMOTE_PORT);
    printf("\r\n");
    sem_->release();

    result = _socket.connect(address);
    if (result != 0) {
        sem_->acquire();
        printf("Error! _socket.connect() returned: %d\r\n", result);
        sem_->release();
        return;
    }
}

bool SocketModule::resolve_hostname(SocketAddress &address) {
    const char hostname[] = MBED_CONF_APP_HOSTNAME;

    /* get the host address */
    sem_->acquire();
    printf("\nResolving hostname %s\r\n", hostname);
    sem_->release();
    nsapi_size_or_error_t result = net_->gethostbyname(hostname, &address);
    if (result != 0) {
        sem_->acquire();
        printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
        sem_->release();
        return false;
    }

    sem_->acquire();
    printf("Address: %s\r\n", (address.get_ip_address() ? address.get_ip_address() : "None") );
    sem_->release();

    return true;
}

void SocketModule::send_http_request(char* buffer) {
    nsapi_size_t bytes_to_send = strlen(buffer);
    nsapi_size_or_error_t bytes_sent = 0;

    while (bytes_to_send) {
        bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
        if (bytes_sent < 0) {
            sem_->acquire();
            printf("Error! _socket.send() returned: %d\r\n", bytes_sent);
            sem_->release();
            return;
        }
        bytes_to_send -= bytes_sent;
    }
}


void SocketModule::wifi_scan() {
    WiFiInterface *wifi = net_->wifiInterface();

    WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

    /* scan call returns number of access points found */
    int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

    if (result <= 0) {
        sem_->acquire();
        printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
        sem_->release();
        return;
    }

    sem_->acquire();
    printf("%d networks available:\r\n", result);
    sem_->release();

    sem_->acquire();
    for (int i = 0; i < result; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                ap[i].get_rssi(), ap[i].get_channel());  
    }
    printf("\r\n");
    sem_->release();
}

void SocketModule::print_network_info() {
    /* print the network info */
    SocketAddress a;
    net_->get_ip_address(&a);
    sem_->acquire();
    printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    sem_->release();
    
    net_->get_netmask(&a);
    sem_->acquire();
    printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    sem_->release();
    
    net_->get_gateway(&a);
    sem_->acquire();
    printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    sem_->release();
}