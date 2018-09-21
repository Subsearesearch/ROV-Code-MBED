#include "mbed.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include "math.h"
#include "MbedJSONValue.h"
#include <string>
#include "Quaternion.h"

DigitalOut led1(LED1);

// Vector3 feedbackLoop(const Quaternion &setpoint, const Quaternion &orientation, const Vector3 &velocity, const float pq, const float pw)
// {
//   Quaternion error = setpoint.difference(orientation);
//   return -pq * error.v - pw * velocity;
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

  eth.set_network("192.168.1.61", "255.255.255.0", "192.168.1.1");

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
    // printf("%s\n", buffer);
    MbedJSONValue data;
    const char *json = &buffer[ret];
    // bool my_bool;
    // double x_lin;
    // double y_lin;
    // double z_lin;
    // double x_rot;
    // double y_rot;
    // double z_rot;
    //parse the previous string and fill the object data
    parse(data, json);
    double x_lin = data["ctrl"]["xLin"].get<double>();
    double y_lin = data["ctrl"]["yLin"].get<double>();
    double z_lin = data["ctrl"]["zLin"].get<double>();
    double x_rot = data["ctrl"]["xRot"].get<double>();
    double y_rot = data["ctrl"]["yRot"].get<double>();
    double z_rot = data["ctrl"]["zLin"].get<double>();
    printf("%4.2f\n", z_rot);
    // x_lin = data["ctrl"]["xLin"].get<double>();
    // y_lin = data["ctrl"]["yLin"].get<double>();
    // z_lin = data["ctrl"]["zLin"].get<double>();
    // x_rot = data["ctrl"]["xRot"].get<double>();
    // y_rot = data["ctrl"]["yRot"].get<double>();
    // z_rot = data["ctrl"]["zLin"].get<double>();
    int bl = 0;
    int br = 0;
    int fl = 0;
    int fr = 0;
    // z_lin (up and down)
    bl += z_lin;
    br += z_lin;
    fl += z_lin;
    fr += z_lin;
    // y_rot (tilt left (-) and right (+))
    fr -= y_rot;
    br -= y_rot;
    bl += y_rot;
    fl += y_rot;
    // x_rot (tilt up (-) and down (+))
    fr -= x_rot;
    fl -= x_rot;
    fr += x_rot;
    bl += x_rot;
    // make sure highest is 1
    int max;
    // max = Math.max(Math.abs(fr, fl, br, bl)) if (max > 1);
    max = fmax(fmax(abs(fr), abs(fl)), fmax(abs(br), abs(bl)));
    {
      fr /= max;
      fl /= max;
      br /= max;
      bl /= max;
    }
    /* console.log(fl);
    console.log(fr);
    console.log(bl);
    console.log(br); */
    int f = 0;
    int b = 0;
    int l = 0;
    int r = 0;

    // z_rot (+ is clockwise)
    f += z_rot;
    b -= z_rot;
    l += z_rot;
    r -= z_rot;
    // x_lin (side to side, + is right)
    f += x_lin;
    b += x_lin;
    // y_lin (front and back, + is forward)
    l += y_lin;
    r += y_lin;
    // make sure highest is 1
    // max = Math.max(Math.abs(f, b, r, l));
    max = fmax(fmax(abs(f), abs(b)), fmax(abs(r), abs(l)));

    printf("%i\n", f);
    if (max > 1)
    {
      f /= max;
      b /= max;
      r /= max;
      l /= max;
    }

    // Convert to seconds (for pwm)
    f = 0.004 * f + 0.0015;
    b = 0.004 * b + 0.0015;
    l = 0.004 * l + 0.0015;
    r = 0.004 * r + 0.0015;
    fr = 0.004 * fr + 0.0015;
    fl = 0.004 * fl + 0.0015;
    br = 0.004 * br + 0.0015;
    bl = 0.004 * bl + 0.0015;
    // printf("my_bool: %s\r\n", my_bool ? "true" : "false");
    // printf("Received %d chars from server:\n%s\n", ret, buffer); //["ctrl"]);
  }
  // client_sock.close();
  // delete[] buffer;
  while (1)
  {
  }
}