#include "ModuleId.h"

ModuleId::ModuleId() : UniqueId<ModuleId>() { }
ModuleId::ModuleId(const std::string& id) : UniqueId<ModuleId>(id) { }

