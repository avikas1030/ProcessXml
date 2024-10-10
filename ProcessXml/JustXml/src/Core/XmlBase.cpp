/*
 * XmlBase.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#include "XmlBase.h"

#include "internal/CoreAlgorithms.h"

namespace xprocesser
{
namespace xcore
{

template<typename ItemType>
Xml_Base<ItemType>::Xml_Base()
: m_name(0)
  , m_value(0)
  , m_parent(0){}


template<typename ItemType>
ItemType* Xml_Base<ItemType>::name() const
{
	return m_name ? m_name : nullstr();
}

template<typename ItemType>
std::size_t Xml_Base<ItemType>::name_size() const
{
	return m_name ? m_name_size : 0;
}

template<typename ItemType>
ItemType* Xml_Base<ItemType>::value() const
{
	return m_value ? m_value : nullstr();
}

template<typename ItemType>
std::size_t Xml_Base<ItemType>::value_size() const
{
	return m_value ? m_value_size : 0;
}

template<typename ItemType>
void Xml_Base<ItemType>::name(const ItemType* name, std::size_t size)
{
	m_name = const_cast<ItemType*>(name);
	m_name_size = size;
}

template<typename ItemType>
void Xml_Base<ItemType>::name(const ItemType* name)
{
	this->name(name, xinternal::measure(name));
}

template<typename ItemType>
void Xml_Base<ItemType>::value(const ItemType* value, std::size_t size)
{
	m_value = const_cast<ItemType*>(value);
	m_value_size = size;
}

template<typename ItemType>
void Xml_Base<ItemType>::value(const ItemType* value)
{
	this->value(value, xinternal::measure(value));
}

template<typename ItemType>
xml_node<ItemType>* Xml_Base<ItemType>::parent() const
{
	return m_parent;
}

template<typename ItemType>
ItemType* Xml_Base<ItemType>::nullstr()
{
	static ItemType zero = ItemType('\0');
	return &zero;
}


} /* namespace xcore */
} /* namespace xprocesser */
