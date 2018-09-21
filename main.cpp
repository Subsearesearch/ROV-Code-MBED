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
    MbedJSONValue data;
    // printf("%s\n", buffer);
    const char *json = &buffer[ret];
    parse(data, buffer);
    // Values are between -100 and 100
    int x_lin = data["ctrl"]["xLin"].get<int>();
    int y_lin = data["ctrl"]["yLin"].get<int>();
    int z_lin = data["ctrl"]["zLin"].get<int>();
    int x_rot = data["ctrl"]["xRot"].get<int>();
    int y_rot = data["ctrl"]["yRot"].get<int>();
    int z_rot = data["ctrl"]["zRot"].get<int>();
    // printf("%d\n", z_rot);
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
    // make sure highest is 100
    int max;
    max = fmax(fmax(abs(fr), abs(fl)), fmax(abs(br), abs(bl)));
    if (max > 100)
    {
      fr /= max;
      fl /= max;
      br /= max;
      bl /= max;
    }
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
    // make sure highest is 100
    max = fmax(fmax(abs(f), abs(b)), fmax(abs(r), abs(l)));

    if (max > 100)
    {
      f /= max;
      b /= max;
      r /= max;
      l /= max;
    }

    // Convert to micro seconds (for pwm)
    f = 4 * f + 1500;
    b = 4 * b + 1500;
    l = 4 * l + 1500;
    r = 4 * r + 1500;
    fr = 4 * fr + 1500;
    fl = 4 * fl + 1500;
    br = 4 * br + 1500;
    bl = 4 * bl + 1500;
    // printf("%d\n", f);
    // printf("%d\n", r);
    // pulsewidth_us(f)
  }
  // client_sock.close();
  // delete[] buffer;
  while (1)
  {
  }
}