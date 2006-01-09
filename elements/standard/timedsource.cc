/*
 * timedsink.{cc,hh} -- element creates packets, pushes them periodically
 * Eddie Kohler
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include "timedsource.hh"
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/router.hh>
CLICK_DECLS

TimedSource::TimedSource()
  : _packet(0), _timer(this)
{
}

TimedSource::~TimedSource()
{
}

int
TimedSource::configure(Vector<String> &conf, ErrorHandler *errh)
{
  String data = "Random bullshit in a packet, at least 64 bytes long. Well, now it is.";
  int limit = -1;
  int interval = 500;
  bool active = true, stop = false;

  if (cp_va_parse(conf, this, errh,
		  cpOptional,
		  cpSecondsAsMilli, "packet generation interval", &interval,
		  cpString, "packet data", &data,
		  cpKeywords,
		  "DATA", cpString, "packet data", &data,
		  "INTERVAL", cpSecondsAsMilli, "packet generation interval", &interval,
		  "LIMIT", cpInteger, "total packet count", &limit,
		  "ACTIVE", cpBool, "active?", &active,
		  "STOP", cpBool, "stop driver when done?", &stop,
		  cpEnd) < 0)
    return -1;

  _data = data;
  _interval = interval;
  _limit = limit;
  _count = 0;
  _active = active;
  _stop = stop;
  if (_packet) _packet->kill();
  _packet = Packet::make(_data.data(), _data.length());
  return 0;
}

int
TimedSource::initialize(ErrorHandler *)
{
  _timer.initialize(this);
  if (_active)
    _timer.schedule_after_msec(_interval);
  return 0;
}

void
TimedSource::cleanup(CleanupStage)
{
  if (_packet)
    _packet->kill();
  _packet = 0;
}

void
TimedSource::run_timer(Timer *)
{
    if (!_active)
	return;
    if (_limit < 0 || _count < _limit) {
	Packet *p = _packet->clone();
	p->timestamp_anno().set_now();
	output(0).push(p);
	_count++;
	_timer.reschedule_after_msec(_interval);
    } else if (_stop)
	router()->please_stop_driver();
}

String
TimedSource::read_param(Element *e, void *vparam)
{
  TimedSource *ts = (TimedSource *)e;
  switch ((int)vparam) {
   case 0:			// data
    return ts->_data;
   case 1:			// limit
    return String(ts->_limit);
   case 2:			// interval
    return cp_unparse_milliseconds(ts->_interval);
   case 3:			// active
    return cp_unparse_bool(ts->_active);
   case 4:			// count
    return String(ts->_count);
   default:
    return "";
  }
}

int
TimedSource::change_param(const String &in_s, Element *e, void *vparam,
			  ErrorHandler *errh)
{
  TimedSource *ts = (TimedSource *)e;
  String s = cp_uncomment(in_s);
  switch ((int)vparam) {

  case 0:			// data
      ts->_data = in_s;
      if (ts->_packet)
	  ts->_packet->kill();
      ts->_packet = Packet::make(ts->_data.data(), ts->_data.length());
      break;
   
   case 1: {			// limit
     int limit;
     if (!cp_integer(s, &limit))
       return errh->error("limit parameter must be integer");
     ts->_limit = limit;
     break;
   }
   
   case 2: {			// interval
     uint32_t interval;
     if (!cp_seconds_as_milli(s, &interval) || interval < 1)
       return errh->error("interval parameter must be integer >= 1");
     ts->_interval = interval;
     break;
   }
   
   case 3: {			// active
     bool active;
     if (!cp_bool(s, &active))
       return errh->error("active parameter must be boolean");
     ts->_active = active;
     if (!ts->_timer.scheduled() && active)
       ts->_timer.schedule_now();
     break;
   }

   case 5: {			// reset
     ts->_count = 0;
     if (!ts->_timer.scheduled() && ts->_active)
       ts->_timer.schedule_now();
     break;
   }

  }
  return 0;
}

void
TimedSource::add_handlers()
{
  add_read_handler("data", read_param, (void *)0);
  add_write_handler("data", change_param, (void *)0);
  set_handler_flags("data", Handler::RAW);
  add_read_handler("limit", read_param, (void *)1);
  add_write_handler("limit", change_param, (void *)1);
  add_read_handler("interval", read_param, (void *)2);
  add_write_handler("interval", change_param, (void *)2);
  add_read_handler("active", read_param, (void *)3);
  add_write_handler("active", change_param, (void *)3);
  add_read_handler("count", read_param, (void *)4);
  add_write_handler("reset", change_param, (void *)5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TimedSource)
ELEMENT_MT_SAFE(TimedSource)
