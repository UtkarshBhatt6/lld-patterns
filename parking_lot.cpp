#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>

using namespace std;

enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

enum class SpotType {
    SMALL,
    MEDIUM,
    LARGE
};

class Vehicle {
protected:
    string licenseNumber;
    VehicleType type;

public:
    Vehicle(string licenseNumber, VehicleType type) {
        this->licenseNumber = licenseNumber;
        this->type = type;
    }

    virtual ~Vehicle() {}

    string getLicenseNumber() {
        return licenseNumber;
    }

    VehicleType getVehicleType() {
        return type;
    }
};

class Bike : public Vehicle {
public:
    Bike(string licenseNumber)
        : Vehicle(licenseNumber, VehicleType::BIKE) {}
};

class Car : public Vehicle {
public:
    Car(string licenseNumber)
        : Vehicle(licenseNumber, VehicleType::CAR) {}
};

class Truck : public Vehicle {
public:
    Truck(string licenseNumber)
        : Vehicle(licenseNumber, VehicleType::TRUCK) {}
};

class ParkingSpot {
private:
    int spotNumber;
    bool occupied;
    SpotType type;
    Vehicle* vehicle;

public:
    ParkingSpot(int spotNumber, SpotType type) {
        this->spotNumber = spotNumber;
        this->type = type;
        this->occupied = false;
        this->vehicle = nullptr;
    }

    virtual ~ParkingSpot() {}

    virtual bool canFitVehicle(Vehicle* vehicle) = 0;

    bool parkVehicle(Vehicle* v) {

        if (occupied || !canFitVehicle(v)) {
            return false;
        }

        occupied = true;
        vehicle = v;

        return true;
    }

    void removeVehicle() {
        occupied = false;
        vehicle = nullptr;
    }

    bool isAvailable() {
        return !occupied;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    SpotType getSpotType() {
        return type;
    }
};

class BikeSpot : public ParkingSpot {
public:
    BikeSpot(int spotNumber)
        : ParkingSpot(spotNumber, SpotType::SMALL) {}

    bool canFitVehicle(Vehicle* vehicle) override {

        return vehicle->getVehicleType()
               == VehicleType::BIKE;
    }
};

class CarSpot : public ParkingSpot {
public:
    CarSpot(int spotNumber)
        : ParkingSpot(spotNumber, SpotType::MEDIUM) {}

    bool canFitVehicle(Vehicle* vehicle) override {

        return vehicle->getVehicleType()
                   == VehicleType::BIKE ||

               vehicle->getVehicleType()
                   == VehicleType::CAR;
    }
};

class TruckSpot : public ParkingSpot {
public:
    TruckSpot(int spotNumber)
        : ParkingSpot(spotNumber, SpotType::LARGE) {}

    bool canFitVehicle(Vehicle* vehicle) override {            
        return true;
    }
};

class ParkingStrategy {
public:
    virtual ParkingSpot* getParkingSpot(
        vector<ParkingSpot*>& parkingSpots,
        Vehicle* vehicle
    ) = 0;

    virtual ~ParkingStrategy() {}
};

class FirstFitStrategy : public ParkingStrategy {
public:
    ParkingSpot* getParkingSpot(
        vector<ParkingSpot*>& parkingSpots,
        Vehicle* vehicle
    ) override {

        for (auto parkingSpot : parkingSpots) {

            if (parkingSpot->isAvailable() &&
                parkingSpot->canFitVehicle(vehicle)) {

                return parkingSpot;
            }
        }

        return nullptr;
    }
};

class ParkingFloor {
private:
    int floorNumber;

    vector<ParkingSpot*> parkingSpots;

    ParkingStrategy* parkingStrategy;

public:
    ParkingFloor(
        int floorNumber,
        ParkingStrategy* parkingStrategy
    ) {
        this->floorNumber = floorNumber;
        this->parkingStrategy = parkingStrategy;
    }

    void addParkingSpot(
        ParkingSpot* parkingSpot
    ) {
        parkingSpots.push_back(parkingSpot);
    }

    ParkingSpot* occupySpot(
        Vehicle* vehicle
    ) {

        ParkingSpot* parkingSpot =
            parkingStrategy->getParkingSpot(
                parkingSpots,
                vehicle
            );

        if (parkingSpot != nullptr) {

            parkingSpot->parkVehicle(vehicle);

            return parkingSpot;
        }

        return nullptr;
    }

    bool removeVehicle(
        string licenseNumber
    ) {

        for (auto parkingSpot :
             parkingSpots) {

            Vehicle* vehicle =
                parkingSpot->getVehicle();

            if (vehicle != nullptr &&
                vehicle->getLicenseNumber()
                    == licenseNumber) {

                parkingSpot->removeVehicle();

                return true;
            }
        }

        return false;
    }
};

class Ticket {
private:
    int ticketID;

    Vehicle* vehicle;

    ParkingSpot* parkingSpot;

    time_t entryTime;

    time_t exitTime;

public:
    Ticket(
        int ticketID,
        Vehicle* vehicle,
        ParkingSpot* parkingSpot
    ) {
        this->ticketID = ticketID;
        this->vehicle = vehicle;
        this->parkingSpot = parkingSpot;

        entryTime = time(nullptr);

        exitTime = 0;
    }

    int getTicketID() {
        return ticketID;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    ParkingSpot* getParkingSpot() {
        return parkingSpot;
    }

    time_t getEntryTime() {
        return entryTime;
    }

    time_t getExitTime() {
        return exitTime;
    }

    void closeTicket() {
        exitTime = time(nullptr);
    }
};

class PricingStrategy {
public:
    virtual double calculateFee(
        Ticket* ticket
    ) = 0;

    virtual ~PricingStrategy() {}
};

class HourlyPricingStrategy
    : public PricingStrategy {

public:
    double calculateFee(
        Ticket* ticket
    ) override {

        time_t currentTime =
            time(nullptr);

        double hours =
            difftime(
                currentTime,
                ticket->getEntryTime()
            ) / 3600.0;

        if (hours < 1) {
            hours = 1;
        }

        VehicleType type =
            ticket->getVehicle()
                  ->getVehicleType();

        if (type == VehicleType::BIKE) {
            return hours * 10;
        }

        if (type == VehicleType::CAR) {
            return hours * 20;
        }

        return hours * 40;
    }
};

class ParkingLot {
private:
    vector<ParkingFloor*> parkingFloors;

    unordered_map<int, Ticket*> activeTickets;

    PricingStrategy* pricingStrategy;

    int nextTicketID;

public:
    ParkingLot(
        PricingStrategy* pricingStrategy
    ) {
        this->pricingStrategy =
            pricingStrategy;

        nextTicketID = 1;
    }

    void addFloor(
        ParkingFloor* parkingFloor
    ) {
        parkingFloors.push_back(
            parkingFloor
        );
    }

    Ticket* parkVehicle(
        Vehicle* vehicle
    ) {

        for (auto parkingFloor :
             parkingFloors) {

            ParkingSpot* parkingSpot =
                parkingFloor->occupySpot(
                    vehicle
                );

            if (parkingSpot != nullptr) {

                Ticket* ticket =
                    new Ticket(
                        nextTicketID++,
                        vehicle,
                        parkingSpot
                    );

                activeTickets[
                    ticket->getTicketID()
                ] = ticket;

                return ticket;
            }
        }

        return nullptr;
    }

    double exitVehicle(
        int ticketID
    ) {

        if (activeTickets.find(ticketID)
            == activeTickets.end()) {

            return -1;
        }

        Ticket* ticket =
            activeTickets[ticketID];

        double fee =
            pricingStrategy
                ->calculateFee(ticket);

        ticket->closeTicket();

        string licenseNumber =
            ticket->getVehicle()
                  ->getLicenseNumber();

        for (auto parkingFloor :
             parkingFloors) {

            if (parkingFloor->removeVehicle(
                    licenseNumber)) {

                break;
            }
        }

        activeTickets.erase(ticketID);

        return fee;
    }
};

int main() {

    ParkingStrategy* parkingStrategy =
        new FirstFitStrategy();

    PricingStrategy* pricingStrategy =
        new HourlyPricingStrategy();

    ParkingLot parkingLot(
        pricingStrategy
    );

    ParkingFloor* floor1 =
        new ParkingFloor(
            1,
            parkingStrategy
        );

    floor1->addParkingSpot(
        new BikeSpot(1));

    floor1->addParkingSpot(
        new CarSpot(2));

    floor1->addParkingSpot(
        new TruckSpot(3));

    parkingLot.addFloor(floor1);

    Vehicle* car =
        new Car("KA01AB1234");

    Ticket* ticket =
        parkingLot.parkVehicle(car);

    if (ticket != nullptr) {

        cout << "Vehicle Parked Successfully\n";

        cout << "Ticket ID: "
             << ticket->getTicketID()
             << endl;
    }
    else {

        cout << "Parking Full\n";
    }

    double fee =
        parkingLot.exitVehicle(
            ticket->getTicketID()
        );

    cout << "Parking Fee: "
         << fee
         << endl;

    return 0;
}
