/***************************************************************************                                                                                     
*   Copyright 2012 Advanced Micro Devices, Inc.                                     
*                                                                                    
*   Licensed under the Apache License, Version 2.0 (the "License");   
*   you may not use this file except in compliance with the License.                 
*   You may obtain a copy of the License at                                          
*                                                                                    
*       http://www.apache.org/licenses/LICENSE-2.0                      
*                                                                                    
*   Unless required by applicable law or agreed to in writing, software              
*   distributed under the License is distributed on an "AS IS" BASIS,              
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         
*   See the License for the specific language governing permissions and              
*   limitations under the License.                                                   

***************************************************************************/                                                                                     

#if !defined( TRANSFORM_SCAN_H )
#define TRANSFORM_SCAN_H
#pragma once

#include <bolt/cl/bolt.h>
#include <bolt/cl/functional.h>
#include <bolt/cl/device_vector.h>

namespace bolt
{
    namespace cl
    {
      // TODO documentation needs to be updated

        /*! \addtogroup algorithms
         */

        /*! \addtogroup PrefixSums Prefix Sums
        *   \ingroup algorithms
        *   The sorting Algorithm for sorting the given InputIterator.
        */ 

        /*! \addtogroup transform_scan
        *   \ingroup PrefixSums
        *   \{
        *   \todo The user_code parameter is not used yet.
        */

        /*! \brief transform_inclusive_scan calculates a running sum over a range of values, inclusive of the current value.
        *   The result value at iterator position \p i is the running sum of all values less than \p i in the input range.
        *
        * \param first The first iterator in the input range to be scanned.
        * \param last  The last iterator in the input range to be scanned.
        * \param result  The first iterator in the output range.
        * \param user_code A client-specified string that is appended to the generated OpenCL kernel.
        * \tparam InputIterator An iterator signifying the range is used as input.
        * \tparam OutputIterator An iterator signifying the range is used as output.
        * \return An iterator pointing at the end of the result range.
        *
        * \code
        * #include "bolt/cl/scan.h"
        *
        * int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        *
        * // Calculate the inclusive scan of an input range, modifying the values in-place.
        * bolt::cl::inclusive_scan( a, a+10, a );
        * // a => {1, 3, 6, 10, 15, 21, 28, 36, 45, 55}
        *  \endcode
        * \sa http://www.sgi.com/tech/stl/partial_sum.html
        */
        template<
            typename InputIterator,
            typename OutputIterator,
            typename UnaryFunction,
            typename BinaryFunction>
            OutputIterator
        transform_inclusive_scan(
            InputIterator first,
            InputIterator last,
            OutputIterator result, 
            UnaryFunction unary_op,
            BinaryFunction binary_op,
            const std::string& user_code="" );

        template<
            typename InputIterator,
            typename OutputIterator,
            typename UnaryFunction,
            typename BinaryFunction>
            OutputIterator
        transform_inclusive_scan(
            bolt::cl::control &ctl,
            InputIterator first,
            InputIterator last,
            OutputIterator result, 
            UnaryFunction unary_op,
            BinaryFunction binary_op,
            const std::string& user_code="" );

        template<
            typename InputIterator,
            typename OutputIterator,
            typename UnaryFunction,
            typename T,
            typename BinaryFunction>
            OutputIterator
        transform_exclusive_scan(
            InputIterator first,
            InputIterator last,
            OutputIterator result, 
            UnaryFunction unary_op,
            T init,
            BinaryFunction binary_op,
            const std::string& user_code="" );

        template<
            typename InputIterator,
            typename OutputIterator,
            typename UnaryFunction,
            typename T,
            typename BinaryFunction>
            OutputIterator
        transform_exclusive_scan(
            bolt::cl::control &ctl,
            InputIterator first,
            InputIterator last,
            OutputIterator result, 
            UnaryFunction unary_op,
            T init,
            BinaryFunction binary_op,
            const std::string& user_code="" );

    }// end of bolt::cl namespace
}// end of bolt namespace

#include <bolt/cl/detail/transform_scan.inl>

#endif