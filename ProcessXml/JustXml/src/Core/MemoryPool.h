/*
 * MemoryPool.h
 *
 *  Created on: Nov 17, 2019
 *      Author: LavishK1
 */

#ifndef SRC_CORE_MEMORYPOOL_H_
#define SRC_CORE_MEMORYPOOL_H_

// If standard library is disabled, user must provide implementations of required functions and typedefs
#if !defined(XPROC_NO_STDLIB)
#include <cstdlib>      // For std::size_t
#include <cassert>      // For assert
#endif

#include "Internal/ProcessFlags.h"

namespace xprocesser
{
namespace xcore
{

// Forward declarations
template<class ItemType> class xml_node;
template<class ItemType> class xml_attribute;

enum node_type
{
	node_document,      //!< A document node. Name and value are empty.
	node_element,       //!< An element node. Name contains element name. Value contains text of first data node.
	node_data,          //!< A data node. Name is empty. Value contains data text.
	node_cdata,         //!< A CDATA node. Name is empty. Value contains data text.
	node_comment,       //!< A comment node. Name is empty. Value contains comment text.
	node_declaration,   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
	node_doctype,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
	node_pi             //!< A PI node. Name contains target. Value contains instructions.
};

///////////////////////////////////////////////////////////////////////
// Memory pool

//! This class is used by the parser to create new nodes and attributes, without overheads of dynamic memory allocation.
//! In most cases, you will not need to use this class directly.
//! However, if you need to create nodes manually or modify names/values of nodes,
//! you are encouraged to use memory_pool of relevant xml_document to allocate the memory.
//! Not only is this faster than allocating them by using <code>new</code> operator,
//! but also their lifetime will be tied to the lifetime of document,
//! possibly simplyfing memory management.
//! <br><br>
//! Call allocate_node() or allocate_attribute() functions to obtain new nodes or attributes from the pool.
//! You can also call allocate_string() function to allocate strings.
//! Such strings can then be used as names or values of nodes without worrying about their lifetime.
//! Note that there is no <code>free()</code> function -- all allocations are freed at once when clear() function is called,
//! or when the pool is destroyed.
//! <br><br>
//! It is also possible to create a standalone memory_pool, and use it
//! to allocate nodes, whose lifetime will not be tied to any document.
//! <br><br>
//! Pool maintains <code>RAPIDXML_STATIC_POOL_SIZE</code> bytes of statically allocated memory.
//! Until static memory is exhausted, no dynamic memory allocations are done.
//! When static memory is exhausted, pool allocates additional blocks of memory of size <code>RAPIDXML_DYNAMIC_POOL_SIZE</code> each,
//! by using global <code>new[]</code> and <code>delete[]</code> operators.


//! This behaviour can be changed by setting custom allocation routines.
//! Use set_allocator() function to set them.
typedef void *(alloc_func)(std::size_t);       // Type of user-defined function used to allocate memory
typedef void (free_func)(void *);              // Type of user-defined function used to free memory


//! <br><br>
//! Allocations for nodes, attributes and strings are aligned at <code>RAPIDXML_ALIGNMENT</code> bytes.
//! This value defaults to the size of pointer on target architecture.
//! <br><br>
//! To obtain absolutely top performance from the parser,
//! it is important that all nodes are allocated from a single, contiguous block of memory.
//! Otherwise, cache misses when jumping between two (or more) disjoint blocks of memory can slow down parsing quite considerably.
//! If required, you can tweak <code>RAPIDXML_STATIC_POOL_SIZE</code>, <code>RAPIDXML_DYNAMIC_POOL_SIZE</code> and <code>RAPIDXML_ALIGNMENT</code>
//! to obtain best wasted memory to performance compromise.
//! To do it, define their values before rapidxml.hpp file is included.
//! \param ItemType Character type of created nodes.
template<class ItemType = char>
class memory_pool
{
public:
	//! Constructs empty pool with default allocator functions.
	memory_pool();

	//! Destroys pool and frees all the memory.
	//! This causes memory occupied by nodes allocated by the pool to be freed.
	//! Nodes allocated from the pool are no longer valid.
	~memory_pool();

	//! Allocates a new node from the pool, and optionally assigns name and value to it.
	//! If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
	//! If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
	//! will call rapidxml::parse_error_handler() function.
	//! \param type Type of node to create.
	//! \param name Name to assign to the node, or 0 to assign no name.
	//! \param value Value to assign to the node, or 0 to assign no value.
	//! \param name_size Size of name to assign, or 0 to automatically calculate size from name string.
	//! \param value_size Size of value to assign, or 0 to automatically calculate size from value string.
	//! \return Pointer to allocated node. This pointer will never be NULL.
	xml_node<ItemType> *allocate_node(node_type ,const ItemType * = 0, const ItemType * = 0, std::size_t  = 0, std::size_t  = 0);

	//! Allocates a new attribute from the pool, and optionally assigns name and value to it.
	//! If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
	//! If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
	//! will call rapidxml::parse_error_handler() function.
	//! \param name Name to assign to the attribute, or 0 to assign no name.
	//! \param value Value to assign to the attribute, or 0 to assign no value.
	//! \param name_size Size of name to assign, or 0 to automatically calculate size from name string.
	//! \param value_size Size of value to assign, or 0 to automatically calculate size from value string.
	//! \return Pointer to allocated attribute. This pointer will never be NULL.
	xml_attribute<ItemType> *allocate_attribute(const ItemType * = 0, const ItemType * = 0,
			std::size_t = 0, std::size_t = 0);

	//! Allocates a char array of given size from the pool, and optionally copies a given string to it.
	//! If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
	//! If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
	//! will call rapidxml::parse_error_handler() function.
	//! \param source String to initialize the allocated memory with, or 0 to not initialize it.
	//! \param size Number of characters to allocate, or zero to calculate it automatically from source string length; if size is 0, source string must be specified and null terminated.
	//! \return Pointer to allocated char array. This pointer will never be NULL.
	ItemType *allocate_string(const ItemType * = 0, std::size_t = 0);

	//! Clones an xml_node and its hierarchy of child nodes and attributes.
	//! Nodes and attributes are allocated from this memory pool.
	//! Names and values are not cloned, they are shared between the clone and the source.
	//! Result node can be optionally specified as a second parameter,
	//! in which case its contents will be replaced with cloned source node.
	//! This is useful when you want to clone entire document.
	//! \param source Node to clone.
	//! \param result Node to put results in, or 0 to automatically allocate result node
	//! \return Pointer to cloned node. This pointer will never be NULL.
	xml_node<ItemType> *clone_node(const xml_node<ItemType> *, xml_node<ItemType> * = 0);

	//! Clears the pool.
	//! This causes memory occupied by nodes allocated by the pool to be freed.
	//! Any nodes or strings allocated from the pool will no longer be valid.
	void clear();

	//! Sets or resets the user-defined memory allocation functions for the pool.
	//! This can only be called when no memory is allocated from the pool yet, otherwise results are undefined.
	//! Allocation function must not return invalid pointer on failure. It should either throw,
	//! stop the program, or use <code>longjmp()</code> function to pass control to other place of program.
	//! If it returns invalid pointer, results are undefined.
	//! <br><br>
	//! User defined allocation functions must have the following forms:
	//! <br><code>
	//! <br>void *allocate(std::size_t size);
	//! <br>void free(void *pointer);
	//! </code><br>
	//! \param af Allocation function, or 0 to restore default function
	//! \param ff Free function, or 0 to restore default function
	void set_allocator(alloc_func *, free_func *);

private:

	struct header
	{
		char *previous_begin;
	};

	void init();

	char *align(char *);

	char *allocate_raw(std::size_t);

	void *allocate_aligned(std::size_t);

	char *m_begin;                                      // Start of raw memory making up current pool
	char *m_ptr;                                        // First free byte in current pool
	char *m_end;                                        // One past last available byte in current pool
	char m_static_memory[RAPIDXML_STATIC_POOL_SIZE];    // Static raw memory
	alloc_func *m_alloc_func;                           // Allocator function, or 0 if default is to be used
	free_func *m_free_func;                             // Free function, or 0 if default is to be used
};


} /* namespace xcore */
} /* namespace xprocesser */

#endif /* SRC_CORE_MEMORYPOOL_H_ */
