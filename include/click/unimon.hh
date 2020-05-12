#ifndef CLICK_UNIMON_HH
#define CLICK_UNIMON_HH

#include <click/element.hh>

#if CLICK_MINIOS
extern "C"{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mini-os/xenbus.h>
}

# define XS_ROOT_PATH "data/clickos/unimon"
# define XS_PATH_MAX_LEN 1024
# define XS_UNIMON_TOKEN "unimon-watch"
# define XS_UNIMON_DID_TRUST 0
#endif
#define MAX_ELEM_NAME_LEN 128

CLICK_DECLS

class Router;

class Unimon;

struct xs_dev {
	domid_t dom;
	char *nodename;
	xenbus_event_queue events;
};

typedef struct umdata {
  uint64_t export_time;
  uint64_t data_size;
  char elem_name[MAX_ELEM_NAME_LEN];
  void *data;
} umdata_t;

class Unimon { public:

  enum mechanism {
    print = 0,
    xenstore = 1
  };

  Unimon(Router *r);
  ~Unimon();

  String version() const { return "v0.1"; }

  bool register_element(Element *e);

  void export_data(umdata_t *data, mechanism ex);

private:

  Router *_parent_router;
  Vector<Element *> _registered;
  struct xs_dev *_xsdev;

  void write_status(char *status);

};

CLICK_ENDDECLS
#endif