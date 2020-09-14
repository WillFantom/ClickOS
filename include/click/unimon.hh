#ifndef CLICK_UNIMON_HH
#define CLICK_UNIMON_HH

#include <click/element.hh>

#if CLICK_MINIOS
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mini-os/xenbus.h>
#include <mini-os/sched.h>
}

#endif

CLICK_DECLS

#if CLICK_MINIOS
struct xs_dev 
{
  domid_t dom;
  char *nodename;
  xenbus_event_queue events;
};

#define XS_ROOT_PATH "data/clickos/unimon"
#define XS_PATH_MAX_LEN 1024
#define XS_CONTROL_PATH "control"
#define XS_UNIMON_TOKEN "unimon-watch"
#define XS_UNIMON_DID_TRUST 0
#endif

class Unimon
{
public:

  enum mechanism
  {
    print = 0,
    xenstore = 1
  };
  static const uint8_t mechanism_count = 2;

  enum um_state
  {
    pending = 0,
    error = 1,
    inactive = 2,
    active = 3
  };


  Unimon();
  ~Unimon();

  String version() const { return "v0.0.1"; }

  bool register_element(Element *e);

  bool write_status(char *elem_name, um_state state, char *message);
  bool write_data(char *elem_name, mechanism mech, char *data);

private:
  Vector<Element *> _registered;
  
  // XenStore
  struct thread *_xs_event_thread;
  bool setup_xenstore();
  bool write_to_xenstore(char *path, const char *data);
  static void xs_event_check(void *data);

};

CLICK_ENDDECLS
#endif