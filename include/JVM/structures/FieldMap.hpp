#ifndef _FIELDMAP_H_
#define _FIELDMAP_H_

#include <DotClassReader/MethodInfo.hpp>
#include <JVM/structures/ContextEntry.hpp>
#include <map>
#include <string>

typedef std::map<std::string, std::shared_ptr<ContextEntry>> ClassFields;
typedef std::map<std::string, MethodInfoCte> ClassMethods;
#endif