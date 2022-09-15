#include "InternalNode.hpp"
#include<bits/stdc++.h>
//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    vector<Key> keys=this->keys;
    int ind;
    
    auto index=lower_bound(keys.begin(),keys.end(),key);
    ind=index-keys.begin();
    TreePtr insertnode=this->tree_pointers[ind];
    cout<<ind<<endl;
    TreeNode * node=tree_node_factory(insertnode);
    TreePtr split= node->insert_key(key,record_ptr);
    if(!is_null(split)){
        auto leftnode=tree_node_factory(insertnode);
        this->keys.push_back(leftnode->max());
        sort(this->keys.begin(),this->keys.end());
        int curr=lower_bound(this->keys.begin(),this->keys.end(),leftnode->max())-this->keys.begin()+1;
        // this->tree_pointers.push_back(split);
        this->tree_pointers.insert(this->tree_pointers.begin()+curr,split);
        this->size+=1;
        if(this->overflows()){
            auto mid=this->keys.begin();
            cout<<this->keys.size()<<endl;
            advance(mid,ceil(this->keys.size()/2.0));
            auto newnode=new InternalNode();
            for(auto it=mid;it!=this->keys.end();it++){
                newnode->keys.push_back(*it);
            }
            auto midpointer=this->tree_pointers.begin();
            advance(midpointer,ceil(this->tree_pointers.size()/2.0));
            for(auto it=midpointer;it!=this->tree_pointers.end();it++){
                newnode->tree_pointers.push_back(*it);
                newnode->size+=1;
            }
            newnode->dump();
        
            this->keys.erase(mid-1,this->keys.end());
            cout<<this->keys.size()<<"*************"<<this->size<<endl;
            this->size=this->keys.size()+1;
            this->tree_pointers.erase(midpointer-1,this->tree_pointers.end());
            this->dump();
            cout<<this->tree_ptr<<"  "<<newnode->tree_ptr<<endl;
            return newnode->tree_ptr;
        }
    }
    this->dump();
    // cout << "InternalNode::insert_key not implemented" << endl;
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    cout << "InternalNode::delete_key not implemented" << endl;
    this->dump();
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++) {
        if (min_key <= this->keys[i]) {
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            // cout<<"AGAIAN -----------"<<endl;
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++) {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i - 1]) + " < x";
        } else {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for (int i = 0; i < this->size - 1; i++) {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is) {
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++) {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
