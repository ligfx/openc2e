#include "common/TypeErasedSharedPtr.h"

#include <gtest/gtest.h>

struct IncrementOnDestroy {
	IncrementOnDestroy(int* destroyed_)
		: destroyed(destroyed_) {}
	~IncrementOnDestroy() { (*destroyed)++; }
	int* destroyed = nullptr;
};

TEST(TypeErasedSharedPtr, TypeErasedSharedPtr) {
	int destroyed = 0;
	{
		TypeErasedSharedPtr p;

		auto m1 = std::make_shared<IncrementOnDestroy>(&destroyed);
		p = TypeErasedSharedPtr(m1);
		EXPECT_EQ(p.get<IncrementOnDestroy>(), m1.get());

		auto m2 = std::make_shared<IncrementOnDestroy>(&destroyed);
		p = TypeErasedSharedPtr(m2);
		EXPECT_EQ(p.get<IncrementOnDestroy>(), m2.get());
	}

	EXPECT_EQ(destroyed, 2);
}