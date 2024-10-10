/*
 * LookupTables.h
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#ifndef SRC_CORE_INTERNAL_LOOKUPTABLES_H_
#define SRC_CORE_INTERNAL_LOOKUPTABLES_H_

namespace xprocesser
{
namespace xcore
{
namespace xinternal
{

// Class that contains lookup tables for the parser
// It must be a template to allow correct linking (because it has static data members, which are defined in a header file).
template<int Dummy>
class lookup_tables
{
public:
	static const unsigned char lookup_whitespace[256];              // Whitespace table
	static const unsigned char lookup_node_name[256];               // Node name table
	static const unsigned char lookup_text[256];                    // Text table
	static const unsigned char lookup_text_pure_no_ws[256];         // Text table
	static const unsigned char lookup_text_pure_with_ws[256];       // Text table
	static const unsigned char lookup_attribute_name[256];          // Attribute name table
	static const unsigned char lookup_attribute_data_1[256];        // Attribute data table with single quote
	static const unsigned char lookup_attribute_data_1_pure[256];   // Attribute data table with single quote
	static const unsigned char lookup_attribute_data_2[256];        // Attribute data table with double quotes
	static const unsigned char lookup_attribute_data_2_pure[256];   // Attribute data table with double quotes
	static const unsigned char lookup_digits[256];                  // Digits
	static const unsigned char lookup_upcase[256];                  // To uppercase conversion table for ASCII characters

};

} /* namespace xinternal */
} /* namespace xcore */
} /* namespace xprocesser */

#endif /* SRC_CORE_INTERNAL_LOOKUPTABLES_H_ */
