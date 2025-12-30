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

TEST(DoubleLinkedListTests, Test_Iterator) {
    ads::DoubleLinkedList<int> list;
    list.push_back(0);
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    int count = 0;
    for (const auto &v : list) {
        EXPECT_EQ(v, count);
        count++;
    }

    for (auto &v : list) {
        v = -1 * v;
    }

    count = 0;
    for (const auto &v : list) {
        EXPECT_EQ(v, count);
        count--;
    }

    list.pop_front();
    list.pop_back();
    EXPECT_EQ(list.size(), 4);

    count = -1;
    for (const auto &v : list) {
        EXPECT_EQ(v, count);
        count--;
    }
}

TEST(DoubleLinkedListTests, Test_ExistsAndRemove) {
    ads::DoubleLinkedList<int> list;

    EXPECT_FALSE(list.exists(0));
    EXPECT_FALSE(list.remove(0));

    list.push_back(0);
    list.push_back(1);
    list.push_back(2);

    EXPECT_EQ(list.size(), 3);
    EXPECT_TRUE(list.exists(0));
    EXPECT_TRUE(list.exists(1));
    EXPECT_TRUE(list.exists(2));
    EXPECT_TRUE(list.remove(1));

    EXPECT_EQ(list.size(), 2);
    EXPECT_TRUE(list.exists(0));
    EXPECT_TRUE(list.exists(2));
    EXPECT_TRUE(list.remove(0));

    EXPECT_EQ(list.size(), 1);
    EXPECT_TRUE(list.exists(2));
    EXPECT_TRUE(list.remove(2));

    EXPECT_EQ(list.size(), 0);

    EXPECT_FALSE(list.exists(0));
    EXPECT_FALSE(list.remove(0));
}
