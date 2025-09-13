#include <iostream>
#include <climits>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <queue>

using namespace std;

/*
employee:
	int id;
	int age;
	string name;
	string role;
	double salary;
	manager* manager;

manager:
	vector<employee*> team;

tree:
	employees salaries;
	insert.salary;
*/



class manager;
class Hr;

class employee {
	string name;
	string position;
	double salary;
	short age;
	short manager;
	short id;

	protected:
		void updateName(string newName){
			name = newName;
		}

		void updateAge(int newAge){
			age = newAge;
		}

		void updatePosition(string newPosition){
			position = newPosition;
		}

		void updateSalary(double newSalary) {
			salary = newSalary;  
		}

		employee* GetEmployeeObject(){
			return this;
		}
	public:
		// ~employee(){
		// 	this->removeCurrManager();
		// }

		employee(int _id, int _age, string _name, string _position, double _salary) {
			id = _id;
			name = _name;
			position = _position;
			age = _age;
			salary = _salary;
		}

		int getId() const {
			return id;
		}

		int getManagerId() const {
			return manager;
		}

		string getName(){
			return name;
		}

		string getPosition(){
			return position;
		}

		void bonus(double bonus) {
			salary += bonus;
		}

		void Deduction(double deduction) {
			salary -= deduction;
		}	

		void assignManger(int manager) {
			this->manager = manager;
		}

		void removeCurrManager() {
			this->manager = -1;
		}

		virtual bool isManager(){
			return 0;
		}
	
	friend class Hr;
};

class manager : public employee {
	vector<short> teamIds;

	protected:
		void unassignEmployee(int id){
			for (short i = 0; i < teamIds.size(); i++)
			{
				if(teamIds[i] == id){
					teamIds.erase(teamIds.begin() + i);
					return;
				}
				cout << "Employee not founded." << endl;
			}
		}

	public:
		manager(int _id, int _age, string _name, string _position, double _salary)
			: employee(_id, _age, _name, _position, _salary) {} 

		// ~manager(){
		// 	// for (short i = 0; i < teamIds.size(); i++)
		// 	// {
		// 	// 	teamIds[i].removeCurrManager();
		// 	// }
		// 	// this->removeCurrManager();
		// }

		bool isManager(){
			return 1;
		}

		void assignEmployee(int employeeId) {
			for (int i = 0; i < teamIds.size(); i++) {
				if (teamIds[i] == employeeId) {
					cout << "Employee already assigned.\n";
					return;
				}
			}
			cout << "Employee " << employeeId << " assigned to manager " << getId() << "\n";
			teamIds.push_back(employeeId);
		}

		void deductionEmployee(employee* employee, int amount) {
			employee->Deduction(amount);
		}

		void bonusEmployee(employee* employee, int amount) {
			employee->bonus(amount);
		}

		vector<short> getEmployeesIds(){
			return teamIds;
		}

	friend class Hr;
};

class drawEmployeesHierarchy{
		map<short, employee*> employees;

		struct employeeNode{
			int id;
			vector<employeeNode*> employees;
		};

		vector<short> getIds(int managerId){
			employee* emp = employees[managerId];
			if(emp == 0){
				cout << "Employee not found!" << endl;;
				return {};
			}

			if(!emp->isManager()){
				// cout << "This is not a manager." << endl;;
				return {};
			}

			manager* mgr = dynamic_cast<manager*>(emp);
			return mgr->getEmployeesIds();
		}

		void printTree(employeeNode* node, string prefix, bool isLast, int depth, int widthLimit) {
			if (!node || depth > 5) return; // max height = 5

			employee* emp = employees[node->id];

			// print branch
			cout << prefix;
			if (depth > 0) {
				cout << (isLast ? "└── " : "├── ");
			}

			// print employee info (ID + role for clarity)
			cout << "[name " << emp->getName() << ", Position: " << emp->getPosition() << "]\n";

			// prepare new prefix for children
			string newPrefix = prefix + (isLast ? "    " : "│   ");

			// limit width
			int count = node->employees.size();
			if (count > widthLimit) count = widthLimit;

			for (int i = 0; i < count; i++) {
				printTree(node->employees[i], newPrefix, i == count - 1, depth + 1, widthLimit);
			}

			if (node->employees.size() > widthLimit) {
				cout << newPrefix << "└── ... more ...\n";
			}
		}

	public:
		drawEmployeesHierarchy(map<short, employee*> _employees){
			employees = _employees;
		}

		~drawEmployeesHierarchy(){
		}

		employeeNode* draw(int id){
			employeeNode* topEmp = new employeeNode{id}; 
			queue<employeeNode*> q;
			q.push(topEmp);
			while(!q.empty()){
				employeeNode* current = q.front();
				q.pop();
				vector<short> tempIds = getIds(current->id);
				for (int i = 0; i < tempIds.size(); i++) {
					employeeNode* newEmp = new employeeNode{tempIds[i]};
					q.push(newEmp);
					current->employees.push_back(newEmp);
				}
			}
			return topEmp;
		}
		
		void printHierarchy(int id) {
			employeeNode* root = draw(id);
			printTree(root, "", true, 0, 8); // max width = 8
			delete root;
		}
};

class Hr {
	private:
		map<short, employee*> employees;
		short id = 0;
		
	public:
		void addEmployee(string name, string position, short age, double salary, bool isManager){
			employee* emp = nullptr;
			int currId = id++;
			if (isManager) {
				emp = new manager(currId, age, name, position, salary);
			} else {
				emp = new employee(currId, age, name, position, salary);
			}

			employees[currId] = emp;
		}

		employee* searchEmployeeById(short id) {
			if(employees[id]){
				return employees[id];
			}
			cout << "Employee not found:\n";
			return 0;
		}

		void assignEmployeeToManager(short empId, short mgrId) {
			employee* emp = searchEmployeeById(empId);
			manager* mgr = dynamic_cast<manager*>(searchEmployeeById(mgrId));
			if (emp && mgr) {
				mgr->assignEmployee(empId);
				emp->assignManger(mgrId);
				return;
			}
			cout << "Employee or Manager not found:\n";
		}

		void removeEmployee(short id) {
			employee* currEmp = employees[id];
			if(currEmp){
				if(currEmp->isManager()){
					short managerId = currEmp->getManagerId();
					manager* managerObj = dynamic_cast<manager*>(employees[managerId]);
					managerObj->unassignEmployee(id);
					
					manager* currManager = dynamic_cast<manager*>(currEmp);
					vector<short> employeesIds = currManager->getEmployeesIds();
					for (size_t i = 0; i < employeesIds.size(); i++)
					{
						employees[employeesIds[i]]->removeCurrManager();
					}
					delete currManager;
				} else {
					short managerId = currEmp->getManagerId();
					manager* managerObj =dynamic_cast<manager*>(employees[managerId]);
					managerObj->unassignEmployee(id);
					delete currEmp;
				}
				return;
			}
			cout << "Employee not found:\n";
		}

		void drawHierarchy(int id = 0) {
			drawEmployeesHierarchy employeesHierarchy(employees);
			employeesHierarchy.printHierarchy(id);
		}

		void updateEmployeeData(int empId) {
			employee* emp = searchEmployeeById(empId);
			if (!emp) {
				 cout << "Employee not found!\n";
				return;
			}

			cout << "What do you want to update?\n";
			cout << "1. Name\n2. Position\n3. Age\n4. Salary\nChoice: ";
			int choice; cin >> choice;

			switch(choice) {
				case 1: {
					string newName; 
					cout << "Enter new name: "; cin >> newName;
					emp->updateName(newName);
					break;
				}
				case 2: {
					string newPos;
					cout << "Enter new position: "; cin >> newPos;
					emp->updatePosition(newPos);
					break;
				}
				case 3: {
					int newAge;
					cout << "Enter new age: "; cin >> newAge;
					emp->updateAge(newAge);
					break;
				}
				case 4: {
					double newSalary;
					cout << "Enter new salary: "; cin >> newSalary;
					emp->updateSalary(newSalary);
					break;
				}
				default:
					cout << "Invalid choice.\n";
			}
		}

};


int main() {
	Hr myHr;
	// string name, string position, int age, double salary, bool isManager
    // Top of the company
    myHr.addEmployee("Mark", "STO", 55, 100000, 1);  // ID 0

    // Managers
    myHr.addEmployee("Abdelrahman", "BackEnd Manager", 35, 5000, 1); // ID 1
    myHr.addEmployee("Mohamed", "Front Manager", 33, 5000, 1);       // ID 2
    myHr.addEmployee("Moshrif", "Marketing Manager", 40, 5000, 1);   // ID 3

    // Seniors
    myHr.addEmployee("Ahmed", "BackEnd Senior", 28, 4000, 1);        // ID 4 (manager type)
    myHr.addEmployee("Ali", "Front Senior", 29, 4000, 1);            // ID 5 (manager type)
    myHr.addEmployee("Youssef", "Marketing Senior", 32, 4000, 1);    // ID 6 (manager type)

    // Regular juniors
    myHr.addEmployee("Khaled", "BackEnd Junior", 24, 2000, 0);       // ID 7
    myHr.addEmployee("Omar", "BackEnd Junior", 23, 2000, 0);         // ID 8
    myHr.addEmployee("Mostafa", "Front Junior", 22, 2000, 0);        // ID 9
    myHr.addEmployee("Hossam", "Front Junior", 21, 2000, 0);         // ID 10
    myHr.addEmployee("Amr", "Marketing Junior", 23, 2000, 0);        // ID 11
    myHr.addEmployee("Sara", "Marketing Junior", 22, 2000, 0);       // ID 12

    // Build hierarchy
    // STO manage Managers
    myHr.assignEmployeeToManager(1, 0);
    myHr.assignEmployeeToManager(2, 0);
    myHr.assignEmployeeToManager(3, 0);

    // Managers manage Seniors
    myHr.assignEmployeeToManager(4, 1);
    myHr.assignEmployeeToManager(5, 2);
    myHr.assignEmployeeToManager(6, 3);

    // Seniors manage Juniors
    myHr.assignEmployeeToManager(7, 4);
    myHr.assignEmployeeToManager(8, 4);
    myHr.assignEmployeeToManager(9, 5);
    myHr.assignEmployeeToManager(10, 5);
    myHr.assignEmployeeToManager(11, 6);
    myHr.assignEmployeeToManager(12, 6);

	myHr.drawHierarchy();

	// myHr.removeEmployee(6);
	// myHr.updateEmployeeData(1);
 }

