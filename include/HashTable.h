// HashTable.h
// Purpose: Custom hash table keyed by facility ID, using separate chaining.
//          Each key maps to the record's position in the facility list.
// Author:  Sushil Gautam
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstddef>
#include <string>
#include <vector>

class HashTable {
public:
    // Number of buckets used unless another capacity is requested.
    static const std::size_t DEFAULT_CAPACITY = 101;

    // Creates an empty table. A capacity of 0 falls back to the default.
    explicit HashTable(std::size_t capacity = DEFAULT_CAPACITY);

    // Inserts a key and value. Returns false, changing nothing, if the key
    // already exists. Counts a collision if the target bucket is nonempty.
    bool insert(const std::string& key, std::size_t value);

    // Looks up a key. Returns true and sets value if found.
    bool find(const std::string& key, std::size_t& value) const;

    // Returns true if the key is already stored.
    bool contains(const std::string& key) const;

    // Calculates the bucket index for a key.
    std::size_t hash(const std::string& key) const;

    // Returns the number of buckets (HASH-7 diagnostic).
    std::size_t capacity() const;

    // Returns the number of stored keys (HASH-7 diagnostic).
    std::size_t entryCount() const;

    // Returns the number of accepted insertions into a nonempty bucket
    // (HASH-7 diagnostic).
    std::size_t collisionCount() const;

private:
    // One stored key and its value.
    struct Entry {
        std::string key;
        std::size_t value;
    };

    std::vector<std::vector<Entry> > buckets_;  // each bucket is a chain
    std::size_t entries_;
    std::size_t collisions_;
};

#endif
