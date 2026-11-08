// HashTable.cpp
// Purpose: Implements the separate-chaining hash table and its hash function.
// Author:  Sushil Gautam
#include "HashTable.h"

#include "StringUtils.h"

// Polynomial multiplier used by the hash function.
static const std::size_t HASH_MULTIPLIER = 31;

const std::size_t HashTable::DEFAULT_CAPACITY;

// Allocates the empty buckets.
HashTable::HashTable(std::size_t capacity)
    : buckets_(capacity == 0 ? DEFAULT_CAPACITY : capacity),
      entries_(0),
      collisions_(0) {}

// Polynomial rolling hash over the uppercase key:
// h = (h * 31 + character) mod capacity, for each character in order.
std::size_t HashTable::hash(const std::string& key) const {
    const std::string normalized = normalizeFacilityId(key);
    std::size_t value = 0;
    for (std::string::size_type i = 0; i < normalized.size(); ++i) {
        const unsigned char character = static_cast<unsigned char>(normalized[i]);
        value = (value * HASH_MULTIPLIER + character) % buckets_.size();
    }
    return value;
}

// Adds a new key to its chain. Duplicates are detected first, so a rejected
// insertion never changes the entry or collision counts.
bool HashTable::insert(const std::string& key, std::size_t value) {
    const std::string normalized = normalizeFacilityId(key);
    if (contains(normalized)) {
        return false;
    }
    std::vector<Entry>& bucket = buckets_[hash(normalized)];
    if (!bucket.empty()) {
        ++collisions_;
    }
    Entry entry;
    entry.key = normalized;
    entry.value = value;
    bucket.push_back(entry);
    ++entries_;
    return true;
}

// Searches only the one chain that the key hashes to.
bool HashTable::find(const std::string& key, std::size_t& value) const {
    const std::string normalized = normalizeFacilityId(key);
    const std::vector<Entry>& bucket = buckets_[hash(normalized)];
    for (std::size_t i = 0; i < bucket.size(); ++i) {
        if (bucket[i].key == normalized) {
            value = bucket[i].value;
            return true;
        }
    }
    return false;
}

// Reports whether a key is stored, ignoring its value.
bool HashTable::contains(const std::string& key) const {
    std::size_t ignored = 0;
    return find(key, ignored);
}

// Returns the number of buckets.
std::size_t HashTable::capacity() const {
    return buckets_.size();
}

// Returns the number of stored keys.
std::size_t HashTable::entryCount() const {
    return entries_;
}

// Returns the number of accepted insertions into a nonempty bucket.
std::size_t HashTable::collisionCount() const {
    return collisions_;
}
