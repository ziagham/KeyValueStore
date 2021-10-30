#include<iostream>
#include<string.h>
// #include<mutex>
using namespace std;

// mutex hi;


struct Slice
{
    uint8_t size;
    char *data;
};

class kvStore
{

    pthread_mutex_t m_lock;
    struct TrieNode 
    { 
        struct TrieNode *children[53]; 
        
        // isEndOfWord is true if the node represents 
        // end of a word 
        bool isEndOfWord; 
        Slice *value;
        pthread_mutex_t lock; 
        // struct Slice *value;
        int no_of_ends;
    }; 
    
    // Returns new trie node (initialized to NULLs) 
    struct TrieNode *getNode(void) 
    { 
        struct TrieNode *pNode =  new TrieNode; 
    
        pNode->isEndOfWord = false; 
        pNode->no_of_ends=0;
    
        for (int i = 0; i < 52; i++) 
            pNode->children[i] = NULL; 
    
        return pNode; 
    } 
    public:
        struct TrieNode *root = getNode(); 
        int get_index(char ch)
        {
            // if(ch>='a')
            //     return ch-71;
            // return ch-65;
            return ch-65 - (6 & -(ch>='a'));
        }
        int size;
        kvStore(uint64_t max_entries)
        {
            size=0;
        }
        bool put(Slice &key, Slice &value)
        {
            pthread_mutex_lock(&m_lock);
            // return false;
            // bool isThere=true;
            bool isThere=true; 
            struct TrieNode *curNode = root;
            // printf("%s\n",key);
            // cout << key;
            // malloc(sizeof(TrieNode)); 
            for (int i = 0; i < key.size; i++)
            { 
                // int index = get_index(key.data[i]); 
                int index = key.data[i]-65 - (6 & -(key.data[i]>='a'));

                // cout << index << endl;

                if (!curNode->children[index])
                { 
                    curNode->children[index] = getNode();
                    curNode->children[index]->children[52] = curNode; 
                    isThere=false;
                }
                // pthread_mutex_lock(&curNode->children[index]->lock); 
                curNode->children[index]->no_of_ends+=1;
                curNode = curNode->children[index];
                // pthread_mutex_unlock(&curNode->lock);
                
                // curNode->no_of_ends+=1;

            } 

            // mark last node as leaf
            bool isEnd=curNode->isEndOfWord; 
            Slice *val;
            val=(struct Slice *)malloc(sizeof(Slice));
            val->size=value.size;
            val->data=value.data; 
            // pthread_mutex_lock(&curNode->lock); 
            curNode->isEndOfWord = true;
            curNode->value=val;
            size++;
            // pthread_mutex_unlock(&curNode->lock); 
            // cout << key.data << "  ---  " << curNode->value << endl ;
            if(isEnd)
            {
                // cout << "ocindi\n";
                size--;
                for (int i = key.size-1; i>=0; i--) 
                { 
                    // // int index = get_index(key[i]); 
                    // if (!curNode->children[index])
                    // { 
                    //     curNode->children[index] = getNode();
                    //     curNode->children[index]->children[52] = curNode; 
                    //     isThere=false;
                    // }
                        
                    // pthread_mutex_lock(&curNode->lock); 
                    curNode->no_of_ends-=1; 
                    // pthread_mutex_unlock(&curNode->lock);
                    curNode = curNode->children[52];

                } 
            }
            pthread_mutex_unlock(&m_lock);
            return isThere && isEnd; 
        }

        bool get(Slice &key,Slice &value)
        {
            // pthread_mutex_lock(&m_lock);
            // return false;
            struct TrieNode *curNode = root;
            // struct Slice st=key;
            // malloc(sizeof(TrieNode)); 

            for (int i = 0; i<key.size; i++) 
            { 
                // int index = get_index(key.data[i]); 
                int index = key.data[i]-65 - (6 & -(key.data[i]>='a'));

                if (!curNode->children[index]) 
                {
                    // pthread_mutex_unlock(&m_lock);
                    return false; 
                }
                curNode = curNode->children[index]; 
            } 
            // return curNode->value;
            if(curNode->value!=NULL)
                value=*curNode->value;
            // pthread_mutex_unlock(&m_lock);
            return (curNode != NULL && curNode->isEndOfWord);             
        }

        bool del(Slice &key)
        {
            pthread_mutex_lock(&m_lock);
            // cout << key;
            // printf("1");
            struct TrieNode *curNode = root;
            // struct Slice st=key;
            // malloc(sizeof(TrieNode)); 
            for (int i = 0; i<key.size-1; i++) 
            { 
                // int index = get_index(key.data[i]); 
                int index = key.data[i]-65 - (6 & -(key.data[i]>='a'));

                // cout << key[i];
                // printf("1");
                if (!curNode->children[index]) 
                {
                    pthread_mutex_unlock(&m_lock);
                    return false; 
                }
                curNode = curNode->children[index]; 
            }

            int i=key.size-1;
            // int index = get_index(key.data[i]);
            int index = key.data[i]-65 - (6 & -(key.data[i]>='a'));

            if(curNode->children[index]==NULL)
            {
                pthread_mutex_unlock(&m_lock);
                return false;
            }
            size--;
            struct TrieNode *temp=curNode;
            temp=temp->children[index];
            // cout << "temp" << temp->value << endl;
            for (int i = key.size-1; i>=0; i--) 
            { 
                // pthread_mutex_lock(&temp->lock); 
                temp->no_of_ends-=1;
                // pthread_mutex_unlock(&temp->lock); 
                 
                // cout << "temp   " << temp->no_of_ends << "   " << temp->value << endl;
                temp = temp->children[52];
            } 
            // curNode=temp;
            if(curNode->children[index]->no_of_ends==0 && curNode->children[index]->isEndOfWord)
            {
            
                free(curNode->children[index]);
                curNode->children[index] = NULL;
            }
            else
            {
                // printf("ochindi\n");
                // pthread_mutex_lock(&temp->children[index]->lock); 
                curNode->children[index]->value=NULL;
                curNode->children[index]->isEndOfWord=false;
                // pthread_mutex_unlock(&temp->children[index]->lock); 
                // cout << curNode->children[index]->value << "   " << curNode->children[index]->no_of_ends << endl;
            }
            // cout << "final " << curNode->children[index]->children['i'-'a']->value << endl;
            pthread_mutex_unlock(&m_lock);
            return true;
        }  
};

// int main()
// {
//     kvStore obj(10);
//     Slice a,b,c,d,e,f,test,test1;
//     // a=(struct Slice *)malloc(sizeof(Slice));
//     // b=(struct Slice *)malloc(sizeof(Slice));
//     // c=(struct Slice *)malloc(sizeof(Slice));
//     // d=(struct Slice *)malloc(sizeof(Slice));
//     // e=(struct Slice *)malloc(sizeof(Slice));
//     // f=(struct Slice *)malloc(sizeof(Slice));
//     // test=(struct Slice *)malloc(sizeof(Slice));
//     // test1=(struct Slice *)malloc(sizeof(Slice));
//     a.data=(char *)"hi";
//     a.size=2;
//     b.data=(char *)"hello";
//     b.size=5;
//     c.data=(char *)"welcome";
//     c.size=7;
//     d.data=(char *)"adab";
//     d.size=4;
//     e.data=(char *)"namaste";
//     e.size=7;
//     f.data=(char *)"vanakkam";
//     f.size=8;
//     // for(int i=0;i<5;i++)
//     //     obj.put(a,f);
    
//     // obj.put(a,b);
//     // obj.put(a,c);
//     // obj.put(d,f);
//     // obj.put(c,d);
//     // obj.put(c,d);
//     cout << "size " << obj.size << endl;
    
//     if(obj.del(1))
//         cout << test1.data << " " << test1.data << endl;
//     else
//         cout << "empty" << endl;
        
//     // obj.get(2,test,test1);
//     // cout << test.data << " " << test1.data << endl;

//     // obj.del(2);

//     // obj.get(1,test,test1);
//     // cout << test.data << " " << test1.data << endl;
//     // obj.get(2,test,test1);
//     // cout << test.data << " " << test1.data << endl;
// }






// struct Slice
// {
//     uint8_t size;
//     char *data;
// };

// struct TrieNode 
// { 
//     struct TrieNode *children[52]; 
    
//     // isEndOfWord is true if the node represents 
//     // end of a word 
//     bool isEndOfWord; 
//     struct Slice *key;
// }; 
  
// // Returns new trie node (initialized to NULLs) 
// struct TrieNode *getNode(void) 
// { 
//     struct TrieNode *pNode =  new TrieNode; 
  
//     pNode->isEndOfWord = false; 
  
//     for (int i = 0; i < 52; i++) 
//         pNode->children[i] = NULL; 
  
//     return pNode; 
// } 
  
// If not present, inserts key into trie 
// If the key is prefix of trie node, just 
// marks leaf node 
// void insert(struct TrieNode *root, string key) 
// { 
//     struct TrieNode *curNode = root; 
  
//     for (int i = 0; i < key.length(); i++) 
//     { 
//         int index = get_index(key[i]); 
//         if (!curNode->children[index]) 
//             curNode->children[index] = getNode(); 
  
//         curNode = curNode->children[index]; 
//     } 
  
//     // mark last node as leaf 
//     curNode->isEndOfWord = true; 
// } 
  
// // Returns true if key presents in trie, else 
// // false 
// bool search(struct TrieNode *root, string key) 
// { 
//     struct TrieNode *curNode = root; 
  
//     for (int i = 0; i < key.length(); i++) 
//     { 
//         int index = get_index(key[i]); 
//         if (!curNode->children[index]) 
//             return false; 
  
//         curNode = curNode->children[index]; 
//     } 
  
//     return (curNode != NULL && curNode->isEndOfWord); 
// }