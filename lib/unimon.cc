/*
 * unimon.{cc,hh} -- monitoring clickos routers
 * Will Fantom
 */
#include <click/config.h>
#include <click/unimon.hh>
#include <click/glue.hh>
#include <click/timestamp.hh>
#include <click/router.hh>

Unimon::Unimon()
{
  printf("Unimon: Enabled\n");

  if(this->setup_xenstore()) {
    click_chatter("Unimon: xenstore communication avaliable");
  }

  char *version_string = new char();
  sprintf(version_string, "Unimon Version: %s", this->version().c_str());
  click_chatter(version_string);
}

Unimon::~Unimon()
{
}

bool Unimon::register_element(Element *e)
{
  // TODO: check if an element by the same name exits
  _registered.push_back(e);
  return true;
}

bool Unimon::write_data(char *elem_name, mechanism mech, char *data) 
{
  // currently only supports exporting uint64_t
  //uint64_t *d = reinterpret_cast<uint64_t *>(data);
  String t = (Timestamp::now()).unparse();
  switch(mech) {
    case 0:
      click_chatter("Unimon Data | %s | %s | %d", elem_name, t.c_str(), data);
      return true;
    case 1:
#if CLICK_MINIOS
      xenbus_transaction_t xbt;
      xenbus_transaction_start(&xbt);
      char *path = new char();
      snprintf(path, XS_PATH_MAX_LEN, "%s/elements/%s/%s", XS_ROOT_PATH, elem_name, t.c_str());
      //snprintf(path, XS_PATH_MAX_LEN, "%s/elements/%s", XS_ROOT_PATH, elem_name);
      xenbus_write(xbt, path, data);
      xenbus_transaction_end(xbt, 0, NULL);
      return true;
#endif
      return false;
  }
  return false;
}


// XenStore
bool Unimon::setup_xenstore() 
{
#if CLICK_MINIOS
  char *path = new char();
  snprintf(path, XS_PATH_MAX_LEN, "%s/version", XS_ROOT_PATH);
  xenbus_write(XBT_NIL, path, this->version().c_str());
  this->_xs_event_thread = create_thread((char *)"unimon-xs", this->xs_event_check, NULL);
  schedule();
  return true;
#endif
  return false;
}

void Unimon::xs_event_check(void *data)
{
#if CLICK_MINIOS
  struct xs_dev *_xsdev = (struct xs_dev *)calloc(1, sizeof(struct xs_dev));
  char control_path[XS_PATH_MAX_LEN];
  snprintf(control_path, XS_PATH_MAX_LEN, "%s/control", XS_ROOT_PATH);  // data/clickos/unimon/control
  xenbus_watch_path_token(XBT_NIL, control_path, XS_UNIMON_TOKEN, &_xsdev->events);
  for (;;)
  {
    char **path;
    path = xenbus_wait_for_watch_return(&_xsdev->events);
    if (!path)
      continue;
    printf("Unimon XenStore: Event Triggered\n");
    char *path_ext = (*path + strlen(control_path) + 1);
    if (!strchr(path_ext, '/'))
      continue;
    int sep = strchr(path_ext, '/') - path_ext;
    String element(path_ext, sep);
    String action(path_ext + sep + 1);
    printf("Unimon: Control Triggered for [Element, Action] : [%s, %s]\n", element.c_str(), action.c_str());
    if (action.lower().equals("pollrate")) {
      printf("mod poll rate for %s\n", element.c_str());
      String t = (Timestamp::now()).unparse();
      printf("time: %s\n", t.c_str());
    }
  };
#endif
}