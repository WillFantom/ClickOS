/*
 * unimon.{cc,hh} -- monitoring clickos routers
 * Will Fantom
 */
#include <click/config.h>
#include <click/unimon.hh>
#include <click/straccum.hh>

Unimon::Unimon(Router *r)
{
  this->_parent_router = r;

  this->_xsdev = (struct xs_dev*) calloc(1, sizeof(struct xs_dev));
  char version_path[XS_PATH_MAX_LEN];
  snprintf(version_path, XS_PATH_MAX_LEN, "%s/version", XS_ROOT_PATH);
  xenbus_write(XBT_NIL, version_path, this->version().c_str());
  write_status((char *)"present");
}

Unimon::~Unimon()
{
  free(this->_xsdev);
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
      break;
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
write_data_xs(char* path, char *data)
{
  
}

void 
write_data_xs(char *path, uint64_t data)
{
  char str_data[MAX_ELEM_NAME_LEN];
  snprintf(str_data, MAX_ELEM_NAME_LEN, "%d", data);
  write_data_xs(path, str_data);
}