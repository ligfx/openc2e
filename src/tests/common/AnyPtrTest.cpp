#include "common/AnyPtr.h"

#include <gtest/gtest.h>

TEST(AnyPtr, AnyPtr) {
	// TODO
}

class IncrementUponDestroy {
  public:
	IncrementUponDestroy(int* counter, int value)
		: m_counter(counter), m_value(value) {}
	~IncrementUponDestroy() {
		if (m_counter) {
			*m_counter += m_value;
		}
	}
	int* m_counter = nullptr;
	int m_value = 1;
};

TEST(AnyPtr, UniqueAnyPtr) {
	int counter = 0;
	{
		UniqueAnyPtr x;
		EXPECT_FALSE(x);

		x = UniqueAnyPtr(new IncrementUponDestroy(&counter, 5));
		EXPECT_TRUE(x);

		EXPECT_EQ(x.get<int>(), nullptr);
		ASSERT_NE(x.get<IncrementUponDestroy>(), nullptr);
		EXPECT_EQ(x.get<IncrementUponDestroy>()->m_value, 5);

		UniqueAnyPtr y = std::move(x);
		UniqueAnyPtr z;
		EXPECT_FALSE(z);
		z = std::move(y);
		EXPECT_EQ(y.get<IncrementUponDestroy>(), nullptr);
		ASSERT_NE(z.get<IncrementUponDestroy>(), nullptr);
		EXPECT_EQ(z.get<IncrementUponDestroy>()->m_value, 5);
	}
	EXPECT_EQ(counter, 5);
}