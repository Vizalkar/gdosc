/* osc_client.h */
#include "scene/3d/spatial.h"
#include "scene/3d/position_3d.h"
#include "scene/main/node.h"
#include "node_path.h"
#include "print_string.h"
#include <cstdlib>
#include <iostream>
#include <memory>

#include "oscSender.h"

#ifndef GDOSC_H
#define GDOSC_H

class OSCclient : public Spatial {
  GDCLASS(OSCclient, Spatial);

 protected:
  static void _bind_methods();

 public:
  OSCclient();
  void testSend();

 private:
  gdOscSender* osc_sender;
  gdOscMessage msg;
};

#endif
