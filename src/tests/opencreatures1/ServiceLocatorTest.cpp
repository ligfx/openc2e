#include "opencreatures1/ServiceLocator.h"

#include <gtest/gtest.h>

struct MyThing {
	MyThing(int value_)
		: value(value_) {}
	int value;
};

TEST(ServiceLocator, ServiceLocator) {
	MyThing* p = g_services.get<MyThing>();
	EXPECT_EQ(p, nullptr);

	g_services.emplace<MyThing>(5);
	p = g_services.get<MyThing>();

	EXPECT_NE(p, nullptr);
	EXPECT_EQ(p->value, 5);

	g_services.clear();
	p = g_services.get<MyThing>();
	EXPECT_EQ(p, nullptr);
}