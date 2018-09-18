#include "fastfillarray.hpp"
#include "linkedlistarray.hpp"

using namespace FFArray;

template <class T>
FastFillArray<T>::FastFillArray() : naive_(true), n_(64), num_active_(0),
                    data_ (new T[n_])
{
}

template <class T>
FastFillArray<T>::FastFillArray(unsigned int N_elem) : naive_(true), n_(N_elem), num_active_(0),
                    data_(new T[n_])
{
}

template <class T>
FastFillArray<T>::FastFillArray(FastFillArray<T>&& other)
{
  move(other);
}

template <class T>
FastFillArray<T>::~FastFillArray()
{
  delete[] data_;
}

template <class T>
FastFillArray<T>& FastFillArray<T>::operator=(FastFillArray<T>&& other)
{
  move(other);
  return *this;
}

template <class T>
T FastFillArray<T>::operator[](unsigned int idx) const
{
  return naive_ ? data_[idx] : //naive mode is just a regular array
              this->data_[idx]; //FIXME: linked list mode
}

template <class T>
void FastFillArray<T>::fill(T d)
{
  //Switch to linked list mode
  naive_ = false;
  num_active_ = 0;
  d_last_ = d;
}

template <class T>
void FastFillArray<T>::insert(unsigned int idx, T elem)
{
  naive_ ? (data_[idx] = elem) : //naive mode is a regular array
          this->insert_linked(idx, elem) ;
}

template <class T>
void FastFillArray<T>::insert_linked(unsigned int idx, T elem)
{
  //TODO:
}

template <class T>
void FastFillArray<T>::move(FastFillArray&& other)
{
  naive_ = other.naive_;
  n_ = other.n_;
  data_ = other.data_;
  other.data_ = nullptr;
}

template <class T>
void FastFillArray<T>::transform_to_naive()
{
}
