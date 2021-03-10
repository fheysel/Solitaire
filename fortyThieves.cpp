//============================================================================
// Name        : fortyThievesAttempt1.cpp
// Author      : Fiona Heysel
// Version     : 1

//============================================================================
#include "fortyThievesAttempt1.h"

#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stack>


using namespace std;

//Global Variables

//Each array specifies the next cards we can play in each suit's sorted pile
int clubNext[2];
int diamondNext[2];
int heartNext[2];
int spadeNext[2];

//The algorithm runs until at least 15 cards have been moved in the autostart function.
int moveCount;//move count keeps track of how many moves to final position have occurs per game
int interMoveCount;//keeps track of how many moves to intermediate positions occur per game
int loopCount = 40;//the number of times to run the algorithm so as to find an average
vector<vector<int>> histogram(loopCount, vector<int>(30, 0));//creates 30 slots of zero. 30 was selected as it is highly unlikely that the autostart could lay 75% of the cards

void setUpSeed(){//set up random seed so shuffle is different each tiem
	srand(time(0));
}

vector<card> createDeck(){//creates two decks in sorted order. Only one deck is created for the entirety of the program
	vector<card> deck;
	int suitIndicator = 0;//loops through the 4 suits
	int cardValue = 1;//the value the card will become
	string requiredSuit;//the suit that the card will become

	for(int i=0; i<104; i++){//104 as forty thieves plays with two full decks

		switch(suitIndicator){//choose suit
			case 0 :
				requiredSuit = "club";
				break;
			case 1 :
				requiredSuit = "diamond";
				break;
			case 2 :
				requiredSuit = "heart";
				break;
			case 3 :
				requiredSuit = "spade";
				break;
		}
		suitIndicator = (suitIndicator+1)%4; //update counter so next suit is assigned properly

		if((i%4) == 0 && i != 0){//ensure that each suit gets a value
			cardValue++;
			if(cardValue == 14)
				cardValue = 1;
			}

		card theCard;
		theCard.suit = requiredSuit;
		theCard.value = cardValue;
		theCard.intermediate = false;
		deck.push_back(theCard);
	}
	return deck;
}


void shuffle(vector<card> &deck){//shuffles the deck
	random_shuffle(deck.begin(), deck.end());
}

void dealCards(vector<card> &deck, table &table){//lays out cards in the sorted piles
	 for(int i = 0; i<4; i++){
		 for(int j = 0; j<10; j++){
			 table.unsortedPiles[j].push(deck.at(i+j));
		 }
	 }
}

int checkCard(table &table, int pileNum){//checks if a card can be moved to a sorted pile. Returns the pile number where it can be moved to. Returns -1 if it cannot be moved
	if (!table.unsortedPiles[pileNum].empty()) {//if the stack is not empty check if the card can be played
		string cardSuit = table.unsortedPiles[pileNum].top().suit; //determine the suit of the card in question.
		int cardValue = table.unsortedPiles[pileNum].top().value; //determine the suit of the card in question.

		if (cardSuit == "club") {
			if (cardValue == clubNext[0]) {
				clubNext[0] += 1;
				return 1; //the first club pile
			}
			if (cardValue == clubNext[1]) {
				clubNext[1] += 1;
				return 5; //second club pile
			}
		} else if (cardSuit == "diamond") {
			if (cardValue == diamondNext[0]) {
				diamondNext[0] += 1;
				return 0; //the first diamond pile
			}
			if (cardValue == diamondNext[1]) {
				diamondNext[1] += 1;
				return 4; //second diamond pile
			}
		} else if (cardSuit == "heart") {
			if (cardValue == heartNext[0]) {
				heartNext[0] += 1;
				return 2; //the first heart pile
			}
			if (cardValue == heartNext[1]) {
				heartNext[1] += 1;
				return 6; //second heart pile
			}
		} else if (cardSuit == "spade") {
			if (cardValue == spadeNext[0]) {
				spadeNext[0] += 1;
				return 3; //the first spade pile
			}
			if (cardValue == clubNext[1]) {
				spadeNext[0] += 1;
				return 7; //second spade pile
			}
		}
	}
	return -1;//no match indicator
}

void moveCard(table &table, int sendingPile, int recievingPile){//moves a card to its correct final position
	card tempCard = table.unsortedPiles[sendingPile].top();
	table.unsortedPiles[sendingPile].pop();
	table.sortedPiles[recievingPile].push(tempCard);
	return;
}

int checkIntermediate(table& table, int checkPile){//returns index in unsortedPiles of the intermediate pile. Returns -1 for no op
	if(!table.unsortedPiles[checkPile].empty()){
		if(table.unsortedPiles[checkPile].top().intermediate == true){
			return -1;
		}
		int cardValue = table.unsortedPiles[checkPile].top().value;
		string cardSuit = table.unsortedPiles[checkPile].top().suit;
		string otherSuit;
		for(int i=0; i<10; i++){
			if(table.unsortedPiles[i].empty()){
				break;
			}
			otherSuit = table.unsortedPiles[i].top().suit;
			if(otherSuit.compare(cardSuit) == 0){//if the suits are the same
				if((table.unsortedPiles[i].top().value - 1) == cardValue){//if the suits are the same and the other card is one greater than
					return i;
				}
			}
		}
	}
	return -1;
}

void autoStart(table &table){//uses heuristic to lay out the cards. If a card can be moved to a final or intermediate position than it is.
	for(int i = 0; i<2; i++){//initialize ace as the next card that can be played on the sorted piles
		clubNext[i] = 1;
		diamondNext[i] = 1;
		heartNext[i] = 1;
		spadeNext[i] = 1;
	}

	bool noAction;//boolean to indicate that no cards were laid in a pass of all 10 piles. If noAction is true the loop will end
	int recievingPile;//the indicator of which pile to move a card to. If no action should be completed it is set to -1.

	do{//loop until a pass of all the cards has resulted in no moves
		noAction = true;
		for(int pileInQuestion=0; pileInQuestion<10; pileInQuestion++){//check all 10 unsorted piles to see if a move to final position can be made
			recievingPile = checkCard(table, pileInQuestion);
			if (recievingPile != -1) {
				moveCard(table, pileInQuestion, recievingPile);
				noAction = false;//a card was moved therefore we must loop again.
				moveCount++;
			}else {//check if a move to an intermediate position can be made
				int intermediatePile = checkIntermediate(table, pileInQuestion);
				if(intermediatePile != -1){
					card temp = table.unsortedPiles[pileInQuestion].top();
					table.unsortedPiles[pileInQuestion].pop();
					table.unsortedPiles[intermediatePile].push(temp);
					table.unsortedPiles[intermediatePile].top().intermediate = true;
					interMoveCount++;
					noAction = false;
				}
			}
		}
	}while(!noAction);
}

table::table(){}//empty constructor

void table::reset(){//clears all table stacks so that the game can reset
	for(int i=0; i<10;i++){//clean up all the unsorted piles
		while(!this->unsortedPiles[i].empty()){
				this->unsortedPiles[i].pop();
			}
	}
	for(int i=0; i<8; i++){ //clean up all the sorted piles
		while(!this->sortedPiles[i].empty()){
			this->sortedPiles[i].pop();
		}
	}
}

int main() {
	setUpSeed();//set up random seed

	cout << "Welcome to the game"<<endl<<endl;

	vector<card> deck;//the vector that will hold the cards
	deck = createDeck();//populate the deck with cards
	table table; //create table class
	int dealCount = 0;//counts the number of deals it takes to autostart at least 15 cards
	double start, finish;//timer variables
	int moveCountTotal;

	for(int i=0; i<loopCount;i++){//run the autostart function x number of times to find an average
		dealCount = 0;//reset the number of deals
		moveCountTotal = 0;
		start = clock();
		do{
			dealCount++;
			moveCount = 0;//reset the count of how many cards were moved in the autostart function.
			interMoveCount = 0;//reset the count of how many times cards were moved in the autostart function
			shuffle(deck);//shuffle the cards
			table.reset();//clear the table
			dealCards(deck, table);//deal the cards
			autoStart(table);//run the greedy heuristic

			histogram.at(i).at(moveCount)++;
			moveCountTotal += moveCount;

			//printing results
//			if (moveCount >= 15) {
//				for (int i = 0; i < 8; i++) { //clean up all the sorted piles
//					while (!table.sortedPiles[i].empty()) {
//						cout << "Pile " << i << " "
//								<< table.sortedPiles[i].top().value << " of "
//								<< table.sortedPiles[i].top().suit << "s" << endl;
//						table.sortedPiles[i].pop();
//					}
//				}
//				cout<<"This took "<<dealCount<<" deals to find a solution"<<endl;
//			}
		}while(moveCount<15);//repeat until 15 cards have successfully been moved
		finish = clock();
		// Readible Output
		//cout<<"Completed trial "<<i+1<<" of "<< loopCount<<". This trial took "<<finish-start<<"ms and "<< dealCount<<" deals"<<endl;

		//Excel Output
		cout<<finish-start<<"\t"<<dealCount<<"\t"<<moveCountTotal<<endl;
	}


	//Printing statistics for excel analysis
//	cout<<"This moved "<<moveCount<<" cards to their final position"<<endl;
//	cout<<"It also moved "<<interMoveCount<<" cards to an intermediate position"<<endl;

	cout<<"\n\nPrinting histogram results: "<<endl;

	vector<int> average (30, 0);

	for(int i=0; i<loopCount; i++){
		for(int j=0; j<histogram.at(i).size(); j++){
			average.at(j) += histogram.at(i).at(j);
		}
	}
	for(int value : average){
		value /= loopCount;
		cout<<value<<endl;
	}

	return 0;
}
