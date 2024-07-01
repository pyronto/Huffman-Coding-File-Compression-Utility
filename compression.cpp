#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;

// Data structure for Huffman tree nodes
struct HuffmanNode {
    char data; // Character data (for leaf nodes)
    int frequency; // Frequency of the character
    HuffmanNode *left, *right; // Left and right children pointers

    HuffmanNode(char data, int frequency) {
        this->data = data;
        this->frequency = frequency;
        left = right = nullptr;
    }
};

// Comparison function for Huffman nodes (used in priority queue)
struct CompareNodes {
    bool operator()(HuffmanNode* lhs, HuffmanNode* rhs) {
        return lhs->frequency > rhs->frequency;
    }
};

// Function to build Huffman tree and return root node
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;

    // Create leaf nodes for each character and add to priority queue
    for (auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    // Build Huffman tree by combining nodes from the priority queue
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        int combinedFrequency = left->frequency + right->frequency;
        HuffmanNode* newNode = new HuffmanNode('\0', combinedFrequency);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    // Return the root of the Huffman tree
    return pq.top();
}

// Function to generate Huffman codes for characters
void generateHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr)
        return;

    // Leaf node (contains character data)
    if (root->left == nullptr && root->right == nullptr) {
        huffmanCodes[root->data] = code;
    }

    // Recursively generate codes for left and right subtrees
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Function to compress input file using Huffman coding
void compressFile(const string& inputFile, const string& outputFile) {
    // Step 1: Calculate character frequencies in the input file
    unordered_map<char, int> freqMap;
    ifstream input(inputFile, ios::binary);
    char ch;

    while (input.get(ch)) {
        freqMap[ch]++;
    }
    input.close();

    // Step 2: Build Huffman tree and generate Huffman codes
    HuffmanNode* root = buildHuffmanTree(freqMap);
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Step 3: Encode input file using Huffman codes and write to output file
    ofstream output(outputFile, ios::binary);
    input.open(inputFile, ios::binary);

    string encodedBits;
    while (input.get(ch)) {
        encodedBits += huffmanCodes[ch];
    }
    input.close();

    // Convert bit string to bytes and write to output file
    bitset<8> bits;
    for (size_t i = 0; i < encodedBits.size(); ++i) {
        bits[7 - i % 8] = (encodedBits[i] == '1');
        if ((i + 1) % 8 == 0 || i == encodedBits.size() - 1) {
            output.put(bits.to_ulong());
        }
    }
    output.close();

    cout << "File compressed successfully!" << endl;
}

int main() {
    string inputFile = "input.txt"; 
    string compressedFile = "compressed.bin";

    compressFile(inputFile, compressedFile);

    return 0;
}
