#include <iostream>
#include "navaroarray.hpp"
#include <gtest/gtest.h>

class NavaroArrayTest : public ::testing::Test
{
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    NavaroArrayTest() {
    // You can do set-up work for each test here.
    }

    ~NavaroArrayTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
    mem_buffer_ = new uint8_t[buffer_size_];
    }

    void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    delete[] mem_buffer_;
    }

    // Objects declared here can be used by all tests in the test case
    const uint32_t buffer_size_ = FFArray::NavarroArray<uint64_t>::GetMemorySize(4096);
    uint8_t* mem_buffer_;
};

TEST_F(NavaroArrayTest, SizeTest)
{
  auto proposed_size = FFArray::NavarroArray<uint64_t>::GetMemorySize(1024);
  EXPECT_EQ(proposed_size, 16400);

  proposed_size = FFArray::NavarroArray<uint64_t>::GetMemorySize(512);
  EXPECT_EQ(proposed_size, 8208);

  proposed_size = FFArray::NavarroArray<uint8_t>::GetMemorySize(1024);
  EXPECT_EQ(proposed_size, 9224);

  proposed_size = FFArray::NavarroArray<uint8_t>::GetMemorySize(512);
  EXPECT_EQ(proposed_size, 4616);

  proposed_size = FFArray::NavarroArray<uint64_t, uint16_t>::GetMemorySize(1024);
  EXPECT_EQ(proposed_size, 12304);

  proposed_size = FFArray::NavarroArray<uint64_t, uint16_t>::GetMemorySize(512);
  EXPECT_EQ(proposed_size, 6160);

  proposed_size = FFArray::NavarroArray<uint8_t, uint16_t>::GetMemorySize(1024);
  EXPECT_EQ(proposed_size, 5124);

  proposed_size = FFArray::NavarroArray<uint8_t, uint16_t>::GetMemorySize(512);
  EXPECT_EQ(proposed_size, 2564);
}

TEST_F(NavaroArrayTest, InitTest)
{
  unsigned int max_count = 1024;
  auto max_size = FFArray::NavarroArray<uint64_t>::GetMemorySize(max_count);
  auto array = FFArray::NavarroArray<uint64_t>{mem_buffer_, max_size, 0xDEADBEEF};
  unsigned int idx = 0;

  for (idx = 0; idx < max_count; idx ++)
  {
    EXPECT_EQ(array[idx], 0xDEADBEEF);
  }

  max_count = 512;
  max_size = FFArray::NavarroArray<uint16_t, uint16_t>::GetMemorySize(max_count);
  auto array1 = FFArray::NavarroArray<uint16_t, uint16_t>{mem_buffer_, max_size, 0xDEAD};
  uint16_t idx_sh = 0;
  for (idx_sh = 0; idx_sh < max_count; idx_sh++)
  {
    EXPECT_EQ(array1[idx_sh], 0xDEAD);
  }
}

TEST_F(NavaroArrayTest, WriteTest)
{
  unsigned int max_count = 1024;
  auto max_size = FFArray::NavarroArray<uint64_t>::GetMemorySize(max_count);
  auto array = FFArray::NavarroArray<uint64_t>{mem_buffer_, max_size, 0xDEADBEEF};
  unsigned int idx = 0;

  for (idx = 0; idx < max_count; idx++)
  {
    EXPECT_EQ(array[idx], 0XDEADBEEF);
    array.write(idx, 0xFEED);
    EXPECT_EQ(array[idx], 0XFEED);
  }

  max_count = 512;
  max_size = FFArray::NavarroArray<uint16_t, uint16_t>::GetMemorySize(max_count);
  auto array1 = FFArray::NavarroArray<uint16_t, uint16_t>{mem_buffer_, max_size, 0xDEAD};
  uint16_t idx_sh = 0;
  for (idx_sh = 0; idx_sh < max_count; idx_sh++)
  {
    EXPECT_EQ(array1[idx_sh], 0XDEAD);
    array1.write(idx_sh, 0xBEEB);
    EXPECT_EQ(array1[idx_sh], 0xBEEB);
  }
  std::cout << std::endl;
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
