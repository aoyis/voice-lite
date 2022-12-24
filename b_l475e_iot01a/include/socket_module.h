#ifndef __SOCKET_MODULE_H__
#define __SOCKET_MODULE_H__

#include "mbed.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET

class SocketModule {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 1024;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 6969; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketModule(Semaphore &sem);
    ~SocketModule();
    void run();
    void send_http_request(char* buffer);

private:
    NetworkInterface *net_;
    Semaphore *sem_;
    bool resolve_hostname(SocketAddress &address);
    void wifi_scan();
    void print_network_info();

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

#endif // __SOCKET_MODULE_H__