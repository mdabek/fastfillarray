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
    return count*(sizeof(Data_T) + (2*sizeof(Index_T))) + sizeof(ArrayHeader);
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

    data_count_ = (size - sizeof (ArrayHeader)) / (sizeof(Data_T) + (2*sizeof(Index_T)));
    index_count_ = data_count_ / 2;

    const size_t data_size = data_count_ * sizeof(Data_T);
    const size_t index_size = index_count_ * sizeof(Index_T);

    data_ = (Data_T*) (header_ + sizeof(ArrayHeader));
    index_ = (Index_T*) (data_ + data_size);
    stack_ = (Index_T*) (index_ + index_size);
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
    auto stack_index = header_->t_;
    index_[index] = stack_index;
    stack_[stack_index] = index;
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
      this->data_count_ = source.data_count_;
      this->index_count_ = source.index_count_;

      source.data_ = nullptr;
      source.index_ = nullptr;
      source.stack_ = nullptr;
      source.data_count_ = 0;
      source.index_count_ = 0;
    }
    return *this;
  }

  bool is_cell_initialized(const Index_T index) const
  {
    auto stack_index = index_[index];
    //If A[i] is intialized:
    //((0 < B[i] < t) && (S[B[i]] == i)
    return (stack_index >= 0) && (stack_index < header_->t_) &&
          (stack_[stack_index] == index);
  }

  ArrayHeader* header_;

  Data_T*     data_;

  Index_T* index_;
  Index_T* stack_;

  size_t  data_count_;
  size_t  index_count_;
};

}
#endif //NAVARRO_ARRAY_
