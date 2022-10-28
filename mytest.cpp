#include "streak.h"
#include <random>

enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else{ //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

class Tester{
public:
    bool testInsertNormal(int size);
    bool testInsertError();
    bool testBalancedAfterInsertion();
    bool testBSTAfterInsertion();
    bool testRemoveNormal(int size);
    bool testRemoveDead();
    bool testCountCubs();
    bool testInsertTime(int treeSize);
    bool testRemoveTime(int treeSize, int numToRemove);

};

/**
 * Tests for whether an insert is successful for any size
 * Tests whether it was successful by finding the ID after insertion.
 * @param size size of the tree
 * @return true if every id was found after insertion, false if not.
 */
bool Tester::testInsertNormal(int size) {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = size;
        int ID = 0;
        bool result = true;
        for(int i = 0; i < streakSize; i++) {
            ID = idGen.getRandNum();
            Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
            streak.insert(tiger);
            //If the insert was successful it should return true ALWAYS
            if(streak.findTiger(ID) != true){
                result = false;
            }
        }

        return result;
    }
}

/**
 * Tests for error cases, i.e. invalid id, duplicate ID.
 * @param id
 * @return
 */
bool Tester::testInsertError() {
    Streak streak;
    Tiger tiger1(00001, YOUNG, MALE);
    Tiger tiger2(10000, OLD, FEMALE);
    Tiger tiger3(10000, CUB, MALE);
    streak.insert(tiger1);
    streak.insert(tiger2);
    streak.insert(tiger3);


    /*
     * If it can find either tiger3 (Should not since duplicate) or tiger1 (invalid ID) then return false
     * Otherwise return true if tiger3 is the same ID as tiger2 and if tiger1's ID is less than the accepted ID.
     */
    if(streak.findTiger(tiger1.getID() == true) || streak.findTiger(tiger3.getID() == true)){
        return false;
    }
    else{
        return (tiger3.getID() == tiger2.getID()) && tiger1.getID() < MINID;
    }

}

/**
 * Tests if a tree is balanced after 300 insertions
 * @return true if successful, false if not.
 */
bool Tester::testBalancedAfterInsertion() {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = 300;
        int ID = 0;
        for (int i = 0; i < streakSize; i++) {
            ID = idGen.getRandNum();
            Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
            streak.insert(tiger);
        }

        /*
         * Calls on helper treeBalanced function
         * treeBalanced checks for an imbalance at each node starting from the root and adds 1 to a counter if it finds one.
         * If at any point does it reach an imbalance it will return a value greater than 0.
         */
        return streak.treeBalanced(streak.m_root) == 0;
    }
}

/**
 * Tests if a tree is a BST after 300 insertions.
 * @return true if it is, false if not.
 */
bool Tester::testBSTAfterInsertion() {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = 300;
        int ID = 0;
        for (int i = 0; i < streakSize; i++) {
            ID = idGen.getRandNum();
            Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
            streak.insert(tiger);
        }

        /*
         * Calls on helper function treeBST
         * treeBST goes through the tree and searches for any instances where the left child is greater than the ID of it's parent or if the right child is less than the parent
         * Returns a +1 for whenever it finds such an instance
         * thus if the call is greater than 0 than it is not a true BST.
         */
        return streak.treeBST(streak.m_root) == 0;
    }
}

/**
 * Takes in a streak of size 300 and deletes a number of them
 * NUMBER TO DELETE MUST BE AT LEAST 1 (i think)
 * Also checks for if the tree is balanced after
 * Also checks for if the tree is still a BST after
 * @param numToDelete the number of IDs to delete
 * @return true if the deletes were successful.
 */
bool Tester::testRemoveNormal(int numToDelete) {
    Random idGen(MINID,MAXID);
    Random idGen2(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = 300;
        int removeSize = numToDelete;
        int ID = 0;

        //For testing if we can find a tiger that's been removed
        int result = true;

        //Creates a tree of 300 nodes with ascending IDs
        for(int x = 10000; x < streakSize + 10000; x++){
            ID = x;
            Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
            streak.insert(tiger);
        }


        //Removes the first x number of nodes from the streak.
        for(int i = 10000; i < removeSize + 10000; i++){
            streak.remove(i);
        }

        /*
         * Return true if:
         * 1. cannot find ANY of the tigers removed (result must stay true in for loop)
         * 2. Tree is balanced (using helper treeBalanced)
         * 3. Tree is a BST (using helper treeBST)
         */
        for(int j = 0; j < removeSize; j++){
            //Return false if it finds ONE of the removed tigers
            if(streak.findTiger(j+10000)){
                result = false;
            }
        }
        return result && streak.treeBalanced(streak.m_root) == 0 && streak.treeBST(streak.m_root) == 0;
    }
}

/**
 * Tests if it properly removes the dead.
 * Also checks if it's a BST
 * Also checks if it's a balanced tree
 * @return true if the removals were successful
 */
bool Tester::testRemoveDead() {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = 50;
        int ID = 0;

        //For testing if we can find a tiger that's been removed
        int result = true;

        //Creates a tree of 50 nodes with ascending IDs
        for(int x = 10000; x < streakSize + 10000; x++){
            ID = x;
            Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()), ALIVE);
            streak.insert(tiger);
        }

        //Set tiger number 10025's state to DEAD.
        streak.setState(10025, DEAD);
        //Remove the dead tiger
        streak.removeDead();

        /*
         * Return true if:
         * 1. It cannot find the tiger we removed
         * 2. It's a BST
         * 3. It's a balanced tree
         */
        return !streak.findTiger(10025) && streak.treeBST(streak.m_root) == 0 && streak.treeBalanced(streak.m_root) == 0;

    }
}

/**
 * Tests if it properly counts the number of cubs
 * @return true if cubs is counted correctly
 */
bool Tester::testCountCubs() {
    Random idGen(MINID,MAXID);
    Random genderGen(0,2);
    {
        Streak streak;
        int streakSize = 20;
        int ID = 0;
        for(int i = 0; i < streakSize; i++) {
            ID = idGen.getRandNum();
            //Make the first 10 entered cubs, the rest are old
            if(i < 10){
                Tiger tiger(ID, CUB, static_cast<GENDER>(genderGen.getRandNum()));
                streak.insert(tiger);
            }
            else{
                Tiger tiger(ID, OLD, static_cast<GENDER>(genderGen.getRandNum()));
                streak.insert(tiger);
            }
        }
        /*
         * Return true if:
         * 1. The number of cubs counted is equal to 10 (since we inserted 10 of them).
         */
        return streak.countTigerCubs() == 10;
    }
}

/**
 * Measures the time for insertion in clock ticks
 * Takes in 2 trials:
 * 1 for inserting 1000 nodes
 * 1 for inserting 2000 nodes
 * @param treeSize
 */
bool Tester::testInsertTime(int treeSize) {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);

    const int a = 2; //Scaling factor
    double T = 0.0, T2 = 0.0; //stores the running time
    clock_t start, stop; //stores clock ticks
    clock_t start2, stop2; //Stores clock ticks for the second test
    Streak streak;
    int ID = 0;

    //Time how long the insertions take
    start = clock();
    for(int j = 0; j < treeSize; j++) {
        ID = idGen.getRandNum();
        Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
        streak.insert(tiger);
    }
    stop = clock();
    T = stop - start;//record the time

    cout <<"\nInserting " << treeSize << " nodes took " << T << " clock ticks\n";

    /* * * * * * * *
     * SECOND TEST *
     * * * * * * * */

    //Double the size for test 2
    treeSize = treeSize * a;

    //Time how long the insertions take
    start2 = clock();
    for(int i = 0; i < treeSize; i++){
        ID = idGen.getRandNum();
        Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
        streak.insert(tiger);
    }
    stop2 = clock();
    T2 = stop2 - start2;//record the time
    cout << "Inserting " << treeSize << " nodes took " << T2 << " clock ticks\n";

    //Calculate T2/2T1
    cout << "T2/2T1 is " << T2/(2*T) << endl;

    //If the values are within the range, return true.
    return T2/(2*T) > 0.7 && T2/(2*T) < 1.5;


}

/**
 * Measures the time for removing in clock ticks
 * Takes in 2 trials:
 * 1 for removing 1000 nodes
 * 1 for removing 2000 nodes
 * NOTE: This seems to be a little high (1.4-2 range)
 * @param treeSize
 * @param numToRemove
 */
bool Tester::testRemoveTime(int treeSize, int numToRemove) {
    Random idGen(MINID,MAXID);
    Random ageGen(0,2);
    Random genderGen(0,2);

    int removeNum = numToRemove; // number of elements to remove
    const int a = 2; //Scaling factor
    double T = 0.0, T2 = 0.0; //stores the running time
    clock_t start, stop; //stores clock ticks
    clock_t start2, stop2; //Stores clock ticks for the second test
    Streak streak;
    Streak streak2;
    int ID = 0;

    //Creates the tree
    for(int j = 10000; j < treeSize + 10000; j++) {
        ID = j;
        Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
        streak.insert(tiger);
    }

    //times how long the removal takes
    start = clock();
    for(int i = 10000; i < removeNum + 10000; i++){
        streak.remove(i);
    }
    stop = clock();
    T = stop - start; //times the removal
    cout << "\nRemoving " << removeNum << " nodes took " << T << " clock ticks\n";

    /* * * * * * * *
     * SECOND TEST *
     * * * * * * * */

    //Scale up the sizes for the second test
    treeSize = treeSize * a;
    removeNum = removeNum * a;

    //Create the second tree
    for(int j = 10000; j < treeSize + 10000; j++){
        ID = j;
        Tiger tiger(ID, static_cast<AGE>(ageGen.getRandNum()), static_cast<GENDER>(genderGen.getRandNum()));
        streak2.insert(tiger);
    }

    //times how long the removals take
    start2 = clock();
    for(int i = 10000; i < removeNum + 10000; i++){
        streak2.remove(i);
    }
    stop2 = clock();
    T2 = stop2 - start2;//times the removal
    cout << "Removing " << removeNum << " nodes took " << T2 << " clock ticks\n";

    //Calculate T2/2T1 - Seems to be a little high, also seems to take a lot of ticks.
    cout << "T2/2T1 is " << T2/(2*T) << endl;

    return T2/(2*T) > 0.7 && T2/(2*T) < 1.5;
}


int main(){
    Tester tester;
    {
        const char* FAIL = "Test Failed\n";
        const char* PASS = "Test Passed\n";

        cout << "Testing insert test cases" << endl;
        //First tests for a size of 10, then a size of 300.
        if(tester.testInsertNormal(10)){
            if(tester.testInsertNormal(300)){
                cout << PASS;
            }
            else{
                cout << FAIL;
            }
        }
        else{
            cout << FAIL;
        }

        //Tests for edge case size of 1
        if(tester.testInsertNormal(1)){
            cout << PASS;
        }

        /*
         * Tests for error cases
         * 1. id entered is too low
         * 2. id matches another id
         */
        if(tester.testInsertError()){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting if tree is balanced and if tree is a BST after insertions\n";
        if(tester.testBalancedAfterInsertion()){
            cout << PASS;
        }
        else{
            cout << FAIL << endl;
        }
        if(tester.testBSTAfterInsertion()){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting remove test cases\n";
        //Case for only 1 being removed
        if(tester.testRemoveNormal(1)){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }
        //Case for 280 removals
        if(tester.testRemoveNormal(280)){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting removeDead\n";
        if(tester.testRemoveDead()){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting count cubs\n";
        if(tester.testCountCubs()){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting insertion time";
        if(tester.testInsertTime(10000)){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }

        cout << "\nTesting removal time";
        if(tester.testRemoveTime(20000,10000)){
            cout << PASS;
        }
        else{
            cout << FAIL;
        }
    }
    return 0;
}
