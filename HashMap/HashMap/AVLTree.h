#ifndef __AVLTREE__
#define __AVLTREE__


#include<algorithm>
#include<functional>
#include "Trait.h"
static const int AllowedImbalance = 1;
template<class T>
struct AVLnode {
	typedef typename is_pair<T>::Type KeyType;
	T Data;
	size_t Hash;
	AVLnode<T>* Parent;
	AVLnode<T>* Left;
	AVLnode<T>* Right;
	int height;
	void setHeight() {
		if (Left && Right)
			height = std::max(Left->height, Right->height) + 1;
		else if (!Left && !Right)
			height = 1;
		else
			height = Left ? Left->height + 1 : Right->height + 1;
		if (height <= 0)
			throw 1;
	}
	const KeyType& Key() { return  is_pair<T>::ExtractKey(Data); }
	static void clearTree(AVLnode * x) {
		if (!x)
			return;
		clearTree(x->Left);
		clearTree(x->Right);
		delete x;
	}
	AVLnode(T Data,
		size_t Hash,
		AVLnode<T>* Parent = nullptr,
		AVLnode<T>* Left = nullptr,
		AVLnode<T>* Right = nullptr)
		:Data(Data), Hash(Hash), Parent(Parent),
		Left(Left), Right(Right), height(1) {}
};
template<class T, class _Equal = std::equal_to<T>>
class AVLtree {
public:
	static AVLnode<T>* findMax(AVLnode<T>* Start);
	static AVLnode<T>* findMin(AVLnode<T>* Start);
	typedef typename is_pair<T>::Type KeyType;
	class keyEqual : public std::binary_function<KeyType, KeyType, bool> {
		friend class AVLtree<T, _Equal>;
		_Equal equal;
		keyEqual(_Equal equal) :equal(equal) {}
	public:
		bool operator()(const KeyType& x, const KeyType& y) {
			return equal(x, y);
		}
	};
	template<class U>
	class iterator {
	public:

		AVLnode<U> & operator*()const {
			return *_Pos;
		}
		AVLnode<U> * operator->()const {
			return _Pos;
		}
		AVLnode<U>* get()const { return _Pos; }
		void operator++() {
			if (_Pos->Right) {
				_Pos = AVLtree<T>::findMin(_Pos->Right);
			}
			else {
				while (_Pos->Parent && _Pos == _Pos->Parent->Right) {
					_Pos = _Pos->Parent;
				}
				if (_Pos == nullptr)
					return;
				_Pos = _Pos->Parent;
			}
		}
		void operator--() {
			if (_Pos->Left) {
				_Pos = AVLtree<T>::findMax(_Pos->Left);
			}
			else {
				while (_Pos->Parent && _Pos == _Pos->Parent->Left) {
					_Pos = _Pos->Parent;
				}
				if (_Pos == nullptr)
					return;
				_Pos = _Pos->Parent;
			}
		}
		void operator++(int) {
			if (_Pos->Right) {
				_Pos = findMin(_Pos->Right);
			}
			else {
				while (_Pos->Parent && _Pos == _Pos->Parent->Right) {
					_Pos = _Pos->Parent;
				}
				if (_Pos == nullptr)
					return;
				_Pos = _Pos->Parent;
			}
		}
		void operator--(int) {
			if (_Pos->Left) {
				_Pos = findMax(_Pos->Left);
			}
			else {
				while (_Pos->Parent && _Pos == _Pos->Parent->Left) {
					_Pos = _Pos->Parent;
				}
				if (_Pos == nullptr)
					return;
				_Pos = _Pos->Parent;
			}
		}
		bool isEnd() { return _Pos == nullptr; }
		iterator(AVLnode<U>* another) : _Pos(another) {}
		iterator(const iterator<U>& another) : _Pos(another._Pos) {}
	private:
		AVLnode<T>* findMin(AVLnode<U>* Start) {
			while (Start->Left)
				Start = Start->Left;
			return Start;
		}
		AVLnode<U>* findMax(AVLnode<T>* Start) {
			while (Start->Right)
				Start = Start->Right;
			return Start;
		}
		AVLnode<U>* _Pos;
	};
private:
	keyEqual isEqual;
	AVLnode<T>* Root;
	void rotateWithLeftChild(AVLnode<T>* & Node);
	void rotateWithRightChild(AVLnode<T>* & Node);
	void doubleRotateWithLeftChild(AVLnode<T>* & Node);
	void doubleRotateWithRightChild(AVLnode<T>* & Node);
	int getHeight(AVLnode<T>* Node) const;
	void balance(AVLnode<T>* & Node);
	void balanceTree(AVLnode<T>* & Node);

public:
	typedef iterator<T> Iterator;
	bool notGreater(const KeyType& x, const KeyType& y);
	AVLtree() : Root(nullptr), isEqual(_Equal()) {}
	AVLtree(_Equal equal) : Root(nullptr), isEqual(equal) {}
	~AVLtree() { AVLnode<T>::clearTree(Root); }
	void clear() { AVLnode<T>::clearTree(Root); Root = nullptr; }
	AVLnode<T>* search(const KeyType& key);
	AVLnode<T>* retRoot() { return Root; }
	AVLnode<T>* insert(const T& Node, size_t Hash, bool& isExist);
	AVLnode<T>* insNode(AVLnode<T>* Node);
	void travesal(AVLnode<T>* Node);
	bool delNode(const KeyType& Key);
	bool delNode(AVLnode<T>*& Todel, bool Delete);
};

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::rotateWithLeftChild(AVLnode<T>*& Node)
{
	AVLnode<T>* temp = Node->Left;
	Node->Left = temp->Right;
	temp->Right = Node;
	Node->setHeight();
	temp->setHeight();
	temp->Parent = Node->Parent;
	Node->Parent = temp;
	if (Node->Left)
		Node->Left->Parent = Node;
	Node = temp;
}

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::rotateWithRightChild(AVLnode<T>*& Node)
{
	AVLnode<T>* temp = Node->Right;
	Node->Right = temp->Left;
	temp->Left = Node;
	Node->setHeight();
	temp->setHeight();
	temp->Parent = Node->Parent;
	Node->Parent = temp;
	if (Node->Right)
		Node->Right->Parent = Node;
	Node = temp;
}

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::doubleRotateWithLeftChild(AVLnode<T>*& Node)
{
	rotateWithRightChild(Node->Left);
	rotateWithLeftChild(Node);
}

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::doubleRotateWithRightChild(AVLnode<T>*& Node)
{
	rotateWithLeftChild(Node->Right);
	rotateWithRightChild(Node);
}

template<class T, class _Equal>
inline int AVLtree<T, _Equal>::getHeight(AVLnode<T>* Node)const
{
	return Node ? Node->height : 0;
}

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::balance(AVLnode<T>* & Node)
{
	int imbalance = getHeight(Node->Left) - getHeight(Node->Right);
	if (imbalance > AllowedImbalance) {
		if (getHeight(Node->Left->Left) > getHeight(Node->Left->Right))
			rotateWithLeftChild(Node);
		else
			doubleRotateWithLeftChild(Node);
	}
	else if (imbalance < -AllowedImbalance) {
		if (getHeight(Node->Right->Right) > getHeight(Node->Right->Left))
			rotateWithRightChild(Node);
		else
			doubleRotateWithRightChild(Node);
	}
	Node->setHeight();
}

template<class T, class _Equal>
inline void AVLtree<T, _Equal>::balanceTree(AVLnode<T>*& Node)
{
	while (Node) {
		int BeforeFixed = Node->height;
		balance(Node->Parent ?
			Node->Parent->Left == Node ?
			Node->Parent->Left : Node->Parent->Right : Root);
		if (Node->height == BeforeFixed)
			return;
		else
			Node = Node->Parent;
	}
}

template<class T, class _Equal>
inline bool AVLtree<T, _Equal>::delNode(AVLnode<T>*& Todel, bool Delete)
{
	if (!Todel) return false;
	else {
		if (Todel->Left && Todel->Right) {
			AVLnode<T>* toPad = findMax(Todel->Left);
			std::swap(toPad->Data, Todel->Data);
			std::swap(toPad->Hash, Todel->Hash);
			Todel = toPad;
		}
		//Todel is root
		AVLnode<T>* parent = Todel->Parent;
		if (Todel == Root) {
			if (Todel->Left == nullptr && Todel->Right == nullptr) {
				Root = nullptr;
			}
			else if (Todel->Left) {
				Root = Todel->Left;
				Root->Parent = nullptr;
			}
			else {
				Root = Todel->Right;
				Root->Parent = nullptr;
			}
		}
		else {
			AVLnode<T>* & ParLorR = parent->Left == Todel ? parent->Left : parent->Right;
			if (Todel->Left == nullptr && Todel->Right == nullptr) {
				ParLorR = nullptr;
			}
			else if (Todel->Left) {
				ParLorR = Todel->Left;
				ParLorR->Parent = parent;
			}
			else {
				ParLorR = Todel->Right;
				ParLorR->Parent = parent;
			}
		}
		if (Delete)
			delete Todel;
		balanceTree(parent);
		return true;
	}
}

template<class T, class _Equal>
inline AVLnode<T>* AVLtree<T, _Equal>::findMax(AVLnode<T>* Start)
{
	while (Start->Right)
		Start = Start->Right;
	return Start;
}

template<class T, class _Equal>
inline AVLnode<T>* AVLtree<T, _Equal>::findMin(AVLnode<T>* Start)
{
	if (Start == nullptr)
		return Start;
	while (Start->Left)
		Start = Start->Left;
	return Start;
}

template<class T, class _Equal>
inline bool AVLtree<T, _Equal>::notGreater(const KeyType & x, const KeyType & y)
{
	return _notGreater<KeyType, decltype(has_less_or_equal_operator<KeyType>(0))>::compare(x, y);
}

template<class T, class _Equal>
inline AVLnode<T>* AVLtree<T, _Equal>::search(const KeyType& key)
{
	AVLnode<T>* temp = Root;
	while (temp && !isEqual(temp->Key(), key)) {
		temp = notGreater(key, temp->Key()) ? temp->Left : temp->Right;
	}
	return temp;
}

template<class T, class _Equal>
inline AVLnode<T>* AVLtree<T, _Equal>::insert(const T& Node, size_t Hash, bool& isExist)
{
	isExist = true;
	if (!Root) {
		Root = new AVLnode<T>(Node, Hash);
		return Root;
	}
	AVLnode<T>* temp = Root;
	const KeyType& TempKey = is_pair<T>::ExtractKey(Node);
	bool res;
	while (1) {
		if (isEqual(temp->Key(), TempKey)) {
			isExist = false;
			return temp;
		}
		else if ((res = notGreater(TempKey, temp->Key())) && temp->Left)
			temp = temp->Left;
		else if (!res && temp->Right)
			temp = temp->Right;
		else
			break;
	}
	AVLnode<T>* Toins = new AVLnode<T>(Node, Hash, temp);
	if (notGreater(TempKey, temp->Key())) {
		temp->Left = Toins;
	}
	else {
		temp->Right = Toins;
	}
	balanceTree(temp);
	return Toins;
}

template<class T, class _Equal>
inline AVLnode<T>* AVLtree<T, _Equal>::insNode(AVLnode<T>* Node)
{
	Node->Left = nullptr;
	Node->Right = nullptr;
	Node->height = 1;
	if (!Root) {
		Node->Parent = nullptr;
		Root = Node;
		return Root;
	}
	AVLnode<T>* temp = Root;
	const KeyType& TempKey = is_pair<T>::ExtractKey(Node->Data);
	bool res;
	while (1) {
		if (isEqual(temp->Key(), TempKey)) {
			return temp;
		}
		else if ((res = notGreater(TempKey, temp->Key())) && temp->Left)
			temp = temp->Left;
		else if (!res && temp->Right)
			temp = temp->Right;
		else
			break;
	}
	Node->Parent = temp;
	if (notGreater(TempKey, temp->Key())) {
		temp->Left = Node;
	}
	else {
		temp->Right = Node;
	}
	balanceTree(temp);
	return Node;
}

template<class T, class _Equal>
inline bool AVLtree<T, _Equal>::delNode(const KeyType& key)
{
	return delNode(search(key), true);
}


#endif // !__AVLTREE__