#include<iostream>
#include<string>
#include<map>
#include<set>
#include<vector>
using namespace std;

enum class Direction{
    UP,
    DOWN,
    IDLE
};

class Elevator{
    private:
        string elevatorId;
        int floorNumber;
        int capacity;
        int currWeight; 
        Direction direction;
        set<int> upStop;
        set<int, greater<int>> downStop;
        mutable mutex mtx;
    public:
        Elevator(string elevatorId,int capacity,int floorNumber,Direction direction){
            this->elevatorId=elevatorId;
            this->capacity=capacity;
            this->floorNumber=floorNumber;
            this->direction=direction;
            currWeight=0;
        }
        string getElevatorId(){
            return elevatorId;
        }
        Direction getElevatorDirection(){
            return direction;
        }
        int getCapacity(){
            return capacity;
        }
        void addUpStop(int floorNumber){
            lock_guard<mutex> lock(mtx);
            upStop.insert(floorNumber);
        }
        void addDownStop(int floorNumber){
            lock_guard<mutex> lock(mtx);
            downStop.insert(floorNumber);
        }
        void removeStop(int floorNumber){
            lock_guard<mutex> lock(mtx);
            upStop.erase(floorNumber);
            downStop.erase(floorNumber);
        }
        int getCurrWeight(){
            return currWeight;
        }
        int getFloorNumber(){
            return floorNumber;
        }
        void incrementWeight(int weight){
            currWeight+=weight;
        }
        void setWeight(int weight){
            currWeight=weight;
        }
        void setFloorNumber(int number){
            floorNumber=number;
        }
        bool canTakePassenger(int weight){
            return weight+currWeight<=capacity;
        }
        void processNextStop(){
            if(direction == Direction::UP){

                if(!upStop.empty()){

                    floorNumber = *upStop.begin();

                    upStop.erase(upStop.begin());
                }
                else if(!downStop.empty()){

                    direction = Direction::DOWN;
                    processNextStop();
                }
                else{

                    direction = Direction::IDLE;
                }
            }else{

                if(!downStop.empty()){

                    floorNumber = *downStop.begin();

                    downStop.erase(downStop.begin());
                }
                else if(!upStop.empty()){
                    direction = Direction::UP;
                    processNextStop();
                }
                else{

                    direction = Direction::IDLE;
                } 
            }
        }

};  

class ExternalRequest{
    private: 
        int floor;
        Direction direction;
    
    public:
        ExternalRequest(int floor,Direction direction){
            this->floor=floor;
            this->direction=direction;
        }
        int getCurrentFloor(){
            return floor;
        }
        Direction getDirection(){
            return direction;
        }
};

class InternalRequest{
    private: 
        int floor;
    
    public:
        InternalRequest(int floor){
            this->floor=floor;
        }
        int getDestinationFloor(){
            return floor;
        }
};


class AssignElevatorStrategy{
    public:
        virtual Elevator* assignElevator(vector<Elevator*> elevators,ExternalRequest *request)=0;
};  

class NearestMatchStrategy : public AssignElevatorStrategy{
    public:
        Elevator* assignElevator(vector<Elevator*> elevators,ExternalRequest *request) override{
            Elevator *res=nullptr;
            int dist=INT_MAX;
            for(auto e : elevators){

                bool eligible = false;

                if(e->getElevatorDirection() == Direction::IDLE){
                    eligible = true;
                }
                else if(
                    e->getElevatorDirection() == Direction::UP &&
                    request->getDirection() == Direction::UP &&
                    e->getFloorNumber() <= request->getCurrentFloor()
                ){
                    eligible = true;
                }
                else if(
                    e->getElevatorDirection() == Direction::DOWN &&
                    request->getDirection() == Direction::DOWN &&
                    e->getFloorNumber() >= request->getCurrentFloor()
                ){
                    eligible = true;
                }

                if(!eligible){
                    continue;
                }

                int currentDistance =
                    abs(
                        e->getFloorNumber() -
                        request->getCurrentFloor()
                    );

                if(currentDistance < dist){
                    dist = currentDistance;
                    res = e;
                }
            }
            if(res==nullptr) return res;

            if(res->getFloorNumber()-request->getCurrentFloor()>0) res->addDownStop(request->getCurrentFloor());
            else res->addUpStop(request->getCurrentFloor());

            return res;
        }
};

class ElevatorController{
    private:
        vector<Elevator*> elevators;
        AssignElevatorStrategy *strategy;
        mutable mutex mtx;

    public:

        ElevatorController(AssignElevatorStrategy* strategy){
            this->strategy = strategy;
        }

        void addElevator(Elevator *e){
            elevators.push_back(e);
        }

        Elevator* assignElevator(ExternalRequest *request){
            lock_guard<mutex> lock(mtx);
            return strategy->assignElevator(elevators,request);
        }

        void addDestination(Elevator *elevator,InternalRequest *request){
            lock_guard<mutex> lock(mtx);
            if(elevator->getFloorNumber()-request->getDestinationFloor()>0) elevator->addDownStop(request->getDestinationFloor());
            else elevator->addUpStop(request->getDestinationFloor());
        }
};


int main() {

    AssignElevatorStrategy* strategy =
        new NearestMatchStrategy();

    ElevatorController controller(strategy);

    Elevator* e1 =
        new Elevator(
            "E1",
            1000,
            2,
            Direction::IDLE
        );

    Elevator* e2 =
        new Elevator(
            "E2",
            1000,
            8,
            Direction::UP
        );

    Elevator* e3 =
        new Elevator(
            "E3",
            1000,
            15,
            Direction::DOWN
        );

    controller.addElevator(e1);
    controller.addElevator(e2);
    controller.addElevator(e3);

    ExternalRequest request1(
        5,
        Direction::UP
    );

    Elevator* assigned =
        controller.assignElevator(&request1);

    if(assigned != nullptr){
        cout << "Assigned Elevator: "
             << assigned->getElevatorId()
             << endl;
    }
    else{
        cout << "No Elevator Found"
             << endl;
    }

    InternalRequest request2(12);

    controller.addDestination(
        assigned,
        &request2
    );

    cout << "\nProcessing Stops...\n";

    assigned->processNextStop();

    cout << "Current Floor: "
         << assigned->getFloorNumber()
         << endl;

    assigned->processNextStop();

    cout << "Current Floor: "
         << assigned->getFloorNumber()
         << endl;

    return 0;
}