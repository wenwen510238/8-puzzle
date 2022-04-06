#include <bits/stdc++.h>
using namespace std; 

int i_search;
const int n = 3; //建立大小
bool random = false;
bool g_start = true; // 初始化
int Goal[n][n] = { { 5,8,6} ,{ 0,7,4 },{ 2,3,1 } };  //目標
int step;
void IDS(); 
void UCS();
void Astar(); 
void GBFS();
void Expand(int &); 

class State{
public:
	int A[n][n], general_cost, heuristic_cost, total_cost;  // State Array
	State *parent; //Parent Pointer.
	State(); //Constructor
	void reset(); // 初始化 
	bool is_goal(); // true: state is goal.
					// false: state is not goal.
	bool operator==(const State &) const; //Logical equality Operator.
	bool operator<(const State &) const; //Logical Less-Than Operator.
	void print(); //State Array Printing function.
	void Heurs(); //State Heuristic Cost Calculator.
};
list< State > closed, fringe; 
State start, cur, temp; 
bool RBFS(State s, int bound,int &); 
void PrintPath(State *s); //結果展示 
bool InClosed(State &s);

int main() {
	cout << "請選擇搜尋演算法 : 1:Iterative-Deepening Search (IDS)   2:Uniform-Cost Search(UCS)  "<< 
			"3:Greedy Best-First Search(GBFS) 4:A*  5:Recursive Best-First Search (RBFS)" << endl;
	while(cin>>i_search && i_search>=1 && i_search<=5){
		//space=1; runtime=1,timer=0;
		start.general_cost = 0; 
		start.Heurs(); 
		start.total_cost = start.general_cost + start.heuristic_cost; 
		start.parent = NULL; 
		if(i_search == 1)
			IDS();
		else if(i_search == 2) 
			UCS();  
		else if(i_search == 3) 
			GBFS(); 
		else if(i_search == 4) 
			Astar(); 
		else if(i_search == 5) {
			int space=1;
			RBFS(start, 1000000,space); 
			cout << "Space = " << space << "\n";
			cout << "Step = " << step << "\n";
		}
		cout << "請選擇搜尋演算法 : 1:Iterative-Deepening Search (IDS)   2:Uniform-Cost Search  "<< 
			"3:Greedy Best-First Search 4:A*  5:Recursive Best-First Search (RBFS)" << endl;
	}					
	//getchar();
	return 0;
}

State::State() {
	reset();
}

void State::reset() {
	general_cost = heuristic_cost = total_cost = 0;
	parent = NULL;
	if(g_start){
		cout<<"Goal state : "<<endl;
		for(int i=0;i<n;i++){
			for(int j=0;j<n;j++)
				cout<<Goal[i][j]<<"  ";
			cout<<endl;	
		}
		if(!random){
			cout<<"請由左到右由上到下橫向輸入起始狀態0-8(第一個輸入的數字為左上方，最後一個輸入為右下方)"<<endl;  
			for(int i=0;i<n;i++){
				for(int j=0;j<n;j++){
					cin>>A[i][j];
				}
			}	
			cout << "initial state：" << endl;
			PrintPath(&start);	
		}
		else{
			list< int > l;
			list< int >::iterator it;
			srand(time(0));
			for(int i=0;i<n*n;i++)
				l.push_back(i);
			for(int i=0;i<n;i++){
				for(int j=0;j<n;j++){
					it = l.begin();
					for(int k=rand()%l.size();k>0;k--)
						it++;
					A[i][j] = (*it);
					l.erase(it);
				}
			}
			cout << "initial state：" << endl;
			PrintPath(&start);
		}
		g_start = false;
	}
}

bool State::is_goal(){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){			
			if(A[i][j] != Goal[i][j])			
				return false;
		}
	}
	return true;
}

void State::print(){
	int i, j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			if((A[i][j]/10) == 0 )
				cout << A[i][j] << ' ' << ' ';
			else
				cout << A[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

bool State::operator==(const State &r)const{
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			if(A[i][j] != r.A[i][j])
				return false;
		}
	}
	return true;
}
bool State::operator<(const State &r)const {
	if(i_search == 1 || i_search == 2){
		return general_cost < r.general_cost; 
	}
	else if(i_search == 3){
		return total_cost < r.total_cost;
	}
	else if(i_search == 4){ 
		return heuristic_cost < r.heuristic_cost;
	}
}

void State::Heurs(){//每塊方塊與其正確位置的總和 
	int Heuristic = 0;
	bool found;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			found = false;
			for(int k=0;k<n;k++){
				for(int p=0;p<n;p++){				
					if(Goal[i][j] == A[k][p]){
						Heuristic += abs(i-k)+abs(j-p);
						found = true;
						break;
					}
				}
				if(found)	break;	
			}
		}
	}
	heuristic_cost = Heuristic;
}

//Iterative-Deepening Search (IDS)
void IDS(){
	int depth=0,space=1; 
	cout << "Starting IDS Algorithm... \n";
	while(true){	
		cur = start;
		fringe.push_front(cur);
		while (!fringe.empty()){
			cur = fringe.front();
			if (cur.is_goal()){
				cout << "Space = " << space << "\n";
				cout << "Step = " << cur.general_cost << "\n";
				cout << "Path:\n";
				PrintPath(&cur);
				return;
			} 
			else if(depth > cur.general_cost){	
				Expand(space);
			}
			else{ 
				fringe.pop_front();
			}
		}
		fringe.clear();
		closed.clear();	
		depth++;
	}
}

/*Uniform-Cost Search*/ 
void UCS(){
	int space=1;
	cout << "starting Uniform-Cost Search Algorithm... \n";
	cur = start;
	fringe.push_front(cur);
	while(true){
		cur = fringe.front();
		for (list<State>::iterator it = fringe.begin();it != fringe.end();++it) {
			if((*it)<cur){ 
				cur = (*it);
			}
		}
		
		if(cur.is_goal()){
			cout << "Space = " << space << "\n";
			cout << "Step = " << cur.general_cost << "\n";
			cout << "Path:\n";
			PrintPath(&cur);
			return;
		}
		else {
			Expand(space);
		}
	}
}

/*A* search*/ 
void Astar() {
	int space=1;
	cout << "starting A* Algorithm... \n";
	cur = start;
	fringe.push_front(cur);
	while (true) {
		cur = fringe.front();
		for(list<State>::iterator it = fringe.begin(); it != fringe.end(); ++it) {
			if ((*it) < cur ) {  
				cur = (*it);
			}
		}
		
		if(cur.is_goal()) {
			cout << "Space = " << space << "\n";
			cout << "Step = " << cur.general_cost << "\n";
			cout << "Path:\n";
			PrintPath(&cur);
			return;
		}
		else{
			Expand(space);
		}
	}
}

/*Greedy Best-First Search*/
void GBFS(){
	int space=1;
	cout << "starting GBFS Algorithm... \n";
	cur = start;
	fringe.push_front(cur);
	while(true){
		cur = fringe.front();
		for (list<State>::iterator it = fringe.begin(); it != fringe.end(); ++it) {
			if((*it) < cur ){ 
				cur = (*it);
			}
		}
		if(cur.is_goal()){	
			cout << "Space = " << space << "\n";
			cout << "Step = " << cur.general_cost << "\n";
			cout << "Path:\n";
			PrintPath(&cur);
			return;
		}
		else{
			Expand(space);
		}
	}
}

/*Recursive Best-First Search (RBFS)*/
bool RBFS(State s, int bound,int &space){
	State initial = s;
	State best, alter, temp;
	int best_f = 0, alter_f;
	bool result = 1;
	int arr[4] = {0};
	list< State > exp;
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (initial.A[i][j] == 0) {
				if (i > 0) {
					temp = initial;
					swap(temp.A[i][j], temp.A[i - 1][j]);
					temp.general_cost += 1; 
					temp.Heurs();
					temp.total_cost = temp.general_cost + temp.heuristic_cost;
					exp.push_front(temp); 
					space++;
					arr[0] = temp.total_cost;		
				}
				if (i < n-1) {
					temp = initial;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i + 1][j]);
					temp.general_cost += 1;
					temp.Heurs();
					temp.total_cost = temp.general_cost + temp.heuristic_cost;
					exp.push_front(temp);
					space++;
					arr[1] = temp.total_cost;
				}
				if (j > 0) {
					temp = initial;
					temp.parent = &(closed.back());					
					swap(temp.A[i][j], temp.A[i][j - 1]);				
					temp.general_cost += 1;
					temp.Heurs();
					temp.total_cost = temp.general_cost + temp.heuristic_cost;
					exp.push_front(temp);
					space++;
					arr[2] = temp.total_cost;
				}
				if (j < n-1) {
					temp = initial;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i][j + 1]);
					temp.general_cost += 1;
					temp.Heurs();
					temp.total_cost = temp.general_cost + temp.heuristic_cost;
					exp.push_front(temp);
					space++;
					arr[3] = temp.total_cost;
				}
			}
		}
	}
	sort(arr, arr+4);
	best_f = arr[0];
	alter_f = arr[1];
	
	for(list<State>::iterator it = exp.begin(); it != exp.end(); it++){
		if(arr[0] == (*it).total_cost){
			best = (*it);
			exp.erase(it);	
			break;
		}
	}
	
	for(list<State>::iterator it = exp.begin(); it != exp.end(); ++it){
		if(arr[1] == (*it).total_cost){
			alter = (*it);
			exp.erase(it);	
			break;
		}
	}
		
	if(best.is_goal()){
			cout << "Path:\n";
			best.print();
			step++;
			return true;
		}
		
	while(true){
		if(best.total_cost > bound){
			return false;	
		}	
		result = RBFS( best, min(bound, alter.total_cost),space);	
		if(result){
			best.print();
			step++;
			return true;
		}
		if(!result){
			alter.total_cost = best.total_cost;
			temp = best;
			best = alter;
			alter = temp;
		}
	}
}

void Expand(int &space){
	closed.push_back(cur);
	int flag=0;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){			
			if(cur.A[i][j] == 0) {	
				flag=1;			
				if(i>0){
					temp = cur;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i - 1][j]);
					if (!InClosed(temp)) {
						temp.general_cost += 1; 
						temp.Heurs();
						temp.total_cost = temp.general_cost + temp.heuristic_cost;
						fringe.push_front(temp); 
						space++;
					}
				}
				if(i<n-1){
					temp = cur;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i + 1][j]);
					if (!InClosed(temp)) {
						temp.general_cost += 1;
						temp.Heurs();
						temp.total_cost = temp.general_cost + temp.heuristic_cost;
						fringe.push_front(temp);
						space++;
					}
				}
				if(j>0){
					temp = cur;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i][j - 1]);
					if (!InClosed(temp)) {
						temp.general_cost += 1;
						temp.Heurs();
						temp.total_cost = temp.general_cost + temp.heuristic_cost;
						fringe.push_front(temp);
						space++;
					}
				}
				if(j<n-1){
					temp = cur;
					temp.parent = &(closed.back());
					swap(temp.A[i][j], temp.A[i][j + 1]);
					if (!InClosed(temp)) {
						temp.general_cost += 1;
						temp.Heurs();
						temp.total_cost = temp.general_cost + temp.heuristic_cost;
						fringe.push_front(temp);
						space++;
					}
				}
			}
			if(flag)	break;
		}
	}
	fringe.remove(cur);
}

void PrintPath(State *s) {
	if(s != NULL){
		(*s).print();
		PrintPath((*s).parent);
	}
}

bool InClosed(State &s){
	for (list<State>::iterator it = closed.begin(); it != closed.end(); ++it) {
		if((*it) == s) { 
			return true;
		}
	}
	return false;
}
