/*
 * unimon.{cc,hh} -- monitoring clickos routers
 * Will Fantom
 */
#include <click/config.h>
#include <click/unimon.hh>

Unimon::Unimon(Router *r)
{
  this->_parent_router = r;

  this->_xsdev = (struct xs_dev*) malloc(sizeof(struct xs_dev));
  memset(this->_xsdev, 0, sizeof(struct xs_dev));
  char status_path[XS_PATH_MAX_LEN];
  snprintf(status_path, XS_PATH_MAX_LEN, "%s/status/%s", XS_ROOT_PATH, "isactive");
  xenbus_write(XBT_NIL, status_path, "active");
  char version_path[XS_PATH_MAX_LEN];
  snprintf(version_path, XS_PATH_MAX_LEN, "%s/status/%s", XS_ROOT_PATH, "version");
  xenbus_write(XBT_NIL, status_path, this->version().c_str());
}

Unimon::~Unimon()
{
  free(this->_xsdev);
}

bool
Unimon::register_element(Element *e)
{
  return true;
}

void
Unimon::export_data(umdata_t *data)
{
  int a = 1+2;
}