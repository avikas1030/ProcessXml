/*
 * CoreAlgorithms.h
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#ifndef SRC_CORE_INTERNAL_COREALGORITHMS_H_
#define SRC_CORE_INTERNAL_COREALGORITHMS_H_

#include "LookupTables.h"

// If standard library is disabled, user must provide implementations of required functions and typedefs
#if !defined(XPROC_NO_STDLIB)
#include <cstdlib>      // For std::size_t
#endif

namespace xprocesser
{
namespace xcore
{
namespace xinternal
{

// Find length of the string
template<class ItemType>
inline std::size_t measure(const ItemType *);

// Compare strings for equality
template<class ItemType>
inline bool compare(const ItemType *, std::size_t, const ItemType *, std::size_t, bool);

} /* namespace xinternal */
} /* namespace xcore */
} /* namespace xprocesser */

#endif /* SRC_CORE_INTERNAL_COREALGORITHMS_H_ */
