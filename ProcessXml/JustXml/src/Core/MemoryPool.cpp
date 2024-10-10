/*
 * MemoryPool.cpp
 *
 *  Created on: Nov 17, 2019
 *      Author: LavishK1
 */

#include "MemoryPool.h"

#include "Internal/CoreAlgorithms.h"

namespace xprocesser
{
namespace xcore
{

template<typename ItemType>
memory_pool<ItemType>::memory_pool()
: m_alloc_func(0)
  , m_free_func(0)
  {
	init();
  }

template<typename ItemType>
memory_pool<ItemType>::~memory_pool()
{
	clear();
}

template<typename ItemType>
xml_node<ItemType> *memory_pool<ItemType>::allocate_node(node_type type, const ItemType *name, const ItemType *value, std::size_t name_size, std::size_t value_size)
{
	void *memory = allocate_aligned(sizeof(xml_node<ItemType>));
	xml_node<ItemType> *node = new(memory) xml_node<ItemType>(type);
	if (name)
	{
		if (name_size > 0)
			node->name(name, name_size);
		else
			node->name(name);
	}
	if (value)
	{
		if (value_size > 0)
			node->value(value, value_size);
		else
			node->value(value);
	}
	return node;
}

template<typename ItemType>
xml_attribute<ItemType> *memory_pool<ItemType>::allocate_attribute(const ItemType *name, const ItemType *value, std::size_t name_size, std::size_t value_size)
{
	void *memory = allocate_aligned(sizeof(xml_attribute<ItemType>));
	xml_attribute<ItemType> *attribute = new(memory) xml_attribute<ItemType>;
	if (name)
	{
		if (name_size > 0)
			attribute->name(name, name_size);
		else
			attribute->name(name);
	}
	if (value)
	{
		if (value_size > 0)
			attribute->value(value, value_size);
		else
			attribute->value(value);
	}
	return attribute;
}

template<typename ItemType>
ItemType *memory_pool<ItemType>::allocate_string(const ItemType *source , std::size_t size )
{
	assert(source || size);     // Either source or size (or both) must be specified
	if (size == 0)
		size = xinternal::measure(source) + 1;
	ItemType *result = static_cast<ItemType *>(allocate_aligned(size * sizeof(ItemType)));
	if (source)
		for (std::size_t i = 0; i < size; ++i)
			result[i] = source[i];
	return result;
}

template<typename ItemType>
xml_node<ItemType> *memory_pool<ItemType>::clone_node(const xml_node<ItemType> *source, xml_node<ItemType> *result)
{
	// Prepare result node
	if (result)
	{
		result->remove_all_attributes();
		result->remove_all_nodes();
		result->type(source->type());
	}
	else
		result = allocate_node(source->type());

	// Clone name and value
	result->name(source->name(), source->name_size());
	result->value(source->value(), source->value_size());

	// Clone child nodes and attributes
	for (xml_node<ItemType> *child = source->first_node(); child; child = child->next_sibling())
		result->append_node(clone_node(child));
	for (xml_attribute<ItemType> *attr = source->first_attribute(); attr; attr = attr->next_attribute())
		result->append_attribute(allocate_attribute(attr->name(), attr->value(), attr->name_size(), attr->value_size()));

	return result;
}

template<typename ItemType>
void memory_pool<ItemType>::clear()
{
	while (m_begin != m_static_memory)
	{
		char *previous_begin = reinterpret_cast<header *>(align(m_begin))->previous_begin;
		if (m_free_func)
			m_free_func(m_begin);
		else
			delete[] m_begin;
		m_begin = previous_begin;
	}
	init();
}

template<typename ItemType>
void memory_pool<ItemType>::set_allocator(alloc_func *af, free_func *ff)
{
	assert(m_begin == m_static_memory && m_ptr == align(m_begin));    // Verify that no memory is allocated yet
	m_alloc_func = af;
	m_free_func = ff;
}

template<typename ItemType>
void memory_pool<ItemType>::init()
{
	m_begin = m_static_memory;
	m_ptr = align(m_begin);
	m_end = m_static_memory + sizeof(m_static_memory);
}

template<typename ItemType>
char *memory_pool<ItemType>::align(char *ptr)
{
	std::size_t alignment = ((RAPIDXML_ALIGNMENT - (std::size_t(ptr) & (RAPIDXML_ALIGNMENT - 1))) & (RAPIDXML_ALIGNMENT - 1));
	return ptr + alignment;
}

template<typename ItemType>
char *memory_pool<ItemType>::allocate_raw(std::size_t size)
{
	// Allocate
	void *memory;
	if (m_alloc_func)   // Allocate memory using either user-specified allocation function or global operator new[]
	{
		memory = m_alloc_func(size);
		assert(memory); // Allocator is not allowed to return 0, on failure it must either throw, stop the program or use longjmp
	}
	else
	{
		memory = new char[size];
#ifdef RAPIDXML_NO_EXCEPTIONS
		if (!memory)            // If exceptions are disabled, verify memory allocation, because new will not be able to throw bad_alloc
			RAPIDXML_PARSE_ERROR("out of memory", 0);
#endif
	}
	return static_cast<char *>(memory);
}

template<typename ItemType>
void *memory_pool<ItemType>::allocate_aligned(std::size_t size)
{
	// Calculate aligned pointer
	char *result = align(m_ptr);

	// If not enough memory left in current pool, allocate a new pool
	if (result + size > m_end)
	{
		// Calculate required pool size (may be bigger than RAPIDXML_DYNAMIC_POOL_SIZE)
		std::size_t pool_size = RAPIDXML_DYNAMIC_POOL_SIZE;
		if (pool_size < size)
			pool_size = size;

		// Allocate
		std::size_t alloc_size = sizeof(header) + (2 * RAPIDXML_ALIGNMENT - 2) + pool_size;     // 2 alignments required in worst case: one for header, one for actual allocation
		char *raw_memory = allocate_raw(alloc_size);

		// Setup new pool in allocated memory
		char *pool = align(raw_memory);
		header *new_header = reinterpret_cast<header *>(pool);
		new_header->previous_begin = m_begin;
		m_begin = raw_memory;
		m_ptr = pool + sizeof(header);
		m_end = raw_memory + alloc_size;

		// Calculate aligned pointer again using new pool
		result = align(m_ptr);
	}

	// Update pool and return aligned pointer
	m_ptr = result + size;
	return result;
}


} /* namespace xcore */
} /* namespace xprocesser */
