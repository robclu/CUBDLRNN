// ==========================================================================================================
//! @file   Header file for fastRNN tensor class.
// ==========================================================================================================

/*
 * ==========================================================================================================
 *  Copyright (C) 2015 Rob Clucas robclu1818@gmail.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ==========================================================================================================
 */

#ifndef _FRNN_NEW_TENSOR_
#define _FRNN_NEW_TENSOR_

#include "tensor_expressions.h"
#include "tensor_exceptions.h"
#include "tensor_indices.h"
#include "../containers/tuple.h"

#include <iostream>
#include <cassert>
#include <initializer_list>
#include <numeric>
#include <type_traits>

namespace frnn {

// ==========================================================================================================
//! @class  Tensor 
//! @brief  Allows an N dimensional space to be created to store data and operate on the data.               \n
//!                                                                                                          \n
//!         The first dimension of the Tensor is 'accross' or the number of columns, so that the first       \n 
//!         dimension is arranged as it it in memory so a 2D Tensor with dimensions [3,2] will have 3        \n
//!         and 2 rows.                                                                                      \n
//!                                                                                                          \n
//!         Detailed usage of the Tensor class can be found in the unit tests for the class.                 \n
//!         The basic usage is:                                                                              \n
//!                                                                                                          \n
//!         Tensor<int, 3> tensor = {2, 2, 2}           // Create a 3-dimensional (rank 3) Tensor            \n
//!                                                                                                          \n
//!         The above Tensor is of int type and each dimension has a size of 2, thus the Tensor is a         \n
//!         matrix with 2 pages. Operations can be performed such as:                                        \n
//!                                                                                                          \n
//!         Tensor<int, 3> new_tensor = tensor + tensor + tensor    // Add 3 Tensors                         \n
//!         Tensor<int, 2> slice_tensor = tensor(j,i)               // New tensor from dim 1 and 2 of old 
//!                                                                    tensor                                \n
//! @tparam T   Type of data used by the Tensor.
//! @tparam R   Rank of the Tensor (the number of dimensions it has).
// ==========================================================================================================
template <typename T, const size_t R>
class Tensor : public TensorExpression<T, Tensor<T, R>> {
public:
    /* =========================================== Typedefs =============================================== */
    using typename TensorExpression<T, Tensor<T,R>>::container_type;
    using typename TensorExpression<T, Tensor<T,R>>::size_type;
    using typename TensorExpression<T, Tensor<T,R>>::value_type;
    using typename TensorExpression<T, Tensor<T,R>>::reference;
    /* ==================================================================================================== */
private:
    container_type          _data;                  //!< Container to hold Tensor data elements
    std::vector<size_type>  _dimensions;            //!< Sizes of each of the Tensor's dimensions
    size_type               _counter;               //!< Iteration of the elemen offset calculation 
    size_type               _offset;                //!< For accessing elements with operator()
public:
    // =====================================================================================================
    //! @brief     Default constructor - sets the member variables to 0, and the number of dimensions equal 
    //!            to the rank.
    // =====================================================================================================
    Tensor() : _data(0), _dimensions(R), _counter(0), _offset(0) {}
    
    // =====================================================================================================
    //! @brief     Constructor using an initializer list - sets the size of each of the dimensions to the 
    //!            values in the intializer_list and the total number of elements equal to the product of 
    //!            the dimension sizes.
    //! @param[in] dimensions  The list of dimensions where the nth element in the list sets the size of the 
    //!            nth dimension of the Tensor.
    // =====================================================================================================
    Tensor(std::initializer_list<int> dimensions) 
    : _data(std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<int>())),
      _counter(0), _offset(0)
    {   
        ASSERT(dimensions.size(), ==, R); 
        for (auto& element : dimensions) _dimensions.push_back(element);
    }
    
    // =====================================================================================================
    //! @brief     Constructor using a TensorExpression - sets the data of the Tensor to the data of the
    //!            TensorExpression so that Tensors can be created from the ouputs of operations such as 
    //!            addition and subtraction.
    //! @param[in] expression  The expression which must be used to construct the Tensor.
    //! @tparam    E           The type of the expression.
    // =====================================================================================================
    template <typename E>
    Tensor(TensorExpression<T,E> const& expression) 
    : _dimensions(expression.dimSizes()), _counter(0), _offset(0)
    {
        E const& expr = expression;
        _data.resize(expr.size());
        for (size_type i = 0; i != expr.size(); ++i) {
            _data[i] = expr[i];
        }
    }
   
    // =====================================================================================================
    //! @brief     Constructor using vectors to set the dimension sizes and the data of the Tensor.
    //! @param     dimensions  The sizes of each of the dimensions for the Tensor.
    //! @param     data        The data for the Tensor.
    // =====================================================================================================
    Tensor(std::vector<size_type>& dimensions, container_type& data) 
    : _dimensions(std::move(dimensions)), _data(std::move(data)), _counter(0), _offset(0) 
    {
        ASSERT(_dimensions.size(), ==, R);           // Check number of dimensions is equal to the rank
        ASSERT(_data.size(), ==,                     // Check total data size is consistent with dim sizes
               std::accumulate(_dimensions.begin()          , 
                               _dimensions.end()            , 
                               1                            , 
                               std::multiplies<size_type>() ));
    }
  
    // =====================================================================================================
    //! @brief     Gets the size (total number of elements) of the Tensor
    //! @return    The total number of elements in the Tensor.
    // =====================================================================================================
    size_type size() const { return _data.size(); }

    // =====================================================================================================
    //! @brief     Gets the size of a specific dimension of the Tensor, if the requested dimension is 
    //!            invalid then 0 is returned.
    //! @param[in] dim                 The dimension for which the size must be returned.
    //! @return    The number of elements in the requested dimension, if the dimension is a valid dimension
    //!            for the Tensor, otherwise 0 is returned.
    //! @throw     TesnorOutOfRange    Throws an error if the requested dimension is invalid for the Tensor.
    // =====================================================================================================
    size_type size(const int dim) const 
    {
        try {
            if (dim >= R) throw TensorOutOfRange(dim, R);
            return _dimensions[dim];
        } catch (TensorOutOfRange& e) {
            std::cout << e.what() << std::endl;
            return 0;
        }
    }
    
    // =====================================================================================================
    //! @brief     Gets the rank (number of dimensions) of the Tensor.
    //! @return    The rank (number of dimensions) of the Tensor.
    // =====================================================================================================
    size_type rank() const { return R; }

    // ======================================================================================================
    //! @brief      Gets a vector holding the size of each dimension of the Tensor.
    //! @return     A vector holding the size of each dimension of the Tensor.
    // ======================================================================================================
    const std::vector<size_type>& dimSizes() const { return _dimensions; }
     
    // ======================================================================================================
    //! @brief      Gets the Tensor data.
    //! @return     The data for the Tensor.
    // ======================================================================================================
    const container_type& data() const { return _data; }

    // ======================================================================================================
    //! @brief      Gets the element at position i in the Tensor's data vector, by reference.
    //! @param[in]  i   The index of the element to access.
    //! @return     The element at position i in the Tensor's data vecor.
    // ======================================================================================================
    reference operator[](size_type i) { return _data[i]; }
    
    // ======================================================================================================
    //! @brief      Gets the element at position i in the Tensor's data vector, by value.
    //! @param[in]  i   The index of the element to access.
    //! @return     The element at position i in the Tensor's data vector.
    // ======================================================================================================
    value_type operator[](size_type i) const { return _data[i]; }

    // ======================================================================================================
    //! @brief      Returns a TensorSlice which is a remapping of the dimensions of this Tensor in some way. \n
    //!                                                                                                      \n
    //!             For example, for a 2D 3x2 float Tensor                                                   \n
    //!                                                                                                      \n
    //!             Tensor<float, 2> tensor = {3, 2}                                                         \n
    //!                                                                                                      \n
    //!             Then slicing the Tensor as                                                               \n
    //!                                                                                                      \n
    //!             Tensor<float, 2> new_tensor = tensor(1,0)                                                \n
    //!                                                                                                      \n
    //!             Will transpose it, to make a 2D 2x3 Tensor
    //! @param[in]  dims    The dimensions of the old Tensor which will make the sliced Tensor/
    //! @return     A TensorSlice which is a remapping of this Tensor's dimensions.
    //! @tparam     Ts      The types of the dimension variables.
    // ======================================================================================================
    template <typename... Ts>
    TensorSlice<T, Tensor<T,R>, Ts...> operator()(Ts... dims) const 
    {
        return TensorSlice<T, Tensor<T,R>, Ts...>(static_cast<Tensor<T,R> const&>(*this),
                                                  Tuple<Ts...>(dims...)                 );          
    }
    
    // ======================================================================================================
    //! @brief      Terminating case for the calculation of the offset for getting an element.
    //! @param[in]  idx     The index in the last dimension of the element of the Tensor to get.
    //! @tparam     I       The type of the idx parameter.
    // ======================================================================================================
    template <typename I>
    typename std::enable_if<std::is_arithmetic<I>::value, T&>::type  operator() (I idx) 
    {
        try {                                                                           // Check in range
            if (idx >= _dimensions[_counter]) {                                 // counter +1 in next line for
                throw TensorOutOfRange(_counter + 1, _dimensions[_counter], idx);       // 0 indexing offset
            }
        } catch (TensorOutOfRange& e) {
            std::cerr << e.what() << std::endl;
            _counter = 0;
            return _data[0];
        }
        try {                                                                   // Make sure variadic version 
            if (_counter == 0) throw TensorInvalidArguments(1, R);              // has been called already
        } catch (TensorInvalidArguments& e) {
            std::cerr << e.what() << std::endl;
        }
        _offset += std::accumulate(_dimensions.begin()      , 
                                   _dimensions.end() - 1    ,               // Mult all elements except last
                                   1                        ,               // Start val for multiplication
                                   std::multiplies<int>()   ) * idx;        // Add offset due to idx for dim
        _counter = 0;                                                       // Reset counter for next iter
        return _data[_offset];
    }
    
    /* 
     * ======================================================================================================
     * Function     : operator() (for setting)
     * Description  : Variadic case of operator() so that the offset is determined for any rank tensor
     * Inputs       : idx       : The index for the current dimension so that the offset for the 
     *                            dimension can be added
     *              : indices   : The rest of the indecies for the other dimensions
     * Params       : I         : The type of the idx argument
     *              : Is        : The types for the remaining indecies
     * ======================================================================================================
     */
    template <typename I, typename... Is>
    typename std::enable_if<std::is_arithmetic<I>::value, T&>::type operator()(I idx, Is... indices) 
    {
        const int num_args = sizeof...(Is);
        try {                                                           // Check index in range
            if (idx >= _dimensions[_counter]) {                         // counter + 1 int next line for
                throw TensorOutOfRange(_counter + 1, _dimensions[_counter], idx);   // 0 indexing offset
            }
        } catch (TensorOutOfRange& e ) {
            std::cout << e.what() << std::endl;
            _counter = 0;
            return _data[0];
        }   
        if (_counter++ == 0) {                                          // Case for first index
            try {                                                       // Check correct number of arguments
                if (num_args + 1 !=  R) throw TensorInvalidArguments(num_args + 1, R);
                _offset = idx;
            } catch (TensorInvalidArguments& e) {
                std::cerr << e.what() << std::endl;
                return _data[0];
            }  
        } else {                                                        // Case for all other indecies
            _offset += std::accumulate(_dimensions.begin()              , 
                                       _dimensions.end() - num_args - 1 ,
                                       1                                , 
                                       std::multiplies<int>()           ) * idx;
        }
        return this->operator()(indices...);
    }  
   
    /*
     * ======================================================================================================
     * Function     : operator() (for getting)
     * Description  : Terminating case of operator(), for when there is only a single element
     * Inputs       : idx       : The index of the element in the last dimension (the variadic version 
     *                            would have been called fist for the other dimensions
     * Params       : I         : The type of the idx argument
     * ======================================================================================================
     */
    template <typename I>
    typename std::enable_if<std::is_arithmetic<I>::value, const T&>::type operator()(I idx) const 
    {
        try {                                                           // Check in range
            if (idx >= _dimensions[_counter]) {                         // counter +1 in next line for
                throw TensorOutOfRange(_counter + 1, _dimensions[_counter], idx);   // 0 indexing offset
            }
        } catch (TensorOutOfRange& e) {
            std::cerr << e.what() << std::endl;
            _counter = 0;
            return _data[0];
        }
        try {                                                           // Make sure variadic version 
            if (_counter == 0) throw TensorInvalidArguments(1, R);      // has been called already
        } catch (TensorInvalidArguments& e) {
            std::cerr << e.what() << std::endl;
        }
        _offset += std::accumulate(_dimensions.begin()      , 
                                   _dimensions.end() - 1    ,           // Mult all elements exepct last
                                   1                        ,           // Start val for multiplication
                                   std::multiplies<int>()   ) * idx;    // Add offset due to idx for dim
        _counter = 0;                                                   // Reset counter for next iter
        return _data[_offset]; 
    }
    
    /* 
     * ======================================================================================================
     * Function     : operator() (for getting)
     * Description  : Variadic case of operator() so that the offset is determined for any rank tensor
     * Inputs       : idx       : The index for the current dimension so that the offset for the 
     *                            dimension can be added
     *              : indecies  : The rest of the indecies for the other dimensions
     * Params       : I         : The type of the idx argument
     *              : Is        : The types for the remaining indecies
     * ======================================================================================================
     */
    template <typename I, typename... Is>
    typename std::enable_if<std::is_arithmetic<I>::value, const T&>::type 
    operator()(I idx, Is... indecies) const
    {
        const int num_args = sizeof...(Is);
        try {                                                           // Check index in range
            if (idx >= _dimensions[_counter]) {                         // counter + 1 int next line for
                throw TensorOutOfRange(_counter + 1, _dimensions[_counter], idx);   // 0 indexing offset
            }
        } catch (TensorOutOfRange& e ) {
            std::cout << e.what() << std::endl;
            _counter = 0;
            return _data[0];
        }   
        if (const_cast<int&>(_counter)++ == 0) {                        // Case for first index
            try {                                                       // Check correct number of arguments
                if (num_args + 1 !=  R) throw TensorInvalidArguments(num_args + 1, R);
                _offset = idx;
            } catch (TensorInvalidArguments& e) {
                std::cerr << e.what() << std::endl;
                return _data[0];
            }  
        } else {                                                        // Case for all other indecies
            _offset += std::accumulate(_dimensions.begin()              , 
                                       _dimensions.end() - num_args - 1 ,
                                       1                                , 
                                       std::multiplies<int>()           ) * idx;
        }
        return this->operator()(indecies...);
    }  
};

}   // End namespace frnn

#endif
