// Copyright 2004-present Facebook. All Rights Reserved.

#include <osquery/core.h>
#include <osquery/logger.h>
#include <osquery/tables.h>

#include "osquery/events/darwin/iokit_hid.h"

namespace osquery {
namespace tables {

/**
 * @brief Track IOKit HID events.
 */
class HardwareEventSubscriber : public EventSubscriber {
  DECLARE_EVENTSUBSCRIBER(HardwareEventSubscriber, IOKitHIDEventPublisher);
  DECLARE_CALLBACK(Callback, IOKitHIDEventContext);

 public:
  void init();

  Status Callback(const IOKitHIDEventContextRef ec);
};

REGISTER_EVENTSUBSCRIBER(HardwareEventSubscriber);

void HardwareEventSubscriber::init() {
  auto subscription = IOKitHIDEventPublisher::createSubscriptionContext();
  // We don't want hardware value changes.
  subscription->values = false;

  BIND_CALLBACK(Callback, subscription);
}

Status HardwareEventSubscriber::Callback(const IOKitHIDEventContextRef ec) {
  Row r;

  r["action"] = ec->action;
  // There is no path in IOKit, there's a location ID (may be useful).
  r["path"] = ec->location;

  // Type and driver are the name in IOKit
  r["type"] = "hid";
  r["driver"] = ec->transport;

  r["model_id"] = ec->model_id;
  r["model"] = ec->model;
  r["vendor_id"] = ec->vendor_id;
  r["vendor"] = ec->vendor;
  r["serial"] = ec->serial; // Not always filled in.
  r["revision"] = ec->version;

  r["time"] = INTEGER(ec->time);
  add(r, ec->time);
  return Status(0, "OK");
}
}
}
