/*
Copyright (c) 2018 Marek Dabek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef NAVARRO_ARRAY_
#define NAVARRO_ARRAY_

#include <climits> //CHAR_BIT

namespace FFArray
{
/// @brief Navarro array implementation
/**
 Implementation of the constant init, write and read array, as described by Navarro
 in Constant Time Array Initialization in Little Space by  Gonzalo Navarro.
 The template class assumes that there is a preallocated memory region, which stores
 data structures.

 Necesary memory region size can be obtained by calling a static method GetMemorySize.
*/
template <class Data_T, typename Index_T = unsigned int>
class NavarroArray
{
  public:

  //@brief Header of array
  struct ArrayHeader
  {

    ArrayHeader(Data_T init) : val_(init)
    {
    }

    Data_T   val_;
    Index_T  t_ = 0;
  };

  //@brief returns bytes of memory required for array
  /**
   This method returns a number of bytes that is required for the array
   for the provided number of elements of a given data type.
   */
  //@param count - number of elements of Data_T type to store
  //
  //@returns number of bytes required for the array
  static size_t GetMemorySize(Index_T count)
  {
    //Data + 3*bitfields + header
    return count*(sizeof(Data_T)) + ((3*count)/(sizeof(Index_T)*CHAR_BIT) + 1) + sizeof(ArrayHeader);
  }

  //Remove default constructor
  NavarroArray() =  delete;

  NavarroArray(void* mem_buf, size_t size, Data_T init)
  {
    //The Navarro Array scheme is as follows
    //Header - containing initialization value and stack size - t,
    //Data - also called A array,
    //Index - also called B array,
    //Stack - also called S.
    header_ = new(mem_buf) ArrayHeader(init);

    //multiplication sizeof(Index_T)*CHAR_BIT, to avoid 3/sizeof(Index_T)*CHAR_BIT
    const size_t data_count = ((size - sizeof (ArrayHeader) - 1) * sizeof(Index_T)*CHAR_BIT)/
                              (sizeof(Data_T)*sizeof(Index_T)*CHAR_BIT+3);


    const size_t data_size = data_count * sizeof(Data_T);
    const size_t index_size = (data_count/CHAR_BIT) + 1;

    data_ = (Data_T*) ((char*)header_ + sizeof(ArrayHeader));
    initialized_ = (Index_T*) ((char*)data_ + data_size);
    index_ = (Index_T*) ((char*)initialized_ + index_size);
    stack_ = (Index_T*) ((char*)index_ + index_size);
  }

  //Rule of five - explanation
  //No copy allowed
  NavarroArray(NavarroArray& ) = delete;
  //Only move allowed
  NavarroArray(NavarroArray&& source)
  {
    forward(source);
  }

  //No copy assignment allowed
  NavarroArray& operator=(NavarroArray&) = delete;
  //Move assignment opearator
  NavarroArray& operator=(NavarroArray&& source)
  {
    return forward(source);
  }

  ~NavarroArray()
  {
  }
  //@brief Write value under index in O(1)
  //
  //@param index - array index
  //@param value - value under index
  //
  //@returns None
  void write(const Index_T index, const Data_T value)
  {
    if (is_cell_initialized(index))
    {
    data_[index] = value;
    }
    else
    {
    Index_T stack_index;
    Index_T arr_index, bit;

    get_position(header_->t_, stack_index, bit);
    get_position(index, arr_index, bit);

    index_[arr_index] = stack_index;
    stack_[stack_index] = arr_index;
    initialized_[arr_index] |= (1 << bit);

    data_[index] = value;
    header_->t_++;
    }
  }
  //@brief Read value under index in O(1)
  //
  //@param index - array index
  //@param value - value under index
  //
  //@returns reference
  Data_T operator[](const Index_T index)
  {
    return (is_cell_initialized(index) ? data_[index] : header_->val_);
  }
  //@brief re-initialize the array to a new value
  //
  //@param value - new value of the array
  void init(const Data_T value)
  {
    header_ = new (header_) ArrayHeader(value);
  }

  private:
  //Forward implementation
  NavarroArray& forward(NavarroArray&& source)
  {
    if(this != source)
    {
      this->data_ = source.data_;
      this->index_ = source.index_;
      this->stack_ = source.stack_;

      source.data_ = nullptr;
      source.index_ = nullptr;
      source.stack_ = nullptr;
    }
    return *this;
  }

  bool is_cell_initialized(const Index_T index) const
  {
    Index_T arr_index=0, bit=0, stack_limit=0;

    get_position(header_->t_, stack_limit, bit);
    get_position(index, arr_index, bit);

    auto stack_index = index_[arr_index];
    //If A[i] is intialized:
    //((0 < B[i] < t) && (S[B[i]] == i)
    return (stack_index >= 0) && (stack_index <= stack_limit) &&
           (stack_[stack_index] == arr_index) &&
           (initialized_[arr_index] & (1 << bit));
  }

  void get_position(const Index_T index, Index_T& arr_index, Index_T& bit) const
  {
    arr_index = index / (sizeof(Index_T)*CHAR_BIT);
    bit = index % (sizeof(Index_T)*CHAR_BIT);
  }

  ArrayHeader* header_;

  Data_T*     data_;

  Index_T* index_;
  Index_T* initialized_;
  Index_T* stack_;

};

}
#endif //NAVARRO_ARRAY_
