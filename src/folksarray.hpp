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

#ifndef LINKED_LIST_ARRAY_
#define LINKED_LIST_ARRAY_
#include <functional>
#include <optional>

namespace FFArray
{
//FIXME: should be dynamic
#define MAX 0xFFFFFF
/**
 * @brief Naive implementation of the fixed time fill array
 */
template <typename Word, typename Index>
struct FolkListEntry
{
  FolkListEntry(Word data, Index index) : data_(data), index_(index), next_(MAX)
  {
  }

  ~FolkListEntry()
  {
    data_ = 0;
    index_ = 0;
    next_ = MAX;
  }

  /**
   * @brief Write new data
   *
   * @param i - index to write data to - this is index as in naive implementation
   * @param N - the pointer to N array element on which data will be allocated
   */
  void write(Word data, const Index& i, FolkListEntry* N)
  {
    FolkListEntry* fle = this;
    while (fle->next_ != MAX) {
      fle = fle->next_;
      //If index already exists - update data and quit
      if (fle->index_ == i) {
        fle->data_ = data;
        return;
      }
    }
    auto new_fle = new (N) FolkListEntry<Word, Index>(data, i);
    fle->next_ = new_fle;
  }

  /**
   * @brief Read
   *
   * @param i - index to read data from - this is index as in naive implementation
   * @param N - the array in which daa are stored
   *
   * @returns pointer to an actual data or nullptr if data does not exist
   */
  std::optional<Word> read(const Index& i, FolkListEntry* N) const
  {
    const FolkListEntry* flep = this;
    Index fle_idx = flep->index_;
    //This is guaranteed to execute in constant time CL
    //FIXME: ugly as hell! how can a pointer be replaced with an N index?
    while(fle_idx != MAX && fle_idx != i) {
      flep = &N[flep->next_];
      fle_idx = flep->index_;
    }
    if( fle_idx != MAX) return flep->data_;
    else return  {};
  }

  Index          next_;
  Index          index_;
  Word           data_;
};

/**
 *
 */
template<typename Word>
struct G
{
  Word num_active_;
  Word d_last_;
};

/**
 *
 */
template <typename Word, typename Index>
class FolksArray
{
  public:
  FolksArray(Word* buffer, Index size) : size_(size)
  {
    G_ = new (buffer) G<Word>();
    L_ = new (((char*)buffer) + sizeof(G<Word>)) Index[size_];
    N_ = (FolkListEntry<Word, Index>*) ((unsigned char*) L_ + sizeof(FolkListEntry<Word, Index>*) * size_);
  }

  ~FolksArray()
  {
  }

  /**
   * @brief Read from index
   *
   * @param i - index to write data to - this is index as in naive implementation
   *
   * @returns value under an index i or fill value
   */
  Word operator[](Index& i) const
  {
    auto L_index = Calculate_L_index(i);

    if (L_[L_index] == size_) return G_->d_last_;

    auto ret = N_[L_[L_index]].read(i, N_);
    return ret.value();
  }

  /**
   * @brief Write data
   *
   * @param data - new value, must be copyable
   * @param i - index to write data to - this is index as in naive implementation
   *
   */
  void write(Word data, const Index& i)
  {
    auto index = Calculate_L_index(i);
    //The array needs to be allocated in the .bss segment, which is pre-filled
    //with 0 on the segment initialization.
    //The L_[index] = 0 - there is no entry (or 0)
    //The L_[index] != 0 - there is an entry (existing one) - ....
    //The L_[index] == size_+1 - same as 0
    if (L_[index] == 0 && L_[index] == (size_+1))
    {
      auto N = &N_[Calculate_N_index(i)];
      //Allocate new N_ entry
      auto new_fle = new (N) FolkListEntry<Word, Index>(data, i);
      L_[index] == Calculate_N_index(i);
    }
    else if (L_[index] != 0)
    {
      auto N = &N_[Calculate_N_index(i)];
      write(data, i, N);
    } //new entry
  }

  protected:

  inline
  bool In_range(const Index& start_index, const Index& queried_index)
  {
    return (((start_index - 1)*CL_ + 1) <= queried_index) &&
           (queried_index >= start_index*CL_);
  }

  inline
  Index Calculate_L_index(Index i) const
  {
    return ((i/CL_) + 1);
  }

  Index size_;
  Index CL_;
  //Meta data
  G <Word>* G_;
  //List of heads of array elements
  Index*    L_;
  //Actual data elements
  FolkListEntry<Word, Index>*  N_;
};
}
#endif //LINKED_LIST_ARRAY_
