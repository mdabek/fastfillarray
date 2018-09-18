namespace FFArray
{
template<class T>
class FastFillArray
{
  public:
    FastFillArray();
    FastFillArray(unsigned int N_elem);
    FastFillArray(FastFillArray&& ffa);

    ~FastFillArray();

    FastFillArray& operator=(FastFillArray&&);

    T operator[](unsigned int) const;
    void fill(T);
    void insert(unsigned int, T);

    //Copy operations on the array are very expensive!
    FastFillArray(const FastFillArray&) = delete;
    FastFillArray& operator=(const FastFillArray&) = delete;

  protected:
    void move(FastFillArray&&);

    void insert_linked(unsigned int idx, T elem);
    void transform_to_naive();

    bool naive_;

    unsigned int n_;

    unsigned int num_active_;
    T  d_last_;
    T* data_;
};
}
