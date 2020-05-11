/*
 * unimon.{cc,hh} -- monitoring clickos routers
 * Will Fantom
 */
#include <click/config.h>
#include <click/unimon.hh>

Unimon::Unimon(Router *r) {
  this->_parent_router = r;
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