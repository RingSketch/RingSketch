#include <cstdlib>

struct Node {
    double key;
    int priority;
    int size;
    Node* left;
    Node* right;

    Node(double key) : key(key), priority(rand()), size(1), left(nullptr), right(nullptr) {}
};

int getSize(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->size;
}

void updateSize(Node* node) {
    if (node != nullptr) {
        node->size = getSize(node->left) + getSize(node->right) + 1;
    }
}

Node* rotateLeft(Node* root) {
    Node* newRoot = root->right;
    root->right = newRoot->left;
    newRoot->left = root;
    updateSize(root);
    updateSize(newRoot);
    return newRoot;
}

Node* rotateRight(Node* root) {
    Node* newRoot = root->left;
    root->left = newRoot->right;
    newRoot->right = root;
    updateSize(root);
    updateSize(newRoot);
    return newRoot;
}

Node* insert(Node* root, double key) {
    if (root == nullptr) {
        return new Node(key);
    }
    if (key <= root->key) {
        root->left = insert(root->left, key);
        if (root->left->priority < root->priority) {
            root = rotateRight(root);
        }
    } else {
        root->right = insert(root->right, key);
        if (root->right->priority < root->priority) {
            root = rotateLeft(root);
        }
    }
    updateSize(root);
    return root;
}

Node* erase(Node* root, double key) {
    if (root == nullptr) {
        return nullptr;
    }
    if (key < root->key) {
        root->left = erase(root->left, key);
    } else if (key > root->key) {
        root->right = erase(root->right, key);
    } else {
        if (root->left == nullptr) {
            Node* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            Node* temp = root->left;
            delete root;
            return temp;
        } else {
            if (root->left->priority < root->right->priority) {
                root = rotateRight(root);
                root->right = erase(root->right, key);
            } else {
                root = rotateLeft(root);
                root->left = erase(root->left, key);
            }
        }
    }
    updateSize(root);
    return root;
}

int rank(Node* root, double key) {
    if (root == nullptr) {
        return 0;
    }
    if (key == root->key) {
        return getSize(root->left) + 1;
    }
    if (key < root->key) {
        return rank(root->left, key);
    }
    return getSize(root->left) + 1 + rank(root->right, key);
}
