#include "ads/SparseSet.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(SparseSetTests, Test_EmptyConstruction) {
    ads::SparseSet<int> set;
    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.contains(0));
}

TEST(SparseSetTests, Test_BasicInsert) {
    ads::SparseSet<int> set;

    size_t id1 = set.insert(42);
    EXPECT_EQ(id1, 0);
    EXPECT_EQ(set.size(), 1);
    EXPECT_TRUE(set.contains(id1));
    EXPECT_EQ(set.get(id1), 42);

    size_t id2 = set.insert(100);
    EXPECT_EQ(id2, 1);
    EXPECT_EQ(set.size(), 2);
    EXPECT_TRUE(set.contains(id2));
    EXPECT_EQ(set.get(id2), 100);

    EXPECT_EQ(set.get(id1), 42);
}

TEST(SparseSetTests, Test_InsertMove) {
    ads::SparseSet<std::string> set;

    std::string value = "hello";
    size_t id = set.insert(std::move(value));
    EXPECT_TRUE(value.empty()); // Value should be moved
    EXPECT_EQ(set.get(id), "hello");
}

TEST(SparseSetTests, Test_BasicRemove) {
    ads::SparseSet<int> set;

    size_t id1 = set.insert(10);
    size_t id2 = set.insert(20);
    size_t id3 = set.insert(30);

    EXPECT_EQ(set.size(), 3);

    set.remove(id2);
    EXPECT_EQ(set.size(), 2);
    EXPECT_FALSE(set.contains(id2));
    EXPECT_TRUE(set.contains(id1));
    EXPECT_TRUE(set.contains(id3));

    EXPECT_EQ(set.get(id1), 10);
    EXPECT_EQ(set.get(id3), 30);
}

TEST(SparseSetTests, Test_RemoveNonExistent) {
    ads::SparseSet<int> set;

    set.remove(0); // Should not crash
    EXPECT_EQ(set.size(), 0);
    EXPECT_FALSE(set.contains(0));

    size_t id = set.insert(42);
    set.remove(999); // Invalid ID, should not crash
    EXPECT_EQ(set.size(), 1);
    EXPECT_TRUE(set.contains(id));
}

TEST(SparseSetTests, Test_IDReuse) {
    ads::SparseSet<int> set;

    // Insert some elements
    size_t id1 = set.insert(10);
    size_t id2 = set.insert(20);
    size_t id3 = set.insert(30);

    // Remove middle element
    set.remove(id2);
    EXPECT_FALSE(set.contains(id2));

    // Insert new element - should reuse the deleted ID
    size_t id4 = set.insert(40);
    EXPECT_EQ(id4, id2); // Should reuse the deleted ID
    EXPECT_TRUE(set.contains(id4));
    EXPECT_EQ(set.get(id4), 40);

    // Verify other elements are still correct
    EXPECT_TRUE(set.contains(id1));
    EXPECT_TRUE(set.contains(id3));
    EXPECT_EQ(set.get(id1), 10);
    EXPECT_EQ(set.get(id3), 30);
}

TEST(SparseSetTests, Test_MultipleInsertRemove) {
    ads::SparseSet<int> set;

    std::vector<size_t> ids;
    for (int i = 0; i < 10; i++) {
        ids.push_back(set.insert(i * 10));
    }

    EXPECT_EQ(set.size(), 10);

    // Remove every other element
    for (size_t i = 1; i < ids.size(); i += 2) {
        set.remove(ids[i]);
    }

    EXPECT_EQ(set.size(), 5);

    // Insert new elements - should reuse deleted IDs
    for (int i = 0; i < 5; i++) {
        size_t new_id = set.insert((i + 10) * 10);
        EXPECT_LT(new_id, 20); // Should be from reused IDs
    }

    EXPECT_EQ(set.size(), 10);
}

TEST(SparseSetTests, Test_GetReference) {
    ads::SparseSet<int> set;

    size_t id = set.insert(42);
    int &ref = set.get(id);

    EXPECT_EQ(ref, 42);
    ref = 100;
    EXPECT_EQ(set.get(id), 100); // Reference should modify the stored value
}

TEST(SparseSetTests, Test_ConstGet) {
    ads::SparseSet<int> set;
    size_t id = set.insert(42);

    const ads::SparseSet<int> &const_set = set;
    const int &const_ref = const_set.get(id);

    EXPECT_EQ(const_ref, 42);
}

TEST(SparseSetTests, Test_Clear) {
    ads::SparseSet<int> set;

    // Add some elements
    for (int i = 0; i < 5; i++) {
        set.insert(i * 10);
    }

    EXPECT_EQ(set.size(), 5);

    set.clear();
    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(2));

    // Should be able to insert new elements after clear
    size_t new_id = set.insert(100);
    EXPECT_EQ(new_id, 0); // Should restart ID generation
    EXPECT_TRUE(set.contains(new_id));
}

TEST(SparseSetTests, Test_Reserve) {
    ads::SparseSet<int> set;

    set.reserve(100);
    EXPECT_GE(set.capacity(), 100);
    EXPECT_EQ(set.size(), 0);

    // Insert elements - should not cause reallocation
    for (int i = 0; i < 50; i++) {
        set.insert(i);
    }

    EXPECT_EQ(set.size(), 50);
}

TEST(SparseSetTests, Test_Iteration) {
    ads::SparseSet<int> set;

    // Insert elements
    std::vector<int> values = {10, 20, 30, 40, 50};
    std::vector<size_t> ids;
    for (int val : values) {
        ids.push_back(set.insert(val));
    }

    // Remove some elements to test iteration over active ones only
    set.remove(ids[1]); // Remove 20
    set.remove(ids[3]); // Remove 40

    // Iterate and collect values
    std::vector<int> iterated_values;
    for (const auto &val : set) {
        iterated_values.push_back(val);
    }

    EXPECT_EQ(iterated_values.size(), 3);

    // Note: Order may change due to swap-remove behavior
    // So we check that all expected values are present
    std::sort(iterated_values.begin(), iterated_values.end());
    std::vector<int> expected = {10, 30, 50};
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(iterated_values, expected);
}

TEST(SparseSetTests, Test_MutableIteration) {
    ads::SparseSet<int> set;

    for (int i = 0; i < 5; i++) {
        set.insert(i);
    }

    // Modify values through iterator
    for (auto &val : set) {
        val *= 2;
    }

    // Check all values were modified
    std::vector<int> values;
    for (const auto &val : set) {
        values.push_back(val);
    }
    std::sort(values.begin(), values.end());

    std::vector<int> expected = {0, 2, 4, 6, 8};
    EXPECT_EQ(values, expected);
}

TEST(SparseSetTests, Test_ConstIteration) {
    ads::SparseSet<int> set;

    for (int i = 0; i < 3; i++) {
        set.insert(i * 10);
    }

    const ads::SparseSet<int> &const_set = set;

    std::vector<int> values;
    for (const auto &val : const_set) {
        values.push_back(val);
    }

    EXPECT_EQ(values.size(), 3);
}

TEST(SparseSetTests, Test_DirectIndexAccess) {
    ads::SparseSet<int> set;

    std::vector<size_t> ids;
    for (int i = 0; i < 3; i++) {
        ids.push_back(set.insert(i * 10));
    }

    // Remove one to test reordering
    set.remove(ids[1]);

    // Access by dense index
    EXPECT_EQ(set[0], 0);  // First element
    EXPECT_EQ(set[1], 20); // Third element moved to second position

    // Test get_id to map dense index back to ID
    EXPECT_EQ(set.get_id(0), ids[0]); // ID of first element
    EXPECT_EQ(set.get_id(1), ids[2]); // ID of third element (now at position 1)
}

TEST(SparseSetTests, Test_ConstDirectIndexAccess) {
    ads::SparseSet<int> set;

    size_t id = set.insert(42);

    const ads::SparseSet<int> &const_set = set;
    EXPECT_EQ(const_set[0], 42);
    EXPECT_EQ(const_set.get_id(0), id);
}

TEST(SparseSetTests, Test_LargeScaleOperations) {
    ads::SparseSet<int> set;

    // Insert many elements
    std::vector<size_t> ids;
    const size_t count = 1000;

    for (size_t i = 0; i < count; i++) {
        ids.push_back(set.insert(static_cast<int>(i)));
    }

    EXPECT_EQ(set.size(), count);

    // Remove half of them
    for (size_t i = 0; i < count; i += 2) {
        set.remove(ids[i]);
    }

    EXPECT_EQ(set.size(), count / 2);

    // Insert new elements - should reuse IDs
    for (size_t i = 0; i < count / 2; i++) {
        size_t new_id = set.insert(static_cast<int>(i + count));
        EXPECT_LT(new_id, count); // Should be from reused IDs
    }

    EXPECT_EQ(set.size(), count);
}

TEST(SparseSetTests, Test_CustomType) {
    struct TestStruct {
        int value;
        std::string name;

        TestStruct(int v, const std::string &n) : value(v), name(n) {}

        bool operator==(const TestStruct &other) const {
            return value == other.value && name == other.name;
        }
    };

    ads::SparseSet<TestStruct> set;

    size_t id1 = set.insert(TestStruct(42, "test1"));
    size_t id2 = set.insert(TestStruct(100, "test2"));

    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.get(id1).value, 42);
    EXPECT_EQ(set.get(id1).name, "test1");
    EXPECT_EQ(set.get(id2).value, 100);
    EXPECT_EQ(set.get(id2).name, "test2");

    // Modify through reference
    set.get(id1).value = 200;
    EXPECT_EQ(set.get(id1).value, 200);
}

TEST(SparseSetTests, Test_EdgeCases) {
    ads::SparseSet<int> set;

    // Test boundary conditions
    EXPECT_FALSE(set.contains(std::numeric_limits<size_t>::max()));

    size_t id = set.insert(1);
    set.remove(id);

    // ID should be reusable
    size_t new_id = set.insert(2);
    EXPECT_EQ(new_id, id);
}

// Test iterator arithmetic and comparisons
TEST(SparseSetTests, Test_IteratorArithmetic) {
    ads::SparseSet<int> set;

    for (int i = 0; i < 5; i++) {
        set.insert(i);
    }

    auto it = set.begin();

    // Test arithmetic
    EXPECT_EQ(*(it + 2), 2);
    EXPECT_EQ(*(it + 4), 4);

    auto it2 = it + 3;
    EXPECT_EQ(it2 - it, 3);

    // Test comparisons
    EXPECT_LT(it, it2);
    EXPECT_LE(it, it2);
    EXPECT_GT(it2, it);
    EXPECT_GE(it2, it);

    // Test bracket operator
    EXPECT_EQ(it[2], 2);
    EXPECT_EQ(it2[0], 3);
}
