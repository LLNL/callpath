#ifndef MODULE_ID_H
#define MODULE_ID_H

#include "UniqueId.h"

class ModuleId : public UniqueId<ModuleId> {
public:
  ModuleId();
  ModuleId(const std::string& id);
};

#endif // MODULE_ID_H
