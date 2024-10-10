/*
 * CoreAlgorithms.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#include "CoreAlgorithms.h"

namespace xprocesser
{
namespace xcore
{
namespace xinternal
{

template<class ItemType>
inline std::size_t measure(const ItemType *p)
{
	const ItemType *tmp = p;
	while (*tmp) ++tmp;
	return tmp - p;
}

template<class ItemType>
inline bool compare(const ItemType *first, std::size_t size1, const ItemType *second, std::size_t size2, bool case_sensitive)
{
	if (size1 != size2)
		return false;
	if (case_sensitive)
	{
		for (const ItemType *end = first + size1; first < end; ++first, ++second)
			if (*first != *second)
				return false;
	}
	else
	{
		for (const ItemType *end = first + size1; first < end; ++first, ++second)
			if (lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*first)] != lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*second)])
				return false;
	}
	return true;
}

} /* namespace xinternal */
} /* namespace xcore */
} /* namespace xprocesser */
