#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<stack>
#include<vector>
#include<queue>
#include<algorithm>
#include<map>

#define WT_MAXNODE -999
#define WT_MINNODE 999

using namespace std;

//enumeration of turns
enum eTurn {X = -1, O = 1};

//enumeration of move directions.
enum eDir  {N=0, NE, E, SE, S, SW, W, NW};


int passcount = 0;

//root's direction
eTurn	root_dir;

string name_lookup[8] = { "a", "b", "c", "d", "e", "f", "g", "h" };

string number_lookup[8] = { "1", "2", "3", "4", "5", "6", "7", "8" };

//Lookup table for evaluation function.
int evaluation_look_up[8][8] = { { 99,	-8,		8,	6,	6,	8,	-8,		99	},
								 { -8,	-24,	-4, -3, -3, -4, -24,	-8	},
								 { 8,	-4,		7,	4,	4,	7,	-4,		8	},
								 { 6,	-3,		4,	0,	0,	4,	-3,		6	},
								 { 6,	-3,		4,	0,	0,	4,	-3,		6	},
								 { 8,	-4,		7,	4,	4,	7,	-4,		8	},
								 { -8,	-24,	-4, -3,	-3, -4, -24,	-8	},
								 { 99,	-8,		8,	6,	6,	8,	-8,		99	},
};


//structure to store last played position for priority queue sorting.
class tlastposition {
public:

	int x;
	int y;

	tlastposition() {
		x = -1;
		y = -1;
	}

	tlastposition(int xx, int yy) {
		x = xx;
		y = yy;
	}

};



//compare for map
class EquateLegalMove {

public:
	
	bool operator() (const tlastposition & last1, const tlastposition & last2) {

		if (last1.x == last2.x)
			return (last1.y < last2.y);
		else
			return (last1.x < last2.x);
	}

};


//class of possible states 
class node {

public :

	char			reversi_board[8][8];
	int				weight;
	eTurn			turn;
	tlastposition	lastmove;
	string			name;
	bool			nopieceleft;
	//childidentifier *	child;
	
	

	node () {
		weight		= -999;
		turn		= X;
		lastmove.x	= -1;
		lastmove.y	= -1;
		name		= "";
		nopieceleft = false;
		//child = NULL;
		
		
	}

		
};

//map<childidentifier, node, EquateSolMap> solution_map;

void MinValue(node &, int);
void MaxValue(node &, int);


void ABMinValue(node &, int, int, int);
void ABMaxValue(node &, int, int , int);

int gCutOff;

//compare function for priority queue
class EquatePQ {

public:

	bool operator() (const node & node1, const node & node2) {

		if (node1.weight == node2.weight) {

			if (node1.lastmove.x == node2.lastmove.x)
				return (node1.lastmove.y > node2.lastmove.y);
			else
				return (node1.lastmove.x > node2.lastmove.x);
		}
		else {
			return node1.weight > node2.weight;
		}
	}


};


//The structure stores the a legal move cell's registered sources.
class tLegalSrcReg {

public:

	int		srcx;
	int		srcy;
	eDir	srcdir;

	tLegalSrcReg() {
		srcx = -1;
		srcy = -1;
		srcdir = N;
	}

	tLegalSrcReg(int psrcx, int psrcy, eDir psrcdir) {
		srcx	= psrcx;
		srcy	= psrcy;
		srcdir	= psrcdir;
	}

};

//A map of vectors to store which stores all possible  legal moves.
//Inner array will store the source cells for which destination cell is legal.
map <tlastposition,vector<tLegalSrcReg>, EquateLegalMove>	vLegalMove;



//generate legal moves
void GenerateLegalMoves(node & pSourceState) {
	
		vector <tLegalSrcReg>	templegalmoves;
		char					currturn;
		int						iterx, itery;
		int						i, j;

	if (pSourceState.turn == X)
		currturn = 'X';
	else
		currturn = 'O';

	for (iterx = 0; iterx < 8; ++iterx){
		
		for (itery = 0; itery < 8; ++itery) {

			if (pSourceState.reversi_board[iterx][itery] == currturn) {

				bool invert = false;

				//North
				for (i = iterx - 1, j = itery; i >= 0; --i) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be South.
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, S));
						break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;

				//North East
				for (i = iterx - 1, j = itery + 1; i >= 0 && j < 8; --i, ++j) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be South-West
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, SW)); break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}




				invert = false;

				//East
				for (i = iterx, j = itery + 1; j < 8; ++j) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be West
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, W)); break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}


				invert = false;

				//South-East
				for (i = iterx + 1, j = itery + 1; i < 8 && j < 8; ++j, ++i) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be North-West
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, NW));
						break;
					}	
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;

				//South
				for (i = iterx + 1, j = itery; i < 8; ++i) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be North
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, N));
						break;
					} else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;

				//South-West
				for (i = iterx + 1, j = itery - 1; i < 8 && j >= 0; --j, ++i) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be NE
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, NE));
						break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;

				//West
				for (i = iterx, j = itery - 1; j >= 0; --j) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be East
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, E)); break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;

				//North-West
				for (i = iterx - 1, j = itery - 1; i >= 0 && j >= 0; --j, --i) {

					//case : same tile encountered
					if (pSourceState.reversi_board[i][j] == currturn)
						break;

					//case: blank tile encountered, but only if inversion has happened
					if (pSourceState.reversi_board[i][j] == '*' && invert) {
						//case : direction of source relative to legal move will be South West
						vLegalMove[tlastposition(i, j)].push_back(tLegalSrcReg(iterx, itery, SE)); break;
					}
					else if (pSourceState.reversi_board[i][j] == '*' && !invert)
						break;

					//case: opposite tile encountered and inversion not done
					if (pSourceState.reversi_board[i][j] != '*' && pSourceState.reversi_board[i][j] != currturn && !invert)
						invert = true;
				}

				invert = false;


				//ready for iterx, itery where iterx and itery were source positions. 

			}//end if where current turn tile was found



		}//end for inner dimension

	}//end for outer dimension


}


//state name
void GetNodeName(node & pNode) {
	
	string tempstring = "";

	tempstring = name_lookup[pNode.lastmove.y] + number_lookup[pNode.lastmove.x];

	pNode.name = tempstring;

}

//state weight
void GetNodeWeight(node & pNode) 
{
	int		my_wt = 0;
	int		op_wt = 0;

	char mychar = root_dir == X ? 'X' : 'O';
	char opchar = root_dir == X ? 'O' : 'X';

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (pNode.reversi_board[i][j] == mychar)
				my_wt += evaluation_look_up[i][j];
			if (pNode.reversi_board[i][j] == opchar)
				op_wt += evaluation_look_up[i][j];
		}
	}
	
	pNode.weight =  my_wt - op_wt;


}

//flip and fill state with new move
void FlipAndCreateNewState(node & pNode, map <tlastposition, vector<tLegalSrcReg>, EquateLegalMove>::iterator pIt) 
{
	

	//begin vector iterator for all registered sources
	vector<tLegalSrcReg>::iterator vit = pIt->second.begin();
	
	
	// fill reversi board for each registered source
	for (; vit != pIt->second.end(); ++vit) {

		//get source's direction
		eDir	dir = vit->srcdir;

		char	currturn, parturn;
		int		i, j;

		//decide which character to be flipped
		//if my turn is X then parent would have placed O
		//and if my turn is O then parent would have placed X
		if (pNode.turn == X)
			parturn = 'O';
		else
			parturn = 'X';

		//Starting point for legal move.
		i = pNode.lastmove.x;
		j = pNode.lastmove.y;

		//Depending on parent's direction place X or O in partunn on all tiles in the direction from legal move to parent.
		//all inclusive.
		switch (dir)
		{
		case N:
			//North means parent is north of legal move
			for (; i >= vit->srcx; --i) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case NE:
			//North East
			for (; i >= vit->srcx && j <= vit->srcy; --i, ++j) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case E:
			for (; j <= vit->srcy; ++j) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case SE:
			//South-East
			for (; i <= vit->srcx && j <= vit->srcy; ++j, ++i) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case S:
			//South
			for (; i <= vit->srcx; ++i) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case SW:
			//South-West
			for (; i <= vit->srcx && j >= vit->srcy; --j, ++i) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case W:
			for (; j >= vit->srcy; --j) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		case NW:
			//North-West
			for (; i >= vit->srcx && j >= vit->srcy; --j, --i) {
				pNode.reversi_board[i][j] = parturn;
			}
			break;
		default:
			break;
		}

	}
	
	
}

bool CheckSinglePieceCondition(node & pNode) {


	int		my_wt = 0;
	int		op_wt = 0;

	char mychar = root_dir == X ? 'X' : 'O';
	char opchar = root_dir == X ? 'O' : 'X';

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (pNode.reversi_board[i][j] == mychar)
				my_wt += 1;
			if (pNode.reversi_board[i][j] == opchar)
				op_wt += 1;

			if (my_wt > 0 && op_wt > 0)
				return false;

		}
	}



	return true;

}

bool CheckAllBoardFilledCondition(node & pNode) {


	int		blank_wt = 0;
	

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {

			if (pNode.reversi_board[i][j] == '*')
				return false;
			

		}
	}



	return true;

}

//function to prepare child state into a priority queue
void GenerateChildren (node & parentstate, priority_queue<node, deque<node>, EquatePQ> & pMyQ, bool pIsLeaf = false)
{

	//node childstate;
	
	//clear the map
	vLegalMove.clear();

	//get legal moves
	GenerateLegalMoves(parentstate);

	//case: no legal moves
	if (vLegalMove.empty()) {

		bool nopieceleft = CheckSinglePieceCondition(parentstate);

		nopieceleft |= CheckAllBoardFilledCondition(parentstate);

		node childstate;

		//if no piece left means there are either no X or no O so game ends no possible moves
		//it is a special pass which results in nothing
		childstate.nopieceleft = nopieceleft;

		//update last move 
		childstate.lastmove.x = parentstate.lastmove.x;
		childstate.lastmove.y = parentstate.lastmove.y;
		//give name
		childstate.name = "pass";
		//eval turn
		childstate.turn = parentstate.turn == X ? O : X;
		//copy reversi board
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				childstate.reversi_board[i][j] = parentstate.reversi_board[i][j];
			}
		}

		 
		if (pIsLeaf)
			GetNodeWeight(childstate);
		else
			childstate.weight = -(parentstate.weight);

		//now new state is completely generated , add it to parent q
		pMyQ.push(childstate);

		return;


	}

	//Now vLegalMove has all the legal moves.
	//navigate and create children
	map <tlastposition, vector<tLegalSrcReg>, EquateLegalMove>::iterator it;
	
	// fill children state
	for (it = vLegalMove.begin(); it != vLegalMove.end(); ++it) {

		node childstate;

		//update last move 
		childstate.lastmove.x = it->first.x;
		childstate.lastmove.y = it->first.y;
		//give name
		GetNodeName(childstate);
		//eval turn
		childstate.turn = parentstate.turn == X ? O : X;
		//copy reversi board
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				childstate.reversi_board[i][j] = parentstate.reversi_board[i][j];
			}
		}

		//flip the places 
		FlipAndCreateNewState(childstate, it);

		//set weight 
		if (pIsLeaf)
			GetNodeWeight(childstate);
		else
			childstate.weight = -(parentstate.weight);

		//now new state is completely generated , add it to parent q
		pMyQ.push(childstate);


	}
		
		
}


//calculate the weight of legal moves

//Prioritize them in order
/*
1. for greedy the cost
2. for minimax the ordering of move position
*/

class lognode {
public:

	string	name;
	int		depth;
	int		weight;

	lognode(){}

	lognode(string pname, int pdepth, int pweight)
	{
		name	= pname;
		depth	= pdepth;
		weight	= pweight;
	}
};

//Log keeper
queue<lognode> logger;

void WriteMinMaxLog(fstream & ofile) {

	lognode test;

	ofile << "Node,Depth,Value" << endl;

 	while (!logger.empty())  {

			test = logger.front();
			logger.pop();
			ofile << test.name << "," << test.depth << ",";

			if (test.weight == 999)
				ofile << "Infinity";
			else if (test.weight == -999)
				ofile << "-Infinity";
			else
				ofile << test.weight;

			if (!logger.empty())
				ofile << endl;

	}

}

stack<node> next_state;

/********************************************************MINIMAX-START***************************************************/
void MiniMax(node & pStart, int pCut)
{
	MaxValue(pStart, pCut);

}

void MaxValue(node & pState, int pCut)
{
	if (pCut == 0 || passcount == 2) {
		GetNodeWeight(pState);
		//log current state of parent
		logger.push(lognode(pState.name, gCutOff -  pCut, pState.weight));
		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		passcount = 0;
		return;

	}

	int v = WT_MAXNODE;
	priority_queue<node, deque<node>, EquatePQ> myq;
	node tempchild;
	int  minweight;

	//Generate children into a queue
	GenerateChildren(pState, myq, pCut?false: true);

	//check if special pass child found.
	if (myq.top().name == "pass" && myq.top().nopieceleft == true) {
		GetNodeWeight(pState);
		//log current state of parent
		logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

		if (pState.name == "root")
			next_state.push(pState);

		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		return;

	}

	//log current state of parent
	logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

	while (!myq.empty())
	{
			
		//run algo on children
		tempchild = myq.top();

		myq.pop();


		if (tempchild.name == "pass")
			++passcount;
		else
			passcount = 0;

		//run algo on this state
		MinValue(tempchild, pCut - 1);

		//update in parent 
		if (tempchild.weight > pState.weight) {
			pState.weight = tempchild.weight;
			if (pState.name == "root")
				next_state.push(tempchild);

			//pState.child = new childidentifier(tempchild.name, gCutOff - pCut + 1, tempchild.lastmove);

		}
		//log state of parent
		logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

	}

	//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;



}


void MinValue(node & pState, int pCut)
{
	if (pCut == 0 || passcount == 2) {
		GetNodeWeight(pState);
		//log current state of parent
		logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));
		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		passcount = 0;
		return;

	}
	int v = WT_MAXNODE;
	priority_queue<node, deque<node>, EquatePQ> myq;
	node tempchild;
	int  minweight;

	//Generate children into a queue
	GenerateChildren(pState, myq, pCut ? false : true);

	//check if special pass child found.
	if (myq.top().name == "pass" && myq.top().nopieceleft == true) {
		GetNodeWeight(pState);
		//log current state of parent
		logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

		if (pState.name == "root")
			next_state.push(pState);

		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		return;

	}

	//log current state of parent
	logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

	while (!myq.empty())
	{
		

		//run algo on children
		tempchild = myq.top();
		myq.pop();


		if (tempchild.name == "pass")
			++passcount;
		else
			passcount = 0;

		//run algo on this state
		MaxValue(tempchild, pCut - 1);

		//update in parent 
		if (tempchild.weight < pState.weight) {
			pState.weight = tempchild.weight;
			if (pState.name == "root")
				next_state.push(tempchild);
			//pState.child = new childidentifier(tempchild.name, gCutOff - pCut + 1, tempchild.lastmove);

		}
		//log state of parent
		logger.push(lognode(pState.name, gCutOff - pCut, pState.weight));

	}

	//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;

}




/*******************************************************MINIMAX-END*****************************************************/

/*******************************************************ALpha-Beta********************************************************/

class ABlognode {
public:

	string	name;
	int		depth;
	int		weight;
	int		alpha;
	int		beta;

	ABlognode(){}

	ABlognode(string pname, int pdepth, int pweight, int pal, int pbe)
	{
		name = pname;
		depth = pdepth;
		weight = pweight;
		alpha = pal;
		beta = pbe;
	}
};

//Log keeper
queue<ABlognode> ABlogger;



void WriteAlphaBetaLog(fstream & ofile) {

	ABlognode test;

	ofile << "Node,Depth,Value,Alpha,Beta" << endl;

	while (!ABlogger.empty())  {

		test = ABlogger.front();
		ABlogger.pop();
		ofile << test.name << "," << test.depth << ",";

		if (test.weight == 999)
			ofile << "Infinity";
		else if (test.weight == -999)
			ofile << "-Infinity";
		else
			ofile << test.weight;

		ofile << ",";

		if (test.alpha == 999)
			ofile << "Infinity";
		else if (test.alpha == -999)
			ofile << "-Infinity";
		else
			ofile << test.alpha;

		ofile << ",";

		if (test.beta == 999)
			ofile << "Infinity";
		else if (test.beta == -999)
			ofile << "-Infinity";
		else
			ofile << test.beta;

		if (!ABlogger.empty())
			ofile << endl;

	}

}

bool STOPLOG = false;

void AlphaBeta(node & pStart, int pCut)
{
	ABMaxValue(pStart, pCut, WT_MAXNODE, WT_MINNODE);

}

void ABMaxValue(node & pState, int pCut, int alpha, int beta)
{
	

	if (pCut == 0 || passcount == 2) {
		GetNodeWeight(pState);
		//log current state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));
		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		passcount = 0;
		return;

	}

	int v = WT_MAXNODE;
	priority_queue<node, deque<node>, EquatePQ> myq;
	node tempchild;
	int  minweight;
	

	//Generate children into a queue
	GenerateChildren(pState, myq, pCut ? false : true);

	//check if special pass child found.
	if (myq.top().name == "pass" && myq.top().nopieceleft == true) {
		GetNodeWeight(pState);
		//log current state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));

		if (pState.name == "root")
			next_state.push(pState);

		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		return;

	}

	//log current state of parent
	ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));

	while (!myq.empty())
	{

		//run algo on children
		tempchild = myq.top();

		myq.pop();

		if (tempchild.name == "pass")
			++passcount;
		else
			passcount = 0;

		//run algo on this state
		ABMinValue(tempchild, pCut - 1, alpha, beta);

		//update in parent 
		if (tempchild.weight > pState.weight) {
			pState.weight = tempchild.weight;
			if (pState.name == "root")
				next_state.push(tempchild);

			//pState.child = new childidentifier(tempchild.name, gCutOff - pCut + 1, tempchild.lastmove);

		}


		if (tempchild.weight >= beta) {
			//log state of parent
			ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));
			return;
		}
		if (tempchild.weight > alpha) {
			alpha = tempchild.weight;
		}

		//log state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));


		

	}

	//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;



}


void ABMinValue(node & pState, int pCut, int alpha, int beta)
{
	if (pCut == 0 || passcount == 2) {

		GetNodeWeight(pState);
		//log current state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));
		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;

		if (pState.name == "root")
			next_state.push(pState);

		passcount = 0;
		return;

	}
	int v = WT_MAXNODE;
	priority_queue<node, deque<node>, EquatePQ> myq;
	node tempchild;
	int  minweight;

	//Generate children into a queue
	GenerateChildren(pState, myq, pCut ? false : true);

	//check if special pass child found.
	if (myq.top().name == "pass" && myq.top().nopieceleft == true) {
		GetNodeWeight(pState);
		//log current state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));
		//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;
		return;

	}

	//log current state of parent
	ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));

	while (!myq.empty())
	{


		//run algo on children
		tempchild = myq.top();
		myq.pop();

		if (tempchild.name == "pass")
			++passcount;
		else
			passcount = 0;

		//run algo on this state
		ABMaxValue(tempchild, pCut - 1, alpha, beta);

		//update in parent 
		if (tempchild.weight < pState.weight) {
			pState.weight = tempchild.weight;
			if (pState.name == "root")
				next_state.push(tempchild);
			//pState.child = new childidentifier(tempchild.name, gCutOff - pCut + 1, tempchild.lastmove);

		}
		

		if (tempchild.weight <= alpha) {
			//log state of parent
			ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));
			return;
		}

		if (tempchild.weight < beta) {
			beta = tempchild.weight;
		}

		//log state of parent
		ABlogger.push(ABlognode(pState.name, gCutOff - pCut, pState.weight, alpha, beta));

	}

	//solution_map[childidentifier(pState.name, gCutOff - pCut, pState.lastmove)] = pState;

}


/********************************************************Alpha beta ******************************************************/







int main() 
{

	//intialize legal move array

	fstream		myfile, ofile;								///< File pointer
	int			choice;										///< Gets the type of algorithm. 1 = BFS 2 = DFS 3 = UniCost.
	long long	cutoff;										///< to read number of nodes too.
	char		turn;
	char		boardp;										///< Reads edge weight
	
	//intialize start state
	node startstate;

	startstate.name = "root";

	//open the file in read mode
	myfile.open("input.txt", fstream::in | fstream::out);
	
	//Reads type of algorithm.
	myfile >> choice;
	
	//read the goal state name
	myfile >> turn;

	if (turn == 'X')
		startstate.turn = X;
	else
		startstate.turn = O;

	startstate.weight = WT_MAXNODE;

	//direction of root.
	root_dir = startstate.turn;

	myfile >> cutoff;

	gCutOff = cutoff;

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			myfile >> boardp;
			startstate.reversi_board[i][j] = boardp;

		}
	}

	myfile.close();

	GenerateLegalMoves(startstate);

	//case : Which algorithm to run
	switch (choice)
	{
		case 1:  {
			MiniMax(startstate, 1);
			break;
		}
		case 2: {
			MiniMax(startstate, cutoff);
			break;
		}
		case 3: {
			AlphaBeta(startstate, cutoff);
			break;
		}
		default:
			break;
	}

	ofile.open("output.txt", fstream::out);


	//case : Which algorithm to run
	switch (choice)
	{
	case 1:  {
		//Print next state
		node	solution = next_state.top();

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {

				ofile << solution.reversi_board[i][j];
			}
			if (i != 7)
				ofile << endl;
		}
		break;
	}
	case 2: {
		//Print next state
		node	solution = next_state.top();

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {

				ofile << solution.reversi_board[i][j];
			}
			
				ofile << endl;
		}


		WriteMinMaxLog(ofile);
		break;
	}
	case 3: {
		//Print next state
		node	solution = next_state.top();

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {

				ofile << solution.reversi_board[i][j];
			}

			ofile << endl;
		}


		WriteAlphaBetaLog(ofile);
		break;
	}
	default:
		break;
	}

	

	ofile.close();



	//Load start state.

	//prepare out put file

	//call appropriate algo

	//reset legal move array


}