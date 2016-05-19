#include<iostream> // 2016/01/05 12:33AM
using namespace std;

class Person{
	public:
		Person();
		Person(int i, int e);
		~Person();
		friend void trav(Person* root, int indent);
		friend void trav_destruct (Person* root, int indent);
	private:
		Person *parent, *child, *sibling; // the pointers to his parent, child, sibling
		int id; // record Person's id
		int energy; // record Person's energy
		int degree;// record how many levels of this Person's children
		friend class Promenade;//you could access all valuables in class Promenade
};

void trav(Person* root, int indent = 0){
	if (root == NULL) return;
	for (int i = 0; i < indent; i++)
		cout << "  ";
	cout << root->id << ' ' << root->energy << ' ' << root->degree << endl;
	Person* tmp = root->child;
	while (tmp != NULL) {
		trav(tmp, indent + 1);
		tmp = tmp ->  sibling;
	}
}
void trav_destruct (Person* root, int indent = 0) {
	if (root == NULL) return;
	Person* tmp = root->child;
	while (tmp != NULL) {
		Person* next = tmp -> sibling;
		trav_destruct (tmp, indent + 1);
		tmp = next;
	}
	delete root;
}
//constructor
Person::Person(){
	parent = child = sibling = NULL;
	id = 0;
	energy = 0;
	degree = 0;
}

//constructor
Person::Person(int i, int e){// define a new Person with id = i and energy = e
	parent = child = sibling = NULL;
	id = i;
	energy = e;
	degree = 0;
}

//destructor
Person::~Person(){

}

class Promenade{

	public:
		Promenade();
		Promenade(Person* g);
		~Promenade();
		void one_person_joined(int id, int energy);
		void one_group_joined(Promenade *g);
		void absorb_energy(int id, int energy);
		int  calculate_people_below(int energy);
		int  get_weakest_person();
		int  size();
		
		Person* merge(Person * a, Person * b);
		void trav_heap();
		int trav_count(Person* root, int energy);
		Person* trav_find (Person* root, int id);
	private:
		Person *head;
		int num;//caculate how many people in the promenade
};


//ctor
Promenade::Promenade(){
	head = NULL;
	num  = 0;
}
Promenade::Promenade(Person* g) {
	head = g;
}
//dtor
Promenade::~Promenade(){
	if (head == NULL) return ;
	Person* cur, * next;
	for (cur = head,next = head -> sibling; next != NULL; cur = next, next = next -> sibling) {
		trav_destruct (cur) ;
	}
	trav_destruct (cur);

}

void Promenade::one_person_joined(int id, int energy){
	//TODO: fill in the code to simulate a person with his id and energy to join the Promenade
	Person * new_person = new Person (id, energy);
	new_person -> sibling = head;
	head = new_person;
	Person * cur = head; 
	while (cur -> sibling != NULL && cur -> sibling -> degree == cur -> degree) {
		Person * pre = head;
		Person * cur_sibsib = cur -> sibling -> sibling;
		cur = merge (cur, cur -> sibling);
		cur -> sibling = cur_sibsib;
		//head = cur;
		if (pre == head) head = cur;
		else pre -> sibling = cur;
	}
	num += 1;
}
void Promenade::one_group_joined(Promenade *g){
	//TODO: fill in the code to simulate one group to join the full promenade
	num += g -> num;
	Person * a_cur = head, * b_cur = g -> head, * cur = NULL, * pre = NULL;
	head = NULL;
	while (a_cur != NULL && b_cur != NULL) {
		Person * to_merge = NULL;
		if (a_cur -> degree == b_cur -> degree) {
			Person * a_next = a_cur -> sibling, * b_next = b_cur -> sibling;
			to_merge = merge(a_cur, b_cur);
			a_cur = a_next; b_cur = b_next;
		}
		else if (a_cur -> degree < b_cur -> degree) {
			to_merge = a_cur;
			a_cur = a_cur -> sibling;
		}
		else if (a_cur -> degree > b_cur -> degree) {
			to_merge = b_cur;
			b_cur = b_cur -> sibling;
		}
		if (head == NULL) {
			head = to_merge;
			cur = head;
		}
		else if (to_merge -> degree == cur -> degree) {
			cur = merge (cur, to_merge);
			if (pre == NULL) {
				head = cur;	
			}
			else {
				pre -> sibling = cur;
			}
		}
		else {
			cur -> sibling = to_merge;
			pre = cur;
			cur = cur -> sibling;
		}
	}
	Person * left = a_cur == NULL ? b_cur : a_cur;
	if (head == NULL) {
		head = left;
		return ;
	}
	while (left != NULL) {
		if (left -> degree == cur -> degree) {
			Person * left_next = left -> sibling;
			cur = merge (cur, left);
			left = left_next;
			if (pre == NULL) head = cur;
			else pre -> sibling = cur;
		}
		else {
			cur -> sibling = left;
			left = left -> sibling;
			pre = cur;
			cur = cur -> sibling;
		}
	}
	g -> head = NULL;
}

void Promenade::absorb_energy(int id, int energy){
	//TODO: Elizabeth absord the energy of the person with this id, this Person's energy will decrease by this number.
	//		If his energy becomes lower than or equal to zero, he will die and should be thrown out automatically
	Person* found = NULL;
	Person* cur = head, * pre = NULL;
	for (cur = head, pre = NULL; cur != NULL; pre = cur, cur = cur -> sibling) { // find id
		found = trav_find(cur, id);	
		if (found != NULL) break;
	}
	if (found == NULL) return ; // not found -> don't do anything
	found -> energy -= energy;
	while (found -> parent != NULL && found -> energy < found -> parent -> energy) { // move to the right place
		int tmp_eng = found -> parent -> energy;
		found -> parent -> energy = found -> energy;
		found -> energy = tmp_eng;
		int tmp_id = found -> parent -> id;
		found -> parent -> id = found -> id;
		found -> id = tmp_id;
		found = found -> parent;
	} 
	if (found -> energy > 0) return ; // energy not > 0, don't delete
	if (pre == NULL) head = found -> sibling;
	else pre -> sibling = found -> sibling;

	Person* to_merge = found -> child;
	num --;
	delete found;
	if (to_merge == NULL) return ;
	to_merge -> parent = NULL;
	if (to_merge == NULL) return ;
	Person* mpre = NULL, * mcur = to_merge, * mnxt = to_merge -> sibling;
	while (mnxt != NULL) {
		mcur -> sibling = mpre;
		mcur -> parent = NULL;
		to_merge = mcur;
		mpre = mcur; mcur = mnxt; mnxt = mnxt -> sibling;
	}
	mcur -> sibling = mpre;
	mcur -> parent = NULL;
	to_merge = mcur;
	Promenade*  tmp = new Promenade(to_merge);
	one_group_joined (tmp);	

}

int Promenade::calculate_people_below(int energy){
	//TODO: You should calculate how many people's energy are below the number (include the number) and return it
	int sum = 0;
	for (Person* cur = head; cur != NULL; cur = cur -> sibling) {
		sum += trav_count(cur, energy);	
	}
	return sum;
}

int Promenade::get_weakest_person(){
	//TODO: return the id of the one with the least energy left. If there are more than one solution, just return one of them
	if (head == NULL) return -1;
	int min = head -> energy, id = head -> id;
	for (Person * cur = head; cur != NULL; cur = cur -> sibling) {
		if (min > cur -> energy) {
			min = cur -> energy;
			id = cur -> id;
		}
	}
	return id;
}

int Promenade::size(){
	return num;
}
Person *  Promenade::merge(Person * a, Person * b) {
	Person * less_eng = a -> energy <= b -> energy ? a : b;
	Person * more_eng  = a -> energy > b -> energy ? a : b;
	more_eng -> sibling = less_eng -> child;
	more_eng -> parent = less_eng;
	less_eng -> sibling = NULL;
	less_eng -> child = more_eng;
	less_eng -> parent = NULL;
	less_eng -> degree += 1;
	return less_eng;
} 
void Promenade::trav_heap () {
	for (Person * cur = head; cur != NULL; cur = cur -> sibling) {
		trav(cur, 0);
		cout << endl;
	}
}
int Promenade::trav_count (Person* root, int energy) {
	int sum = 0;
	if (root == NULL) return 0;
	Person* tmp = root -> child;
	while (tmp != NULL) {
		sum += trav_count (tmp, energy);
		tmp = tmp -> sibling;
	}
	if (root -> energy <= energy) sum += 1;
	return sum;
}
Person* Promenade::trav_find (Person* root, int id) {
	if (root == NULL) return NULL;
	if (root -> id == id) return root;
	Person* tmp = root -> child;
	Person* found = NULL;
	while (tmp != NULL && found == NULL) {
		found = trav_find (tmp, id);
		tmp = tmp -> sibling;
	}
	return found;
}
