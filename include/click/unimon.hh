#ifndef CLICK_UNIMON_HH
#define CLICK_UNIMON_HH

#include <click/element.hh>

#if CLICK_MINIOS
extern "C"{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mini-os/xenbus.h>
#include <mini-os/shutdown.h>
#include <mini-os/sched.h>
}
#endif

CLICK_DECLS

class Unimon;

class Unimon { public:

  Unimon();

};

CLICK_ENDDECLS
#endif