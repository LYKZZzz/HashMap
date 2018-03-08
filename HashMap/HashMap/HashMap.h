#ifndef __HASH_MAP__
#define __HASH_MAP__


#include <functional>
#include <list>
#include <vector>
#include <cstring>
#include "AVLTree.h"
class OutOfBound {};
template<class _Key, class _Value,
	class _Hash = std::hash<_Value>,
	class _Equal = std::equal_to<_Key>
>
class Hash_Map {
public:

private:
	typedef std::pair<_Key, _Value> Pair_type;
	typedef AVLtree<Pair_type, _Equal> Tree;
	class keyHash :public std::unary_function<_Key, size_t> {
		friend class Hash_Map<_Key, _Value, _Hash, _Equal>;
		_Hash hash;
		keyHash(_Hash hash) :hash(hash) {}
	public:
		size_t operator()(const _Key& x)const {
			return hash(x);
		}
	};
	size_t Size = 0;
	keyHash h;
	struct Bucket {
		Tree tree;
		size_t itemnum = 0;
		//int nextIndex = -1;
		//int preIndex = -1;
		Bucket(_Equal eq) : tree(eq) {}
		~Bucket() {
			tree.clear();
		}
	};
	struct HashTable {
		static const size_t iniBucketNum = 1 << 6;
		_Equal e;
		Bucket* Table;
		size_t BucketNum;
		size_t maxCap = iniBucketNum * 8;
		size_t size() { return BucketNum; }
		HashTable(_Equal e) : BucketNum(iniBucketNum), e(e) {
			Table = reinterpret_cast<Bucket*>(new char[sizeof(Bucket) * BucketNum]);
			Bucket* temp = Table;
			for (size_t i = 0; i < BucketNum; i++) {
				new (temp++) Bucket(e);
			}
		}
		void allocAndCopy(Bucket* old, Bucket* now, int length) {
			memcpy(now, old, sizeof(Bucket)*length);
		}
		void expand() {
			resize();
			rehash();
		}
		Bucket& operator[](int index) {
			if (index < 0 || index >= BucketNum)
				throw OutOfBound();
			return Table[index];
		}

		~HashTable() {
			for (size_t i = 0; i < BucketNum; i++) {
				(Table + i)->~Bucket();
			}
			free(Table);
		}
	private:
		void resize() {
			Bucket* newTable = reinterpret_cast<Bucket*>(new char[sizeof(Bucket) * BucketNum * 2]);
			allocAndCopy(Table, newTable, BucketNum);
			Bucket* temp = newTable + BucketNum;
			for (size_t i = BucketNum; i < 2 * BucketNum; i++) {
				new (temp++) Bucket(e);
			}
			free(Table);
			Table = newTable;
			BucketNum *= 2;
			maxCap *= 2;
		}
		void rehash() {
			std::list<AVLnode<Pair_type>*> rehashList;
			size_t reverse = BucketNum / 2;
			for (size_t i = 0; i < reverse; i++) {
				typename Tree::Iterator x(Tree::findMin(Table[i].tree.retRoot()));
				while (!x.isEnd()) {
					if (x->Hash%BucketNum != i) {
						rehashList.push_back(x.get());
					}
					x++;
				}
				for (auto k : rehashList) {
					Table[i].tree.delNode(k, false);
					Table[i].itemnum--;
					Table[i + reverse].tree.insNode(k);
					Table[i + reverse].itemnum++;
				}
				rehashList.clear();
			}
		}
	};
	HashTable hashTable;
public:
	Hash_Map(_Hash Hasher) : h(Hasher), hashTable(_Equal()) {}
	Hash_Map(_Hash Hasher, _Equal Equaler) : h(Hasher), hashTable(Equaler) {}
	Hash_Map() : h(_Hash()), hashTable(_Equal()) {}
	size_t size() { return Size; }
	size_t bucket() { return hashTable.size(); }
	bool isEmpty() { return Size == 0; }
	bool containsKey(_Key& key);
	void clear();
	void replace(const _Key& key, const _Value& now);
	Pair_type& insert(const _Key& key, const _Value& val);
	Pair_type& insert(const Pair_type& node);
	void erase(_Key& key);
	_Value& operator[](const _Key& key);
private:
	Pair_type & insert(size_t index, const _Key& key, const _Value& val, size_t Hash);
	Pair_type& insert(size_t index, const Pair_type& node, size_t Hash);
	Pair_type& search(const _Key& key, const size_t hash);
	bool containsKey(size_t index, _Key& key);
	void erase(size_t index, const _Key& key);
};

template<class _Key, class _Value, class _Hash, class _Equal>
inline bool Hash_Map<_Key, _Value, _Hash, _Equal>::containsKey(_Key & key)
{
	return containsKey(h(key) % hashTable.BucketNum, key);
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline void Hash_Map<_Key, _Value, _Hash, _Equal>::clear()
{
	for (size_t i = 0; i < hashTable.BucketNum; i++) {
		hashTable[i].itemnum = 0;
		hashTable[i].tree.clear();
	}
	Size = 0;
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline void Hash_Map<_Key, _Value, _Hash, _Equal>::replace(const _Key & key,const  _Value & now)
{
	this->operator[](key) = now;
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline std::pair<_Key, _Value> & Hash_Map<_Key, _Value, _Hash, _Equal>::insert(const _Key & key, const _Value & val)
{
	if (Size + 1 > hashTable.maxCap) {
		hashTable.expand();
	}
	size_t hash = h(key);
	return insert(hash % hashTable.BucketNum, key, val, hash);
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline std::pair<_Key, _Value> & Hash_Map<_Key, _Value, _Hash, _Equal>::insert(const Pair_type & node)
{
	if (Size + 1 > hashTable.maxCap) {
		hashTable.expand();
	}
	size_t hash = h(node.first);
	return insert(hash % hashTable.BucketNum, node, hash);
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline void Hash_Map<_Key, _Value, _Hash, _Equal>::erase(_Key & key)
{
	erase(h(key) % hashTable.BucketNum, key);
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline _Value & Hash_Map<_Key, _Value, _Hash, _Equal>::operator[](const _Key & key)
{
	size_t hash = h(key);
	if (hashTable[hash % bucket()].itemnum == 0) {
		if (Size + 1 > hashTable.maxCap) hashTable.expand();
		return insert(hash % bucket(), key, _Value(), hash).second;
	}
	else {
		return search(key, hash).second;
	}

}


template<class _Key, class _Value, class _Hash, class _Equal>
inline std::pair<_Key, _Value> & Hash_Map<_Key, _Value, _Hash, _Equal>::insert(size_t index, const _Key & key, const _Value & val, size_t Hash)
{
	return insert(index, std::make_pair(key, val), Hash);
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline std::pair<_Key, _Value> & Hash_Map<_Key, _Value, _Hash, _Equal>::insert(size_t index, const Pair_type & node, size_t Hash)
{
	bool x;
	std::pair<_Key, _Value> & ret = (hashTable[index].tree.insert(node, Hash, x)->Data);
	hashTable[index].itemnum += x ? 1 : 0;
	Size += x ? 1 : 0;
	return ret;
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline std::pair<_Key, _Value> & Hash_Map<_Key, _Value, _Hash, _Equal>::search(const _Key & key, const size_t hash)
{
	AVLnode<std::pair<_Key, _Value>>* ret = hashTable[hash%bucket()].tree.search(key);
	if (ret == nullptr) {
		if (Size + 1 > hashTable.maxCap) hashTable.expand();
		return insert(hash%bucket(), key, _Value(), hash);
	}
	return ret->Data;
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline bool Hash_Map<_Key, _Value, _Hash, _Equal>::containsKey(size_t index, _Key & key)
{
	return hashTable[index].tree.search(key) == nullptr;
}

template<class _Key, class _Value, class _Hash, class _Equal>
inline void Hash_Map<_Key, _Value, _Hash, _Equal>::erase(size_t index, const _Key & key)
{
	int x = hashTable[index].tree.delNode(key) ? 1 : 0;
	hashTable[index].itemnum -= x;
	Size -= x;
}


#endif // !__HASH_MAP__

