/**
 * Author:                  Paul Devlin
 * Assignment Title:        Huffman Encoding
 * Assignment Description:  Encode and decode in a huffman
 *                          encoding manner
 * Due Date:                2/19/2023
 * Date Created:            2/12/2023
 * Date Last Modified:      2/19/2023
 */

/**
 * Data Abstraction:
     * 	node *encodingRoot will be the root of the encoded huffman tree
     *  ifstream input; opens an input stream
     *  ofstream output; opens an output stream

     *  int secret = -5643 unique id for my encoded files

     *  int uniqueChar = 0, letterCount = 0, encodingLength = 0
            will tell if the file can be compressed
     *  char data for iterating and recording the files data
     *  char EOD = 127 for ending a file created

     *  priority_queue<node *, vector<node *>, compare> dataQueue
            for creating the huffman tree
     *  map<char, int> dataMap;
            will contain the character and its associated weight
     *  map<char, string> encodingMap; will contain the character
            and its associated encoding
     *  toBin b; toBin object to help with encoding and transfer of bits

        //decoding
     *  node *decodingRoot; will be the root of the decoding tree
     *  bool EODRead = false; will tell the program to stop reading data
     *  int uniqueChar2, secret2 num; will id the file when opened for decoding
     *  map<char, int> decDataMap; will contain the character and its
            associated decoding
     *  map<char, string> decodingMap; will contain the character and its
            associated decoding
     *  priority_queue<node *, vector<node *>, compare> decDataQueue; for
            re-creating a huffman tree
 * Input:
 * 		The program reads in format of "-huff/-unhuff <source> <destination>"
 * 		    and opens streams to the appropriate file
 * 		The first input "-huff/-unhuff" dictates how the program will act
 * 		    either encoding or decoding
 * Process:
 * 		-huff: The program will open an input file and read through once at
  		    a character-by-character pace. It will determine if the file can be
  		    compressed. If so then the file is compressed using the greedy
  		    huffman encoding algorithm. If not a message is sent to the user
  		    saying the file is already compressed. The new file will only
  		    be able to decoded by this programs -unhuff algorithm.
 * 		-unhuff: The program opens an input stream and dteremines if this
            program had encode the file. If so the huffman tree is replicated
            and the encoded message is then uncompressed into the destination
            parameter. If not then the program tells the user so, and the
            program finishes.
 * Output:
 * 		-huff: To the given out file a unique encoding that can only be decoded
            by this program's algorithms is sent.
 *       -unhuff: An uncompressed version of a uniquely encoded message
            provided by this program is sent to the output file enterd as the
            destination argument.
 * Assumptions:
 * 		It is assumed that the user does not want to encrypt the data for
   		    safety
 * 		It is assumed that the user inputs valid input
 * 		It is assumed that the users uncompressed file has a standard file
            delimiter that they anticipate to have the encoding end by.
 **/

#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <cstring>
#include <map>
#include <stack>

using namespace std;


struct node {
    int count;
    char c;
    node *left, *right;
//************************************************************
// description: node constructor for a huffman tree          *
// return: none                                              *
// precondition: none                                        *
// postcondition: a node with no children is crated          *
//                                                           *
//************************************************************
    node(node *lptr = nullptr, node *rptr = nullptr) {
        count = 0;
        c = '\0';
        left = lptr;
        right = rptr;
    }
};

class compare {
public:
//************************************************************
// description: comparison for huffman tree nodes            *
// return: bool                                              *
// precondition: l and r are valid                           *
// postcondition: returns a value that will build a minHeap  *
//                in a priority_Queue                        *
//                                                           *
//************************************************************
    bool operator()(node *l, node *r) {
        return l->count > r->count;
    }
};

struct toBin {
    int buffSize = 0;
    char buffer = '\0';

//************************************************************
// description: converts encodings to bits in a char buffer  *
// return: void                                              *
// precondition: b and os are valid                          *
// postcondition: if the buffer it calls clearBuffer()       *
//                                                           *
//************************************************************
    void toBuffer(bool b, ostream &os) {
        //shift left
        buffer <<= 1;
        //will put the value of b (1 or 0) at the end of buffer
        buffer |= b;
        buffSize++;
        if (buffSize == 8) {
            clearBuffer(os);
        }
    }

//************************************************************
// description: make the buffer puke into os                 *
// return: void                                              *
// precondition: os is valid                                 *
// postcondition: the buffer is refreshed and ready to eat   *
//                more yummy bits                            *
//                                                           *
//************************************************************
    void clearBuffer(ostream &os) {
        if (buffSize != 0) {
            buffer <<= ((unsigned int) (8 - buffSize));
            os << buffer;
            buffSize = 0;
            buffer = '\0';
        }
    }

//************************************************************
// description: decodes the 8-bit chunks from an encoded file*
// return: bool                                              *
// precondition: in is a valid char                          *
// postcondition: will tell the algorithm the path to take   *
//                when traversing the recreated huffman tree *
//                                                           *
//************************************************************
    bool decodeBuffer(char &in) {
        if (buffSize == 0) {
            buffer = (unsigned) in;
            buffSize = 8;
        }
        bool flag = (buffer & (1 << 7));
        buffer <<= 1;
        buffSize--;
        return flag;
    }
};


//************************************************************
// description: created the paths in 1/0s to a leaf node     *
// return: void                                              *
// precondition: n, path, map are all valid                  *
// postcondition: a string which is the path to the leaf from*
//                the root is crated                         *
//                                                           *
//************************************************************
//n is the root of the huffman tree
//a printPreorder will give the paths
void encode(node *n, string path, map<char, string> &map) {
    if (n == nullptr) {
        return;
    }
    //leaf found
    if ((!n->left && !n->right)) {
        map[n->c] = path;
    }
    encode(n->left, path + "0", map);
    encode(n->right, path + "1", map);
}

/*****************************************************************************/
/**
 * main
 */
/*****************************************************************************/
int main(int argc, char *args[]) {
    /**
     * steps to complete the encoding "-huff" steps
     *   1) count how many times each character (8-bit chunk) occurs in the
     *      file. dont need to use char variables in the program can use int.
     *
     *   2) build a huffman tree from these counts. One node per character w/
     *      weights according to there frequencies, insert them into a priority
     *      queue. Choose minimal nodes join them as the children of a new node
     *      insert the new node into the priority queue. New node has
     *      (weight == the weight of its children) continue until there is only
     *      one node left in the queue. The node left is the root of the tree.
     *
     *   3) Create a table/map of characters(8-bit chunks) to codings. The
     *      encodings are the paths of the leafs which should be the characters
     *      entoe (going left = 0, going right = 1). This will make retrieval
     *      later much easier.
     *
     *   4) Read the input file a second time. For each 8-bit chunk read,
     *      write the encoding to the character obtained by the map to the
     *      compressed file.
     *
     **/

    /**
     * "-unhuff" steps
     *   1) make sure the file that is being uncompressed
     *      was compressed by my code. make the program
     *      un-crash-able. important to the grading
     *      criteria.
     *   2) come up with a magic number at the beginning of
     *      the compressed file. can be any number of bits.
     *      the decompression reads in N-bits (N == magic)
     *      if the bits dont add up to the magic then dont
     *      continue.
     **/

    /**
     * "Pseudo-EOF character"
     * make it the magic number the magic number will be
     * the furthest right one can get on the huffman tree.
     * but the wont literally be the same in the binary file
     * EOD will be the encoding of magic number
     **/

    //DATA ABSTRACTION
    node *encodingRoot;
    //will be the root of the encoded huffman tree
    ifstream input;
    ofstream output;

    int secret = -5643;
    //unique id for my encoded files

    int uniqueChar = 0, letterCount = 0, encodingLength = 0;
    //will tell if the file can be compressed
    char data;//for iterating and recording the files data
    char EOD = 127; //for ending a file created

    priority_queue<node *, vector<node *>, compare> dataQueue;
    //for creating the huffman tree
    map<char, int> dataMap;
    //will contain the character and its associated weight
    map<char, string> encodingMap;
    //will contain the character and its associated encoding
    toBin b;
    //toBin object to help with encoding and transfer of bits

    //decoding
    node *decodingRoot;
    //will be the root of the decoding tree
    bool EODRead = false;
    //will tell the program to stop reading data
    int uniqueChar2, secret2, num;
    //will id the file when opened for decoding
    map<char, int> decDataMap;
    //will contain the character and its associated decoding
    map<char, string> decodingMap;
    //will contain the character and its associated decoding
    priority_queue<node *, vector<node *>, compare> decDataQueue;
    //for re-creating a huffman tree



/*****************************************************************************/
/**
 * -huff command PROCESS
 */
/*****************************************************************************/
    if (strcmp(args[1], "-huff") == 0) {
        input.open(args[2], ios::in | ios::binary);
        output.open(args[3], ios::out | ios::binary);
        if (!input.is_open()) {
            cout << "Input file not found." << endl;
        } else if (!output.is_open()) {
            cout << "Output file not found." << endl;
        } else {
            // set the file position indicators to the beginning of the file
            input.seekg(0, ios::beg);


            //initial read through
            while (input.read((char *) &data, sizeof(char))) {
                dataMap[data]++;
                letterCount++;
            }
            dataMap[EOD]++;
            uniqueChar = dataMap.size();

            input.close();

            //finding the length of the new file in bits
            for (auto i: dataMap) {
                encodingLength += i.second * encodingMap[i.first].length();
            }

            if ((uniqueChar * 5 + 8 + encodingLength) > letterCount) {
                cout << "File will not compress" << endl;
            } else {

                //create the initial queue with all the character and values
                // added
                //-note: there is no huffman tree yet.
                for (auto i: dataMap) {
                    node *leaf = new node();
                    leaf->c = i.first;
                    leaf->count = i.second;
                    dataQueue.push(leaf);
                }

                //creating the huffman tree
                while (dataQueue.size() > 1) {
                    //combine the two lowest frequencies together
                    node *left = dataQueue.top();
                    dataQueue.pop();
                    node *right = dataQueue.top();
                    dataQueue.pop();
                    node *internal = new node(left, right);
                    internal->count = left->count + right->count;
                    dataQueue.push(internal);
                }

                //the top of the priority queue is the root of the huffman tree
                encodingRoot = dataQueue.top();

                //encoding the values on the values on the huffman tree.
                //a right on the tree == 1, left on the tree == 0.
                //will also have to add an end of data character. -probably be
                // a bunch of 1's-
                //encode(root, encodingMap);
                encode(encodingRoot, "", encodingMap);
                //resets the flags of the file
                input.open(args[2], ios::in | ios::binary);
                //reading the file a second time to write the encodings
                input.seekg(0, ios::beg);

                /**
                 * OUTPUT
                 */
                //METADATA
                output.write((char *) &secret, sizeof(int));
                output.write((char *) &uniqueChar, sizeof(int));
                for (auto x: dataMap) {
                    output.write((char *) &x.first, sizeof(char));
                    output.write((char *) &x.second, sizeof(int));
                }

                //Second read through to send compressed message to output
                while (input.read((char *) &data, sizeof(char))) {
                    for (char i: encodingMap[data]) {
                        b.toBuffer(i == '1', output);
                    }
                }

                for (char i: encodingMap[EOD]) {
                    b.toBuffer(i == '1', output);
                }
                b.clearBuffer(output);

                input.close();
                output.close();
            }
        }
/*****************************************************************************/
/**
 * -unhuff command PROCESS
 */
/*****************************************************************************/
    } else if (strcmp(args[1], "-unhuff") == 0) {
        input.open(args[2], ios::in | ios::binary);
        output.open(args[3], ios::out | ios::binary);
        if (!input.is_open()) {
            cout << "Input file not found." << endl;
        } else if (!output.is_open()) {
            cout << "Output file not found." << endl;
        } else {

            input.seekg(0, ios::beg);
            input.read((char *) &secret2, sizeof(int));
            if (secret2 != secret) {
                cout << "Input file was not Huffman Encoded." << endl;
            } else {

                input.read((char *) &uniqueChar2, sizeof(int));
                for (int i = 0; i < uniqueChar2; i++) {
                    input.read((char *) &data, sizeof(char));
                    input.read((char *) &num, sizeof(int));
                    decDataMap[data] = num;
                }

                for (auto i: decDataMap) {
                    node *leaf = new node();
                    leaf->c = i.first;
                    leaf->count = i.second;
                    decDataQueue.push(leaf);
                }

                while (decDataQueue.size() > 1) {
                    //combine the two lowest frequencies together
                    node *left = decDataQueue.top();
                    decDataQueue.pop();
                    node *right = decDataQueue.top();
                    decDataQueue.pop();
                    node *internal = new node(left, right);
                    internal->count = left->count + right->count;
                    decDataQueue.push(internal);
                }

                decodingRoot = decDataQueue.top();
                encode(decodingRoot, "", decodingMap);


                //reverse the data map so that the encodings are the keys to
                // make traversal easier
                node *traversal = decodingRoot;
                //original file recreation
                /**
                 * OUTPUT
                 */
                while (!EODRead) {
                    input.read((char *) &data, sizeof(char));
                    for (int i = 0; i < 8 && !EODRead; i++) {
                        if (b.decodeBuffer(data)) {
                            traversal = traversal->right;
                        } else {
                            traversal = traversal->left;
                        }
                        if (traversal->c == EOD) {
                            EODRead = true;
                        }
                        if (!(traversal->right && traversal->left)) {
                            if (traversal->c == EOD) {
                                EODRead = true;
                            } else {
                                output.write((char *) &traversal->c,
                                             sizeof(char));
                                traversal = decodingRoot;
                            }

                        }
                    }
                }
                input.close();
                output.close();
            }
        }
    } else {
        cout << "The input format is -huff/-unhuff "
                "<source> <destination>" << endl;
    }
    return 0;
}
