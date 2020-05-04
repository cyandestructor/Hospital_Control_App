#ifndef _LIST_CLASS_
#define _LIST_CLASS_

#include <fstream>
#include <functional>

template <typename T>
class Node {
public:
	Node() { prev = next = nullptr; }
	Node(T content) : content(content) { prev = next = nullptr; }
	Node* prev;
	T content;
	Node* next;
};

template <typename T>
class List {

public:
	//Constructors & Destructor
	List();
	List(T element);
	List(const List& other);
	~List();
	//Methods
	void Insert(T newElement, unsigned int position);
	T Delete(unsigned int position);
	void Clear();
	void Push(T newElement);
	T Pop();
	T PopBack();
	void Enqueue(T newElement);
	T Dequeue();
	T& GetElement(unsigned int index);
	const T& GetElement(unsigned int index) const;
	void ForEach(void(*doFunc)(T&));
	void ForEach(const std::function<void(T&)>& doFunc);
	T* GetArray();
	bool ReadFromFile(const std::wstring& filePath);
	bool WriteToFile(const std::wstring& filePath);
	template <typename U> T& Search(const U& key, bool (*condition)(const T&, const U&));
	void QuickSort();
	bool IsEmpty() const { return m_size == 0; }
	unsigned int Size() const { return m_size; }
	//Operators
	const List& operator=(const List& other);
	T& operator[](unsigned int index) { return GetElement(index); }
	const T& operator[](unsigned int index) const { return GetElement(index); }

private:
	Node<T> *m_head, *m_tail;
	unsigned int m_size;

	void InternQuickSort(T arr[], int l, int r);

};

template <typename T>
List<T>::List() {

	m_head = m_tail = nullptr;
	m_size = 0;

}

template <typename T>
List<T>::List(T element) {

	m_head = m_tail = nullptr;
	m_size = 0;
	Push(element);

}

template <typename T>
List<T>::List(const List& other) {

	m_head = m_tail = nullptr;
	m_size = 0;

	if (other.m_size != 0) {
		Node<T>* aux = other.m_head;
		for (unsigned int i = 0; i < other.m_size; i++) {
			Push(aux->content);
			aux = aux->next;
		}
	}

}

template <typename T>
List<T>::~List() {

	this->Clear();

}

template <typename T>
void List<T>::Push(T newElement) {

	Node<T>* newNode = new Node<T>;

	if (newNode != nullptr) {

		newNode->content = newElement;

		if (IsEmpty()) {
			newNode->prev = nullptr;
			newNode->next = nullptr;
			m_head = m_tail = newNode;
		}
		else {
			newNode->prev = nullptr;
			newNode->next = m_head;

			m_head->prev = newNode;

			m_head = newNode;
		}

		m_size++;

	}

}

template <typename T>
T List<T>::Pop() {

	T popElement;

	if (!IsEmpty()) {
		popElement = m_head->content;
		m_head->next->prev = nullptr;
		Node<T>* aux = m_head;
		m_head = m_head->next;
		delete aux;
		m_size--;
	}

	return popElement;

}

template <typename T>
T List<T>::PopBack() {

	T popElement;

	if (!IsEmpty()) {
		popElement = m_tail->content;
		m_tail->prev->next = nullptr;
		Node<T>* aux = m_tail;
		m_tail = m_tail->prev;
		delete aux;
		m_size--;
	}

	return popElement;

}

template <typename T>
void List<T>::Enqueue(T newElement) {

	Node<T>* newNode = new Node<T>;

	if (newNode != nullptr) {

		newNode->content = newElement;

		if (IsEmpty()) {
			newNode->prev = nullptr;
			newNode->next = nullptr;
			m_head = m_tail = newNode;
		}
		else {
			newNode->prev = m_tail;
			newNode->next = nullptr;

			m_tail->next = newNode;

			m_tail = newNode;
		}

		m_size++;

	}

}

template <typename T>
T List<T>::Dequeue() {

	return Pop();

}

template <typename T>
bool List<T>::ReadFromFile(const std::wstring& filePath) {

	ifstream file(filePath, ios::in | ios::binary);

	if (file.good()) {
		T temp;
		//Read the total of written objects
		unsigned int size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
		for (unsigned int i = 0; i < size; i++) {
			temp.Read(file);
			Push(temp);
		}

		file.close();
		return true;
	}

	return false;

}

template <typename T>
bool List<T>::WriteToFile(const std::wstring& filePath) {

	ofstream file(filePath, ios::out | ios::trunc | ios::binary);
	
	if (file.good()) {
		Node<T>* aux = m_head;
		file.write(reinterpret_cast<char*>(&m_size), sizeof(unsigned int)); //Write the number of elements
		for (unsigned int i = 0; i < m_size; i++) {
			(aux->content).Write(file);
			aux = aux->next;
		}
		file.close();
		return true;
	}

	return false;

}

template <typename T>
template <typename U>
T& List<T>::Search(const U& key, bool (*condition)(const T&, const U&)) {

	static T noexist;

	Node<T>* aux = m_head;
	while (aux != nullptr) {
		if (condition(aux->content, key))
			return aux->content;

		aux = aux->next;
	}

	return noexist;

}

template <typename T>
void List<T>::Insert(T newElement, unsigned int position) {

	if (IsEmpty() || position == 0) {
		Push(newElement);
	}
	else if (position >= m_size - 1) {
		Enqueue(newElement);
	}
	else {

		Node<T>* aux = m_head;
		Node<T>* newNode = new Node<T>;

		newNode->prev = nullptr;
		newNode->content = newElement;
		newNode->next = nullptr;

		for (unsigned int i = 0; i < m_size; i++) {

			if (i == position) {
				aux->prev->next = newNode;
				newNode->prev = aux->prev;
				newNode->next = aux;
				break;
			}

			aux = aux->next;

		}

		m_size++;

	}

}

template <typename T>
T List<T>::Delete(unsigned int position) {

	T deleteElement;

	if (!IsEmpty()) {

		if (position == 0) {
			deleteElement = Pop();
		}
		else if (position >= m_size - 1) {
			deleteElement = PopBack();
		}
		else {
			Node<T>* aux = m_head;

			for (unsigned int i = 0; i < m_size; i++) {

				if (i == position) {
					aux->prev->next = aux->next;
					aux->next->prev = aux->prev;
					deleteElement = aux->content;
					delete aux;
					m_size--;
					break;
				}

				aux = aux->next;

			}

		}

	}

	return deleteElement;

}

template <typename T>
void List<T>::Clear() {

	if (!IsEmpty()) {
		Node<T>* aux = m_tail, * temp = nullptr;
		while (aux != nullptr) {
			temp = aux;
			aux = aux->prev;
			delete temp;
		}
		m_head = m_tail = nullptr;
		m_size = 0;
	}

}

template <typename T>
const List<T>& List<T>::operator=(const List<T>& other) {

	if (this != &other) {
		this->Clear();
		if (other.m_size != 0) {
			Node<T>* aux = other.m_head;
			for (unsigned int i = 0; i < other.m_size; i++) {
				Push(aux->content);
				aux = aux->next;
			}
		}
	}

	return *this;

}

template <typename T>
T& List<T>::GetElement(unsigned int index) {

	if (index < m_size) {

		Node<T>* aux = m_head;

		for (unsigned int i = 0; i < index; i++) {
			aux = aux->next;
		}

		return aux->content;

	}
	else {
		//throw exception
	}

}

template <typename T>
const T& List<T>::GetElement(unsigned int index) const {

	if (index < m_size) {

		Node<T>* aux = m_head;

		for (unsigned int i = 0; i < index; i++) {
			aux = aux->next;
		}

		return aux->content;

	}
	else {
		//throw exception
	}

}

template<typename T>
void List<T>::ForEach(void(*doFunc)(T&)) {

	Node<T>* aux = m_head;
	for (unsigned int i = 0; i < m_size; i++) {
		doFunc(aux->content);
		aux = aux->next;
	}

}

template <typename T>
void List<T>::ForEach(const std::function<void(T&)>& doFunc) {

	Node<T>* aux = m_head;
	for (unsigned int i = 0; i < m_size; i++) {
		doFunc(aux->content);
		aux = aux->next;
	}

}

template <typename T>
T* List<T>::GetArray() {

	T* listArray = nullptr;

	if (!this->IsEmpty()) {
		listArray = new T[this->m_size];

		Node<T>* aux = this->m_head;
		for (unsigned int i = 0; i < this->m_size; i++) {
			listArray[i] = aux->content;
			aux = aux->next;
		}

	}

	return listArray;

}

template <typename T>
void List<T>::QuickSort() {

	size_t size = this->m_size;

	if (!this->IsEmpty()) {
		T* arr = new T[size];	//Create an array with the number of elements of the list

		//Add the elements of the list to the array
		Node<T>* aux = this->m_head;
		for (size_t i = 0; i < size; i++) {
			arr[i] = aux->content;
			aux = aux->next;
		}

		this->InternQuickSort(arr, 0, size);	//Call the intern quicksort to sort the array

		this->Clear();	//Clear the list
		//Then insert the elements in the correct order
		for (size_t i = 0; i < size; i++) {
			this->Enqueue(arr[i]);
		}

		delete[] arr;

	}

}

template <typename T>
void List<T>::InternQuickSort(T arr[], int l, int r) {

	unsigned int i = l,
		j = r,
		pivot = (int)((l + r) * 0.5);

	while (i <= j) {
		while (arr[i] < arr[pivot])	//Find an element in the left side that is grater than the pivot
			i++;

		while (arr[j] > arr[pivot])	//Find an element in the right side that is lesser than the pivot
			j--;

		if (i <= j) {
			std::swap(arr[i], arr[j]);
			//continue
			i++;
			i--;
		}
	}

	if (l < j)
		this->InternQuickSort(arr, l, j);
	
	if (i < r)
		this->InternQuickSort(arr, i, r);

}

#endif