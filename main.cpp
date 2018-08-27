#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"

#define HTTP_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_HEADER_FIELDS "Content-Type: text/html; charset=utf-8"
#define HTTP_MESSAGE_BODY ""                                                  \
                          "<html>"                                            \
                          "\r\n"                                              \
                          "  <body style=\"display:flex;text-align:center\">" \
                          "\r\n"                                              \
                          "    <div style=\"margin:auto\">"                   \
                          "\r\n"                                              \
                          "      <h1>Hello World</h1>"                        \
                          "\r\n"                                              \
                          "      <p>It works !</p>"                           \
                          "\r\n"                                              \
                          "    </div>"                                        \
                          "\r\n"                                              \
                          "  </body>"                                         \
                          "\r\n"                                              \
                          "</html>"

#define HTTP_RESPONSE HTTP_STATUS_LINE "\r\n" HTTP_HEADER_FIELDS "\r\n" \
                                       "\r\n" HTTP_MESSAGE_BODY "\r\n"

DigitalOut led1(LED1);

// Vector3 feedbackLoop(const Quaternion &setpoint, const Quaternion &orientation, const Vector3 &velocity, const float pq, const float pw)
// {
//     Quaternion error = setpoint.difference(orientation);
//     return -pq * error.v - pw * velocity;
// }

void cb()
{
    led1 = !led1;
}

int main()
{
    printf("Basic HTTP server example\n");

    EthernetInterface eth;

    eth.disconnect();

    eth.set_network("192.168.1.191", "255.255.255.0", "192.168.1.1");

    // eth.attach(&cb);
    // eth.set_blocking(false);

    eth.connect();

    printf("The Server IP address is '%s'\n", eth.get_ip_address());
    TCPServer srv;
    TCPSocket client_sock;
    SocketAddress client_addr;
    char buffer[256];

    /* Open the server on ethernet stack */
    srv.open(&eth);

    /* Bind the HTTP port (TCP 80) to the server */
    srv.bind(eth.get_ip_address(), 23);

    /* Can handle x simultaneous connections */
    srv.listen(1);

    srv.accept(&client_sock, &client_addr);
    printf("Accepted %s:%d\n", client_addr.get_ip_address(),
           client_addr.get_port());
    strcpy(buffer, "Hello \n\r");
    client_sock.send(buffer, strlen(buffer));
    int ret;
    while (true)
    {
        ret = client_sock.recv(buffer, sizeof(buffer) - 1);
        if (ret <= 0)
            break;
        buffer[ret] = '\0';
        json data = buffer;
        printf("%s\n", data["ctrl"]);
        // printf("Received %d chars from server:\n%s\n", ret, buffer); //["ctrl"]);
    }
    // client_sock.close();
    // delete[] buffer;
    while (1)
    {
    }

    // printf("The target IP address is '%s'\n", eth.get_ip_address());

    // TCPServer srv;
    // TCPSocket clt_sock;
    // SocketAddress clt_addr;

    // /* Open the server on ethernet stack */
    // srv.open(&eth);

    // /* Bind the HTTP port (TCP 80) to the server */
    // srv.bind(eth.get_ip_address(), 80);

    // /* Can handle 5 simultaneous connections */
    // srv.listen(5);

    // while (true)
    // {
    //     srv.accept(&clt_sock, &clt_addr);
    //     printf("accept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
    //     clt_sock.send(HTTP_RESPONSE, strlen(HTTP_RESPONSE));
    // }
}