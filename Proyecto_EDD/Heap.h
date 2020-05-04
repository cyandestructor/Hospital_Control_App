#ifndef HEAP_CLASS
#define HEAP_CLASS

#include <vector>

enum class HeapType {
	MIN_H, MAX_H
};

template <typename T>
class Heap {
public:
	Heap(HeapType hType = HeapType::MAX_H) :m_data(1), m_type(hType) {}
	Heap(const T& content, HeapType hType = HeapType::MAX_H);
	bool isEmpty() const { return this->Size() == 0; }
	unsigned int Size() const { return m_data.size() - 1; }
	void Insert(const T& value);
	T DeleteRoot();
	const T& GetRoot() const;
	const T& GetLast() const;

	static void HeapSort(std::vector<T>& vector, HeapType heapType = HeapType::MAX_H);
	static void HeapSort(T array[], size_t size, HeapType heapType = HeapType::MAX_H);

private:
	T& GetRoot();
	T& GetLast();
	void HeapifyMax(int index);
	void HeapifyMin(int index);
	int ParentOf(int index) { return (int)(index / 2); }
	int LeftChildOf(int index) { return 2 * index; }
	int RightChildOf(int index) { return 2 * index + 1; }
	HeapType m_type;
	std::vector<T> m_data;

};

template <typename T>
Heap<T>::Heap(const T& content, HeapType hType) {

	m_data.resize(1);			//Create empty obj at index 0
	m_data.push_back(content);	//Create the root at index 1 with the content obj

	m_type = hType;				//Assign the heap type

}

template <typename T>
void Heap<T>::Insert(const T& value) {

	m_data.push_back(value);	//Insert at the end

	int aux = this->Size();		//The last position index
	while (aux > 1) {			//Repeat while it is not the root (index 1 marks the root of the heap)

		int parent = ParentOf(aux);

		//First, check the heap type
		//if it is max heap, check if the node is greater than its parent
		//if it is min heap, check if the node is lesser than its parent
		if ((m_type == HeapType::MAX_H && m_data[aux] > m_data[parent]) ||
			(m_type == HeapType::MIN_H && m_data[aux] < m_data[parent])) {

			std::swap(m_data[parent], m_data[aux]);		//Swap the data
			aux = parent;								//Now work with the parent

		}
		else {
			return;
		}

	}

}

template <typename T>
T Heap<T>::DeleteRoot() {

	T temp;

	if (!isEmpty()) {
		temp = GetRoot();					//Store the orig value in root
		std::swap(GetRoot(), GetLast());	//Swap root and last
		m_data.resize(m_data.size() - 1);	//Delete the last

		if (m_type == HeapType::MAX_H)
			HeapifyMax(1);
		else if (m_type == HeapType::MIN_H)
			HeapifyMin(1);

		return temp;
	}
	else {
		return m_data[0];	//Index 0 is an empty obj
	}

}

template <typename T>
void Heap<T>::HeapifyMax(int index) {

	unsigned int lchild = LeftChildOf(index), rchild = RightChildOf(index), heapify = index;

	if (lchild < m_data.size()) {
		if (m_data[lchild] > m_data[index])
			heapify = lchild;
	}

	if (rchild < m_data.size()) {
		if (m_data[rchild] > m_data[heapify])
			heapify = rchild;
	}

	if (heapify != index) {
		std::swap(m_data[heapify], m_data[index]);
		HeapifyMax(heapify);
	}

}

template <typename T>
void Heap<T>::HeapifyMin(int index) {

	unsigned int lchild = LeftChildOf(index), rchild = RightChildOf(index), heapify = index;

	if (lchild < m_data.size()) {
		if (m_data[lchild] < m_data[index])
			heapify = lchild;
	}

	if (rchild < m_data.size()) {
		if (m_data[rchild] < m_data[heapify])
			heapify = rchild;
	}

	if (heapify != index) {
		std::swap(m_data[heapify], m_data[index]);
		HeapifyMin(heapify);
	}

}

template <typename T>
T& Heap<T>::GetRoot() {

	if (!isEmpty()) {
		return m_data[1];
	}
	else {
		return m_data[0];	//index 0 is an empty obj
	}

}

template <typename T>
T& Heap<T>::GetLast() {

	if (!isEmpty()) {
		return m_data[this->Size()];
	}
	else {
		return m_data[0];	//index 0 is an empty obj
	}

}

template <typename T>
const T& Heap<T>::GetRoot() const {

	if (!isEmpty()) {
		return m_data[1];
	}
	else {
		return m_data[0];	//index 0 is an empty obj
	}

}

template <typename T>
const T& Heap<T>::GetLast() const {

	if (!isEmpty()) {
		return m_data[this->Size()];
	}
	else {
		return m_data[0];	//index 0 is an empty obj
	}

}

template <typename T>
void Heap<T>::HeapSort(std::vector<T>& vector, HeapType heapType) {

	Heap<T> heap(heapType);

	for (size_t i = 0; i < vector.size(); i++) {
		heap.Insert(vector[i]);
	}

	for (size_t i = 0; i < vector.size(); i++) {
		vector[i] = heap.DeleteRoot();
	}

}

template <typename T>
void Heap<T>::HeapSort(T array[], size_t size, HeapType heapType) {

	Heap<T> heap(heapType);

	for (size_t i = 0; i < size; i++) {
		heap.Insert(array[i]);
	}

	for (size_t i = 0; i < size; i++) {
		array[i] = heap.DeleteRoot();
	}

}

#endif