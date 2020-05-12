/*
 * unimon.{cc,hh} -- monitoring clickos routers
 * Will Fantom
 */
#include <click/config.h>
#include <click/unimon.hh>
#include <click/glue.hh>

Unimon::Unimon(Router *r)
{
  this->_parent_router = r;

  char version_path[XS_PATH_MAX_LEN];
  snprintf(version_path, XS_PATH_MAX_LEN, "%s/version", XS_ROOT_PATH);
  xenbus_write(XBT_NIL, version_path, this->version().c_str());
  write_status((char *)"present");
  this->_control_thread = create_thread((char *)"unimon", this->control_thread, NULL);
  schedule();
}

Unimon::~Unimon()
{
  
}

bool
Unimon::register_element(Element *e)
{
  this->_registered.push_back(e);
  return true;
}

void
Unimon::export_data(umdata_t *data, mechanism ex)
{
  switch(ex) {
    case 0: break;
    case 1: 
      char data_path[XS_PATH_MAX_LEN];
      snprintf(data_path, XS_PATH_MAX_LEN, "%s/data/%s/%d", XS_ROOT_PATH, data->elem_name, data->export_time);
      uint64_t value = this->average_data(data->data_arr, data->data_count);
      this->xs_write_data(data_path, value);
      break;
  };
}

void 
Unimon::control_thread(void *data) 
{
  struct xs_dev *_xsdev = (struct xs_dev*) calloc(1, sizeof(struct xs_dev));
  char control_path[XS_PATH_MAX_LEN];
  snprintf(control_path, XS_PATH_MAX_LEN, "%s/control", XS_ROOT_PATH);
  xenbus_watch_path_token(XBT_NIL, control_path, XS_UNIMON_TOKEN, &_xsdev->events);
  for(;;) {
    char **path;
    path = xenbus_wait_for_watch_return(&_xsdev->events);
    if (!path)
      continue;
  };

}

void
Unimon::write_status(char *status)
{
  char status_path[XS_PATH_MAX_LEN];
  snprintf(status_path, XS_PATH_MAX_LEN, "%s/status", XS_ROOT_PATH);
  xenbus_write(XBT_NIL, status_path, status);
}

void 
Unimon::xs_write_data(char *path, uint64_t data)
{
  char str_data[MAX_ELEM_NAME_LEN];
  snprintf(str_data, MAX_ELEM_NAME_LEN, "%d", data);
  xenbus_write(XBT_NIL, path, str_data);
}

uint64_t
Unimon::average_data(uint64_t *data, uint64_t count)
{
  uint64_t sum = 0;
  for (uint64_t c = 0 ; c<count ; c++) {
    sum += data[c];
  };
  //todo: check for div by 0 
  return (sum/count);
}