#ifndef _BINARY_TREE_CLASS
#define _BINARY_TREE_CLASS

#include <memory>
#include <iostream>
#include <functional>

enum class ExecOrder {
	PREORDER, INORDER, POSTORDER
};

template <typename T>
class BTNode {
public:
	BTNode() :left(nullptr), right(nullptr) {}
	BTNode(const T& content) : content(content), left(nullptr), right(nullptr) {}

	T content;
	std::unique_ptr<BTNode<T>> left;
	std::unique_ptr<BTNode<T>> right;

};

template <typename T>
class BinaryTree {
public:
	BinaryTree() :m_root(nullptr) {}
	BinaryTree(const T& content) : m_root(new BTNode<T>(content)) {}

	virtual void Insert(const T& element) {}
	virtual void Delete(const T& element) {}

	const T& GetData() const;
	unsigned long Size() const { return InternSize(this->m_root); }

	void ExecutePreorder(const std::function<void(T&)>& function) {
		InternExecutePreorder(this->m_root, function);
	}
	void ExecuteInorder(const std::function<void(T&)>& function) {
		InternExecuteInorder(this->m_root, function);
	}
	void ExecutePostorder(const std::function<void(T&)>& function) {
		InternExecutePostorder(this->m_root, function);
	}

	void Write(std::ostream& os);
	void WriteToFile(const std::wstring& filePath);
	void Read(std::istream& is);
	void ReadFromFile(const std::wstring& filePath);

	bool IsEmpty() const { return m_root == nullptr; }
	bool IsLeaf() const;
protected:
	std::unique_ptr<BTNode<T>> m_root;
private:
	unsigned long InternSize(const std::unique_ptr<BTNode<T>>& root) const;
	void InternExecutePreorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function);
	void InternExecuteInorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function);
	void InternExecutePostorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function);

};

template <typename T>
void BinaryTree<T>::Write(std::ostream& os) {

	unsigned long size = this->Size();

	os.write(reinterpret_cast<char*>(&size), sizeof(unsigned long));

	this->ExecuteInorder([&](T& obj) {
		obj.Write(os);
		});

}

template <typename T>
void BinaryTree<T>::WriteToFile(const std::wstring& filePath) {

	ofstream file(filePath, std::ios::out | std::ios::trunc | std::ios::binary);

	if (file.is_open()) {
		this->Write(file);
		file.close();
	}

}

template <typename T>
void BinaryTree<T>::ReadFromFile(const std::wstring& filePath) {

	ifstream file(filePath, std::ios::in | std::ios::binary);

	if (file.is_open()) {
		this->Read(file);
		file.close();
	}

}

template <typename T>
void BinaryTree<T>::Read(std::istream& is) {

	unsigned long size = 0;
	is.read(reinterpret_cast<char*>(&size), sizeof(unsigned long));

	T temp;
	for (unsigned long i = 0; i < size; i++) {
		temp.Read(is);
		this->Insert(temp);
	}

}

template <typename T>
const T& BinaryTree<T>::GetData() const {

	if (m_root != nullptr) {
		return m_root->content;
	}
	else {
		//throw exception
	}

}

template <typename T>
bool BinaryTree<T>::IsLeaf() const {

	if (m_root != nullptr) {
		return (m_root->left == nullptr && m_root->right == nullptr);
	}
	else {
		return true;
	}
}

template <typename T>
unsigned long BinaryTree<T>::InternSize(const std::unique_ptr<BTNode<T>>& root) const {

	if (root == nullptr)
		return 0;
	else
		return 1 + InternSize(root->left) + InternSize(root->right);

}

template <typename T>
void BinaryTree<T>::InternExecutePreorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function) {

	if (node != nullptr) {
		function(node->content);
		InternExecutePreorder(node->left, function);
		InternExecutePreorder(node->right, function);
	}

}

template <typename T>
void BinaryTree<T>::InternExecuteInorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function) {

	if (node != nullptr) {
		InternExecuteInorder(node->left, function);
		function(node->content);
		InternExecuteInorder(node->right, function);
	}

}

template <typename T>
void BinaryTree<T>::InternExecutePostorder(std::unique_ptr<BTNode<T>>& node, const std::function<void(T&)>& function) {

	if (node != nullptr) {
		InternExecutePostorder(node->left, function);
		InternExecutePostorder(node->right, function);
		function(node->content);
	}

}

#endif