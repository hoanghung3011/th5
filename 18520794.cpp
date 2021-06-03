//
//  main.cpp
//  buoi5
//
//  Created by Hung Kim Tran on 6/24/20.
//  Copyright © 2020 Hung Kim Tran. All rights reserved.
//

#include<iostream>
#include<string.h>
#include<sstream>
#include <fstream>

using namespace std;

int sizeOfArr = 0;
string grammars[100];
string reduceResult[100];
int numberOfExpression[100];

struct search {
    bool searchFlag;
    int index;
    int possition;
};

struct search compareOperator(char operatorOne, char operatorTwo) {
    struct search res;
    int one = -1, two = -1, pivot = 0;
    
    res.searchFlag = false;
    
    for (int i = 0; i<sizeOfArr; i++) {
        int goal = pivot + numberOfExpression[i];
        res.index = i;
        for (int j = pivot; j < goal; j++) {
            if (grammars[j].length() == 3) {
                if (grammars[j].at(1) == operatorOne) {
                    one = j;
                }
                if (grammars[j].at(1) == operatorTwo) {
                    two = j;
                }
            }
        }
        pivot += numberOfExpression[i];
    }

    res.possition = two;

    if (one >= two) {
        res.searchFlag = true;
        res.possition = one;
    }

    return res;
}

void logShiftEvent(char stack[], char inputSyntax[], int strLen, int startAt) {
    cout<<"\n";
    cout<<"shift";
    cout<<"\t\t|\t\t";
    cout<<stack;
    cout<<"\t\t|\t\t";
    for (int i=startAt; i<strLen; i++) cout<<inputSyntax[i];
    cout<<"\n";
}

void logReduceExpression(char stack[], char inputSyntax[], int strLen, int startAt, string expression, string reduceResult) {
    cout<<"\n";
    cout<<"reduce "<<reduceResult<<" --> "<<expression;
    cout<<"\t\t|\t\t";
    cout<<stack;
    cout<<"\t\t|\t\t";
    for (int i=startAt; i<strLen; i++) cout<<inputSyntax[i];
    cout<<"\n";
}

bool searhOperator(char operatorChar) {
    int pivot = 0;

    for (int i = 0; i<sizeOfArr; i++) {
        int goal = pivot + numberOfExpression[i];

        for (int j = pivot; j < goal; j++) {
            if (grammars[j].length() == 3) {
                if (grammars[j].at(1) == operatorChar) {
                    return true;
                }
                for (int k = 0; k < sizeOfArr; k++) {
                    if (grammars[j].at(1) == reduceResult[k].at(0)) {
                        if (grammars[j].at(0) == operatorChar || grammars[j].at(2) == operatorChar) {
                            return true;
                        }
                    }
                }
            }
        }
        pivot += numberOfExpression[i];
    }

    return false;
}

void elementReducer(char inputSyntax[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop, char reduceGoal) {
    while (true) {
        int pivot = 0;
        if (stack[stackTop - 1] == reduceGoal) {
            break;
        }
        for (int i = 0; i<sizeOfArr; i++) {
            int goal = pivot + numberOfExpression[i];

            for (int j = pivot; j < goal; j++) {
                if (grammars[j].length() == 1) {
                    if (grammars[j].at(0) == stack[stackTop - 1]) {
                        stack[stackTop - 1] = reduceResult[i].at(0);
                        logReduceExpression(stack, inputSyntax, inputLen, startAt, grammars[j], reduceResult[i]);
                    }
                }
            }
            pivot += numberOfExpression[i];
        }
    }
}

int operatorDeterminator(char inputSyntax[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop, struct search &searchResult) {
    int followPossition = 0;
    
    if (stackTop < 2) {
        int pivot = 0;

        followPossition = 1;
        
        for (int i = 0; i<sizeOfArr; i++) {
            int goal = pivot + numberOfExpression[i];

            searchResult.index = i;
            for (int j = pivot; j < goal; j++) {
                if (grammars[j].length() == 3) {
                    if (grammars[j].at(1) == inputSyntax[startAt]) {
                        searchResult.possition = j;
                        searchResult.searchFlag = true;
                        break;
                    }
                }
            }
            pivot += numberOfExpression[i];
        }

    } else if (startAt == inputLen) {
        int pivot = 0;

        followPossition = 3;

        for (int i = 0; i<sizeOfArr; i++) {
            int goal = pivot + numberOfExpression[i];

            searchResult.index = i;
            for (int j = pivot; j < goal; j++) {
                if (grammars[j].length() == 3) {
                    if (grammars[j].at(1) == stack[stackTop - 2]) {
                        searchResult.possition = j;
                        searchResult.searchFlag = true;
                        break;
                    }
                }
            }
            pivot += numberOfExpression[i];
        }
    } else {
        searchResult = compareOperator(stack[stackTop - 2], inputSyntax[startAt]);

        if (searchResult.searchFlag) {
            followPossition = 3;
        } else {
            followPossition = 1;
        }
    }

    if (searchResult.possition == -1) {
        cout<<"\nERROR: Syntax not found\n";
        exit(0);
    }

    return followPossition;
}

void reduceElement(char inputSyntax[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    int followPossition;
    struct search searchResult;

    searchResult.index = -1;
    searchResult.possition = -1;
    searchResult.searchFlag = false;

    if (startAt != inputLen) {
        followPossition = operatorDeterminator(inputSyntax, stack, inputLen, startAt, stackTop, searchResult);

        if (followPossition == 3) {
            elementReducer(inputSyntax, stack, inputLen, startAt, stackTop, grammars[searchResult.possition].at(2));
        }

        if (followPossition == 1) {
            elementReducer(inputSyntax, stack, inputLen, startAt, stackTop, grammars[searchResult.possition].at(0));
        }
    }

}


void expressionReducer(char inputSyntax[], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    int pivot = 0;

    for (int i = 0; i < sizeOfArr; i++) {
        int goal = pivot + numberOfExpression[i];

        for (int j = pivot; j < goal; j++) {
            if (grammars[j].length() == 3) {
                if (grammars[j].at(0) == stack[stackTop - 3] &&
                    grammars[j].at(1) == stack[stackTop - 2] &&
                    grammars[j].at(2) == stack[stackTop - 1]) {

                    stack[stackTop - 3] = reduceResult[i].at(0);
                    stack[stackTop - 2] = '\0';
                    stack[stackTop - 1] = '\0';
                    stackTop -= 2;
                    logReduceExpression(stack, inputSyntax, inputLen, startAt, grammars[j], reduceResult[i]);
                }
            }
        }
        pivot += numberOfExpression[i];
    }
}

void reduceSpecialElement(char inputSyntax[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    int pivot = 0;

    for (int i = 0; i<sizeOfArr; i++) {
        int goal = pivot + numberOfExpression[i];

        for (int j = pivot; j < goal; j++) {
            if (grammars[j].length() == 3) {
                if (grammars[j].at(1) == stack[stackTop - 2]) {
                    elementReducer(inputSyntax, stack, inputLen, startAt, stackTop, grammars[j].at(2));
                }
            }
        }
        pivot += numberOfExpression[i];
    }
}

void reduceSpecialOperator(char inputSyntax[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    int pivot = 0;

    for (int i = 0; i<sizeOfArr; i++) {
        int goal = pivot + numberOfExpression[i];

        for (int j = pivot; j < goal; j++) {
            if (grammars[j].length() == 3) {
                for (int k = 0; k < sizeOfArr; k++) {
                    if (grammars[j].at(1) == reduceResult[k].at(0)) {
                        if (grammars[j].at(0) == stack[stackTop - 1]){
                            break;
                        }
                        if (grammars[j].at(2) == stack[stackTop - 1]) {
                            expressionReducer(inputSyntax, stack, inputLen, startAt, stackTop);
                            break;
                        }
                    }
                }
            }
        }
        pivot += numberOfExpression[i];
    }
}

void reduceExpression(char inputSyntax[100], char (&stack)[100], int &inputLen, int startAt, int &stackTop) {
    int flag = 0;
    while (true) {
        int pivot = 0, firstOpsRank = -1, secondOpsRank = -1;
        char firstOps = stack[stackTop - 2], secondOps = inputSyntax[startAt];

        if (flag == 1) {
            break;
        }

        for (int i = 0; i < sizeOfArr; i++) {
            int goal = pivot + numberOfExpression[i];

            for (int j = pivot; j < goal; j++) {
                if (grammars[j].length() == 3) {
                    if (grammars[j].at(1) == firstOps) {
                        firstOpsRank = j;
                    }
                    if (grammars[j].at(1) == secondOps) {
                        secondOpsRank = j;
                    }
                }
            }
            pivot += numberOfExpression[i];
        }

        if (firstOpsRank < secondOpsRank) {
            break;
        }
        
        if (firstOpsRank == -1 && secondOpsRank == -1) {
            break;
        }

        expressionReducer(inputSyntax, stack, inputLen, startAt, stackTop);
    }
}


void shift(char (&inputSyntax)[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    stack[stackTop] = inputSyntax[startAt];

    startAt++;
    stackTop++;

    logShiftEvent(stack, inputSyntax, inputLen, startAt);
}

void shiftOrReduceVariable(char (&inputSyntax)[100], char (&stack)[100], int &inputLen, int &startAt, int &stackTop) {
    if (inputLen > stackTop && inputLen > startAt) {
        shift(inputSyntax, stack, inputLen, startAt, stackTop);
    }

    if (stackTop > 2) {
        reduceSpecialOperator(inputSyntax, stack, inputLen, startAt, stackTop);
    }

    if (stackTop > 2 && startAt == inputLen) {
        reduceSpecialElement(inputSyntax, stack, inputLen, startAt, stackTop);
    }

    if (!searhOperator(stack[stackTop - 1])) {
        reduceElement(inputSyntax, stack, inputLen, startAt, stackTop);
    }

    if (stackTop == 1 && startAt == inputLen) {
        elementReducer(inputSyntax, stack, inputLen, startAt, stackTop, reduceResult[0].at(0));
    }

    if (stackTop > 2 && startAt == inputLen) {
        expressionReducer(inputSyntax, stack, inputLen, startAt, stackTop);
    }
    

    if (stackTop > 2 && !searhOperator(stack[stackTop - 1])) {
        reduceExpression(inputSyntax, stack, inputLen, startAt, stackTop);
    }
}

void inputGrammar() {
    ifstream file;
    char filename[100];
    int n = 0, lineIndex = 0, grammarIndex = 0;

    string line;
    string inputGrammarRule[100];

    cout<<"file path(start from root like /Users/<machine_user>/src/nguyenli/input.txt):";
    cin>>filename;

    file.open(filename);
    file>>n;

    sizeOfArr = n;

    file.ignore();
    while (getline(file, line)) {
        inputGrammarRule[lineIndex] = line;
        lineIndex++;
        if (lineIndex == n) {
            break;
        }
    }

    for (int i = 0; i<n; i++) {
        int coutVar = 0;

        stringstream test(inputGrammarRule[i]);
        string segment;

        while(getline(test, segment, ' ')) {
            if (segment != "-->" && segment != "|") {
                if (coutVar == 0) {
                    reduceResult[i] = segment;
                } else {
                    grammars[grammarIndex] = segment;
                    grammarIndex++;
                }
                coutVar++;
            }
        }

        numberOfExpression[i] = coutVar - 1;
    }

    file.close();
}

void printOutGrammar() {
    int pivot = 0;

    cout<<"GRAMMAR:\n";

    for (int i = 0; i<sizeOfArr; i++) {
        int goal;

        goal = pivot + numberOfExpression[i];
        cout<<reduceResult[i];
        cout<<" -->";
        for (int j = pivot; j < goal; j++) {
            cout<<" ";
            cout<<grammars[j];
            cout<<" |";
        }
        cout<<"\n";
        pivot += numberOfExpression[i];
    }
}

void inputFile(char (&inputSyntax)[100]) {
    char filename[10000];
    ifstream file;
    cout<<"file path(start from root like /Users/<machine_user>/src/nguyenli/input.txt):";
    cin>>filename;
    file.open(filename);
    file>>inputSyntax;
    file.close();
}

int main() {
    int inputLen = 0, startAt = 0, stackTop = 0;
    char inputSyntax[100] = {'\0'}, stack[100] = {'\0'};

    inputGrammar();
    printOutGrammar();
    inputFile(inputSyntax);
    cout<<"\ninput syntax::"<<inputSyntax;

    inputLen = strlen(inputSyntax);

    cout<<"\n";
    cout<<"action\t\t|\t\tstack\t\t|\t\tinput";
    cout<<"\n";

    while (true) {
        shiftOrReduceVariable(inputSyntax, stack, inputLen, startAt, stackTop);

        if (stackTop == 1) {
            if (startAt == inputLen) {
                if (stack[stackTop - 1] == reduceResult[0].at(0)) {
                    break;
                }
            }
        }

    }


    cout<<"\n";
    cout<<"Accept";
    cout<<"\t\t|\t\t";
    cout<<stack;
    cout<<"\t\t|\t\t";
    for (int i=startAt; i<inputLen; i++) cout<<inputSyntax[i];
    cout<<"\n";

    cout<<"\n done";

    return 0;
}

    // char file[100] = "/Users/ngoctran/src/nguyenli/G2.txt";
    // char fileInput[100] = "/Users/ngoctran/src/nguyenli/input2.txt";