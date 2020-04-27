#ifndef BINARY_SEARCH_TREE_CLASS
#define BINARY_SEARCH_TREE_CLASS

#include "BinaryTree.h"

template <typename T>
class BinarySearchTree : public BinaryTree<T> {
public:
	BinarySearchTree() :BinaryTree<T>() {}
	BinarySearchTree(const T& content) :BinaryTree<T>(content) {}

	void Insert(const T& element) override { InternInsert(this->m_root, element); }
	void Delete(const T& element) override { InternDelete(this->m_root, element); }

	bool BinarySearch(T& retVal, const T& val) const { return InternBinarySearch(retVal, this->m_root, val); }

private:
	void InternInsert(std::unique_ptr<BTNode<T>>& root, const T& element);
	void InternDelete(std::unique_ptr<BTNode<T>>& root, const T& element);
	std::unique_ptr<BTNode<T>>& GetMin(std::unique_ptr<BTNode<T>>& root);
	bool InternBinarySearch(T& retVal, const std::unique_ptr<BTNode<T>>& root, const T& val) const;

};

template <typename T>
void BinarySearchTree<T>::InternInsert(std::unique_ptr<BTNode<T>>& root, const T& element) {

	if (root == nullptr) {
		root.reset(new BTNode<T>(element));
		//root = std::make_unique(BTNode<T>(element));
	}
	else {

		if (element > root->content)
			InternInsert(root->right, element);
		else if (element < root->content)
			InternInsert(root->left, element);

	}

}

template <typename T>
void BinarySearchTree<T>::InternDelete(std::unique_ptr<BTNode<T>>& root, const T& element) {

	if (root == nullptr)
		return;
	else if (element < root->content) {
		InternDelete(root->left, element);
		return;
	}
	else if (element > root->content) {
		InternDelete(root->right, element);
		return;
	}
	//else (The element was found)

	if (root->left != nullptr && root->right != nullptr) {		//If the root has 2 children
		//Temporary reference to the left-most child
		//of the right subtree
		auto& temp = GetMin(root->right);
		//Get the value from this node ^
		root->content = temp->content;
		//Delete the copied node
		InternDelete(temp, temp->content);
	}
	else if (root->left != nullptr) {							//If the root has left child
		//Temporary reference to the left child
		auto& LTemp = root->left;
		//Swap root with its only child
		root.swap(root->left);
		//Delete the root
		LTemp.release();
	}
	else if (root->right != nullptr) {							//If the root has right child
		//Temporary reference to the right child
		auto& RTemp = root->right;
		//Swap root with its only child
		root.swap(root->right);
		//Delete the root
		RTemp.release();
	}
	else {														//If the root has no children
		root.release();	//Delete the root
	}

}

template <typename T>
std::unique_ptr<BTNode<T>>& BinarySearchTree<T>::GetMin(std::unique_ptr<BTNode<T>>& root) {

	if (root->left != nullptr) {
		return GetMin(root->left);
	}

	return root;

}

template <typename T>
bool BinarySearchTree<T>::InternBinarySearch(T& retVal, const std::unique_ptr<BTNode<T>>& root, const T& val) const {

	if (root != nullptr) {

		if (root->content == val) {
			retVal = root->content;
			return true;
		}
		else if (val < root->content)
			return InternBinarySearch(retVal, root->left, val);
		else if (val > root->content)
			return InternBinarySearch(retVal, root->right, val);

	}

	return false;

}

#endif