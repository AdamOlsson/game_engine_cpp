#include "ads/DoubleLinkedList.h"
#include <gtest/gtest.h>

TEST(DoubleLinkedListTests, Test_PushBack) {
    ads::DoubleLinkedList<int> list;
    EXPECT_EQ(list.size(), 0);

    list.push_back(42);
    EXPECT_EQ(list.size(), 1);
    int &v = list.back();
    EXPECT_EQ(v, 42);
    v = 44;
    EXPECT_EQ(v, 44);
}

TEST(DoubleLinkedListTests, Test_PopBack) {
    ads::DoubleLinkedList<int> list;
    EXPECT_EQ(list.size(), 0);

    list.push_back(42);
    list.push_back(43);
    list.push_back(44);
    EXPECT_EQ(list.size(), 3);

    int value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 44);
    EXPECT_EQ(list.size(), 2);

    value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 43);
    EXPECT_EQ(list.size(), 1);

    value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 42);
    EXPECT_EQ(list.size(), 0);

    list.pop_back();
    EXPECT_EQ(list.size(), 0);

    list.push_front(43);
    list.push_back(44);
    list.push_front(42);
    EXPECT_EQ(list.size(), 3);

    value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 44);
    EXPECT_EQ(list.size(), 2);

    value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 43);
    EXPECT_EQ(list.size(), 1);

    value = list.back();
    list.pop_back();
    EXPECT_EQ(value, 42);
    EXPECT_EQ(list.size(), 0);

    list.pop_back();
    EXPECT_EQ(list.size(), 0);
}

TEST(DoubleLinkedListTests, Test_PushFront) {
    ads::DoubleLinkedList<int> list;
    EXPECT_EQ(list.size(), 0);

    list.push_front(42);
    EXPECT_EQ(list.size(), 1);
    int &v = list.front();
    EXPECT_EQ(v, 42);
    v = 44;
    EXPECT_EQ(v, 44);
}

TEST(DoubleLinkedListTests, Test_PopFront) {
    ads::DoubleLinkedList<int> list;
    EXPECT_EQ(list.size(), 0);

    list.push_front(42);
    list.push_front(43);
    list.push_front(44);
    EXPECT_EQ(list.size(), 3);

    int value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 44);
    EXPECT_EQ(list.size(), 2);

    value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 43);
    EXPECT_EQ(list.size(), 1);

    value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 42);
    EXPECT_EQ(list.size(), 0);

    list.pop_back();
    EXPECT_EQ(list.size(), 0);

    list.push_back(43);
    list.push_front(44);
    list.push_back(42);
    EXPECT_EQ(list.size(), 3);

    value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 44);
    EXPECT_EQ(list.size(), 2);

    value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 43);
    EXPECT_EQ(list.size(), 1);

    value = list.front();
    list.pop_front();
    EXPECT_EQ(value, 42);
    EXPECT_EQ(list.size(), 0);

    list.pop_front();
    EXPECT_EQ(list.size(), 0);
}
