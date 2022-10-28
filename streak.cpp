// UMBC - CMSC 341 - Fall 2022 - Proj2
#include "streak.h"

/**
 * NOTES FOR GRADER:
 * I HAVE NO IDEA HOW, BUT MY INSERT FUNCTION GIVES A LOT OF MEMORY LEAK ERRORS (like thousands)
 * I ran out of time, so if you could tell me why that'd be great if it's not too much work.
 */

/**
 * Constructor
 */
Streak::Streak(){
    m_root = nullptr;
}

/**
 * Deconstructor
 */
Streak::~Streak(){
    clear();
}

/**
 * Inserts a tiger node into the BST, doesn't accept if tiger's ID is greater or less than the allowed max and min
 * @param tiger a tiger node to be added.
 */
void Streak::insert(const Tiger& tiger){

    //Check if tiger's ID is not within the accepted range of ID's.
    if(tiger.getID() < MINID || tiger.getID() > MAXID){
        return;
    }
    insert(tiger, m_root);
}

/**
 * Helper class for inserting a tiger into the BST
 * @param tigerAdd Tiger to be added
 * @param aNode the current node we are traversing
 */
void Streak::insert(const Tiger& tigerAdd, Tiger*& aNode) {

    //Check if the node is null, set node to tigerAdd if so
    if(aNode == nullptr){
        aNode = new Tiger(tigerAdd.getID(), tigerAdd.getAge(), tigerAdd.getGender(), tigerAdd.getState());
        aNode->m_right = nullptr;
        aNode->m_left = nullptr;
        aNode->m_height = 0;
    }
    //Check if the ID is equal to the node, if so, return - no duplicates
    else if(tigerAdd.getID() == aNode->getID()){
        return;
    }
    //Check whether we need to traverse left or right
    else if(tigerAdd.getID() < aNode->getID()){
        insert(tigerAdd, aNode->m_left);
        updateHeight(aNode);
        aNode = rebalance(aNode);
    }
    else if(tigerAdd.getID() > aNode->getID()){
        insert(tigerAdd, aNode->m_right);
        updateHeight(aNode);
        aNode = rebalance(aNode);
    }
     //Updates the height of the tree
}

/**
 * Clears the BST
 */
void Streak::clear(){
    clear(m_root);
}

/**
 * Helper class for clearing the BST
 * @param tiger
 */
void Streak::clear(Tiger* tiger) {

    //goes through the tree and deletes each node in postorder fashion
    if(tiger != nullptr){
        clear(tiger->m_right);
        clear(tiger->m_left);
        delete tiger;
        tiger = nullptr;
    }
}

//Removes a tiger with a specific ID.
void Streak::remove(int id){
    m_root = remove(id, m_root);
}

/**
 * Helper class for removing a tiger with a specific ID
 * @param id
 * @param tiger
 * @return
 */
Tiger* Streak::remove(const int& id, Tiger* tiger) {

    Tiger* temp;
    //Element was not found
    if(tiger == nullptr){
        return nullptr;
    }

    //Searches for element
    else if(id < tiger->getID()){
        tiger->m_left = remove(id, tiger->m_left);
    }
    else if(id > tiger->getID()){
        tiger->m_right = remove(id, tiger->m_right);
    }
    //Element found

    //If it has 2 kids
    else if(tiger->m_left != nullptr && tiger->m_right != nullptr){
        temp = findMin(tiger->m_right);

        tiger->m_id = temp->m_id;
        tiger->m_age = temp->m_age;
        tiger->m_gender = temp->m_gender;
        tiger->m_state = temp->m_state;

        tiger->m_right = remove(tiger->m_id,tiger->m_right);
    }

    //If it has 1 or no children
    else{
        temp = tiger;
        if(tiger->m_left == nullptr){
            tiger = tiger->m_right;
        }
        else if(tiger->m_right){
            tiger = tiger->m_left;
        }
        delete temp;
    }

    if(tiger == nullptr){
        return tiger;
    }

    updateHeight(tiger);
    tiger = rebalance(tiger);


    return tiger;
}

/**
 * Helper for finding the minimum value of a subtree
 * @param tiger a tiger node
 * @return the minimum node.
 */
Tiger* Streak::findMin(Tiger *tiger) {
    //Check to make sure the tiger isn't a nullptr and that it has a left.
    if(tiger->m_left == nullptr || tiger == nullptr){
        return tiger;
    }
    else{
        return findMin(tiger->m_left); //recur for left until you can't go left anymore
    }
}

/**
 * Helper for finding the height of a tiger.
 * @param tiger a tiger node
 * @return the height of the tiger.
 */
int Streak::findHeight(Tiger* tiger) {

    //If nullptr the height should be -1
    if(tiger == nullptr){
        return -1;
    }
    int height = 0;

    int lHeight = findHeight(tiger->m_left);
    int rHeight = findHeight(tiger->m_right);

    int maxHeight = max(lHeight, rHeight); //Finds the max height between its left and right subtrees

    height = maxHeight + 1; //Add 1 to it since the parent will always have a height of 1 more than its kids
    return height;
}

/**
 * Updates the height of the subtree
 * @param aTiger a tiger node
 */
void Streak::updateHeight(Tiger* aTiger){

    int lHeight = 0, rHeight = 0;

    //If it's left is not null then set lHeight to m_left's height. Same for right side
    if(aTiger->m_left != nullptr){
        lHeight = aTiger->m_left->getHeight();
    }
    if(aTiger->m_right != nullptr){
        rHeight = aTiger->m_right->getHeight();
    }

    //Get the maxHeight between the 2
    int maxHeight = max(lHeight, rHeight); //Finds the max height between its left and right subtrees

    aTiger->setHeight(maxHeight+1);//Set aTiger's height to maxHeight + 1

}

/**
 * Checks for an imbalance (height diff > 1 || < -1) between a tiger's left and right children
 * @param aTiger
 * @return the imbalance
 */
int Streak::checkImbalance(Tiger* aTiger){

    //find the height of its left and its right

    int lHeight = 0, rHeight = 0;
    if(aTiger->m_left != nullptr){
        lHeight = aTiger->m_left->getHeight();
    }
    if(aTiger->m_right != nullptr){
        rHeight = aTiger->m_right->getHeight();
    }

    return lHeight - rHeight; //return the difference

}

/**
 * Rebalances a tiger node.
 * @param aTiger a tiger node
 * @return the node after it's been rebalanced.
 */
Tiger* Streak::rebalance(Tiger* aTiger){

    Tiger* temp = aTiger;

    //null check
    if(temp == nullptr){
        cout << "Tiger passed in to rebalance is nullptr." << endl;
        return nullptr;
    }

    int imbalance = checkImbalance(temp);

    //If the difference in the heights of its 2 children is > 1
    if(imbalance > 1){
        if(checkImbalance(temp->m_left) > 0){
            temp = ll_rotation(temp);
        }
        else{
            temp = lr_rotation(temp);
        }
    }
    //If the difference in the heights of its 2 children is less than -1
    if(imbalance < -1){
        if(checkImbalance(temp->m_right) > 0){
            temp = rl_rotation(temp);
        }
        else{
            temp = rr_rotation(temp);
        }
    }

    return temp;
}

void Streak::dumpTree() const {dump(m_root);}

void Streak::dump(Tiger* aTiger) const{
    if (aTiger != nullptr){
        cout << "(";
        dump(aTiger->m_left);//first visit the left child
        cout << aTiger->m_id << ":" << aTiger->m_height;//second visit the node itself
        dump(aTiger->m_right);//third visit the right child
        cout << ")";
    }
}

/**
 * Lists the tigers in ascending ID order
 */
void Streak::listTigers() const {
    listTigersHelper(m_root);
}

/**
 * Helper class for listing the tigers in ascending order
 * Uses Inorder traversal.
 * @param tiger a tiger node
 */
void Streak::listTigersHelper(Tiger *tiger) const{

    if(tiger == nullptr) {
        return;
    }

    //Call on left child
    listTigersHelper(tiger->m_left);

    //prints out the current tiger node
    cout << tiger->getID() << ":" << tiger->getAgeStr() << ":" << tiger->getGenderStr() << ":" <<
    tiger->getStateStr() << endl;

    //Calls on right child
    listTigersHelper(tiger->m_right);
}

/**
 * Sets the state of a tiger
 * @param id the id of the tiger we are searching for
 * @param state the state we want to insert into the tiger
 * @return true if it's successful, false if not.
 */
bool Streak::setState(int id, STATE state){
    //If the tiger is not in the tree return false
    if(findTiger(id) == false){
        return false;
    }
    else{
        return setState(id, state, m_root);
    }
}

/**
 * Helper for setting the state of a tiger
 * @param id id of the tiger we're looking for
 * @param state state of the tiger we want it to be changed to
 * @param tiger a tiger node
 * @return true if successful, false if not.
 */
bool Streak::setState(int id, STATE state, Tiger* tiger) {
    //If we've reached null, return false.
    if(tiger == nullptr){
        return false;
    }
    //If we find the id, set the STATE to state and return true;
    else if(tiger->getID() == id){
        tiger->setState(state);
        return true;
    }
    //Go left or right depending on id
    else if(tiger->getID() > id){
        bool result = setState(id, state, tiger->m_left);
        return result;
    }
    else {
        bool result = setState(id, state, tiger->m_right);
        return result;
    }
}

/**
 * Removes the dead tigers from the bst
 */
void Streak::removeDead(){
    m_root = removeDead(m_root);
}

/**
 * Helper for removing the dead tigers from the bst
 * postorder traversal
 * @param tiger the current tiger that is being checked.
 */
Tiger* Streak::removeDead(Tiger* tiger) {

    if(tiger == nullptr){
        return nullptr;
    }

    //Traverse in postorder
    tiger->m_left = removeDead(tiger->m_left);
    tiger->m_right = removeDead(tiger->m_right);

    //If it's found, remove it
    if(tiger->getState() == DEAD){
        remove(tiger->getID());
    }
    return tiger;
}

/**
 * Finds a specific tiger.
 * @param id the ID we are searching for
 * @return true if found, false if not.
 */
bool Streak::findTiger(int id) const {
    return findTigerHelper(m_root, id);
}

/**
 * Helper function for finding a specific Tiger.
 * Uses preorder traversal.
 * @param tiger a tiger node
 * @param id the id we are searching for
 * @return
 */
bool Streak::findTigerHelper(Tiger* tiger, int id) const{

    if(tiger == nullptr){
        return false;
    }
    else if(tiger->getID() == id){
        return true;
    }
    else if(tiger->getID() > id){
        bool result = findTigerHelper(tiger->m_left, id);
        return result;
    }
    else {
        bool result = findTigerHelper(tiger->m_right, id);
        return result;
    }
}

/**
 * Counts the number of cubs in the tree
 * @return the number of cubs
 */
int Streak::countTigerCubs() const {
    return countTigerCubs(m_root);
}

/**
 * Helper class for counting the number of cubs
 * @return the number of cubs.
 */
int Streak::countTigerCubs(Tiger* tiger) const{
    //Case if we reached the end/nullptr
    if(tiger == nullptr){
        return 0;
    }

    //Recursion if it finds the cub
    else if(tiger->getAge() == CUB){
        return 1 + countTigerCubs(tiger->m_left)+ countTigerCubs(tiger->m_right);
    }

    //Recursion if not cub
    else{
        return countTigerCubs(tiger->m_left) + countTigerCubs(tiger->m_right);
    }
}

/**
 * Single right rotation
 * @param root
 * @return
 */
Tiger* Streak::rr_rotation(Tiger* root)
{
    Tiger *temp;
    temp = root->m_right;
    root->m_right = temp->m_left;
    temp->m_left = root;

    updateHeight(root);
    updateHeight(temp);

    return temp;
}

/**
 * Single left rotation
 * @param root
 * @return
 */
Tiger* Streak::ll_rotation(Tiger* root)
{
    Tiger *temp;
    temp = root->m_left;
    root->m_left = temp->m_right;
    temp->m_right = root;

    updateHeight(root);
    updateHeight(temp);

    return temp;
}

/**
 * Double left right rotation
 * @param root
 * @return
 */
Tiger *Streak::lr_rotation(Tiger *root)
{
    Tiger *temp;
    temp = root->m_left;
    root->m_left = rr_rotation(temp);
    return ll_rotation(root);
}

/**
 * Double right left rotation
 * @param root
 * @return
 */
Tiger* Streak::rl_rotation(Tiger* root)
{
    Tiger *temp;
    temp = root->m_right;
    root->m_right = ll_rotation(temp);
    return rr_rotation(root);
}

/**
 * Tests if the tree is balanced by comparing the heights of children at each node
 * @param tiger a tiger node
 * @return true if always balanced
 */
int Streak::treeBalanced(Tiger* tiger) {

    //If nullptr return 0
    if(tiger == nullptr){
        return 0;
    }
    /*
     * Checks for an imbalance, if there is one return +1
     * Otherwise recur for left and right children.
     */
    if (checkImbalance(tiger) > 1 || checkImbalance(tiger) < -1) {
        return 1 + treeBalanced(tiger->m_left) + treeBalanced(tiger->m_right);
    } else {
        return treeBalanced(tiger->m_left) + treeBalanced(tiger->m_right);
    }

}

/**
 * Tests if a tree is a true BST throughout the tree
 * @param tiger a tiger node
 * @return 0 if it's a BST, greater than 0 if not.
 */
int Streak::treeBST(Tiger* tiger) {

    //If it reaches a nullptr then return 0
    if(tiger == nullptr){
        return 0;
    }
    /*
     * All of these are different cases of what types of children tiger has
     * If the id of tiger is greater than its right child return +1
     * If the id of tiger is less than its left child return +1
     * If it has no children return 0
     * Otherwise recur for left and right children
     */

    //These 2 are for if it only has 1 of its kids
    if(tiger->m_left == nullptr && tiger->m_right != nullptr){
        if(tiger->getID() > tiger->m_right->getID()){
            return 1 + treeBST(tiger->m_left) + treeBST(tiger->m_right);
        }
        else{
            return treeBST(tiger->m_left) + treeBST(tiger->m_right);
        }
    }
    else if(tiger->m_left != nullptr && tiger->m_right == nullptr){
        if(tiger->getID() < tiger->m_left->getID()){
            return 1 + treeBST(tiger->m_left) + treeBST(tiger->m_right);
        }
        else{
            return treeBST(tiger->m_left) + treeBST(tiger->m_right);
        }
    }
    //If no children return 0
    else if(tiger->m_left == nullptr && tiger->m_right == nullptr){
        return 0;
    }

    //Final check for when it has both kids (by process of elimination from previous checks)
    else if(tiger->getID() > tiger->m_right->getID() || tiger->getID() < tiger->m_left->getID()){
        return 1 + treeBST(tiger->m_left) + treeBST(tiger->m_right);
    }

    //Otherwise simply recur for children
    return treeBST(tiger->m_left) + treeBST(tiger->m_right);
}











