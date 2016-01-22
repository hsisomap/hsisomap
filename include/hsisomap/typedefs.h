//
// Created by Can on 3/14/15.
//

#ifndef HSISOMAP_TYPEDEFS_H
#define HSISOMAP_TYPEDEFS_H

#include <stddef.h>
#include <unordered_map>


#define HSISOMAP_NAMESPACE_BEGIN namespace hsisomap {
#define HSISOMAP_NAMESPACE_END }


#ifndef TYPEDEFS_INDEX
#define TYPEDEFS_INDEX size_t
#endif

#ifndef TYPEDEFS_SCALAR
#define TYPEDEFS_SCALAR double
#endif

#ifndef TYPEDEFS_CL_INDEX
#define TYPEDEFS_CL_INDEX int
#endif

#ifndef TYPEDEFS_CL_SCALAR
#define TYPEDEFS_CL_SCALAR float
#endif

typedef size_t Index;
typedef const size_t kIndex;
typedef double Scalar;
typedef const Scalar kScalar;
typedef const std::string Key;

typedef std::unordered_map<std::string, std::string> StringPropertyList;
typedef std::unordered_map<std::string, Scalar> PropertyList;

#endif //HSISOMAP_TYPEDEFS_H
