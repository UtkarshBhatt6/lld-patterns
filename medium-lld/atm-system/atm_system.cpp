#include<iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;
class ATM;
class AtmState;
class IdleState;
class HasCardState;
class AuthenticatedState;
enum class Denomination{
    TEN = 10,
    TWENTY = 20,
    FIFTY = 50
};

class Card{
    private:
        string cardNumber;
        string accountNumber;
        int mpin;
    
    public:
        Card(string cardNumber,string accountNumber,int mpin){
            this->cardNumber=cardNumber;
            this->accountNumber=accountNumber;
            this->mpin=mpin;
        }
        string getAccountNumber() const{
            return accountNumber;
        }
        string getCardNumber() const{
            return cardNumber;
        }
        int getMpin() const{
            return mpin;
        }
};

class CashDispenser{
private:
    map<Denomination,int> notes;

public:

    void addCash(Denomination denomination,int count){
        notes[denomination] += count;
    }

    bool dispenseCash(
        int amount,
        map<Denomination,int>& dispensedNotes
    ){
        map<Denomination,int> temp = notes;

        vector<pair<Denomination,int>> denominations = {
            {Denomination::FIFTY, 50},
            {Denomination::TWENTY, 20},
            {Denomination::TEN, 10}
        };

        for(auto& denomination : denominations){

            Denomination note = denomination.first;
            int value = denomination.second;

            int requiredNotes = amount / value;

            int availableNotes = temp[note];

            int notesToGive =
                min(requiredNotes, availableNotes);

            if(notesToGive > 0){

                dispensedNotes[note] = notesToGive;

                temp[note] -= notesToGive;

                amount -= notesToGive * value;
            }
        }

        if(amount != 0){
            return false;
        }

        notes = temp;

        return true;
    }
};
class Account{
    private:
        string accountNumber;
        double balance;

    public:
        Account(string accountNumber,double balance){
            this->accountNumber=accountNumber;
            this->balance=balance;
        }

        void depositAmount(double amount){
            balance+=amount;
        }

        bool withdrawAmount(double amount){
            if(balance>=amount){
                balance-=amount;
                return true;
            }
            return false;
        }

        string getAccountNumber() const{
            return accountNumber;
        }

        double getAccountBalance() const{
            return balance;
        }
};

class BankingService{
    private:

        map<string,Account*>accountMap;
        map<string,Card*>cardMap;

    public:
        void addAccount(Account* account){
            accountMap[account->getAccountNumber()] = account;
        }

        void addCard(Card* card){
            cardMap[card->getAccountNumber()] = card;
        }

        bool validatePIN(string accountNumber,int mpin){
            if(cardMap.at(accountNumber)->getMpin()==mpin) return true;
            else return false;
        }

        void depositAmount(string accountNumber,double amount){
           accountMap.at(accountNumber)->depositAmount(amount);
        }

        bool withdrawAmount(string accountNumber,double amount){
            return accountMap.at(accountNumber)->withdrawAmount(amount);
        }

        double getAccountBalance(string accountNumber) const{
            return accountMap.at(accountNumber)->getAccountBalance();
        }

};

class ATM{
private:
    Card* card;
    AtmState* state;
    BankingService* bankingService;
    CashDispenser* cashDispenser;

public:
    ATM();

    void setState(AtmState* newState);

    void setBankingService(BankingService* bankingService);

    void setCashDispenser(CashDispenser* cashDispenser);

    void deposit(double amount);

    bool withdraw(
        double amount,
        map<Denomination,int>& returnCash
    );

    bool enterPIN(int pin);

    double checkBalance();

    void insertCard(Card* card);

    void ejectCard();

    Card* getCard() const;

    BankingService* getBankingService() const;

    bool validatePIN(int mpin);

    void depositAmount(double amount);

    bool withdrawAmount(
        double amount,
        map<Denomination,int>& returnCash
    );

    double getBalance() const;
};
// IDLE state -> insertCard
// CARD INSERTED state -> enter pin , eject card
// Verified state -> deposit, widthraw, eject card
class AtmState{
    public:
        virtual void insertCard(ATM *atm)=0;
        virtual bool enterPIN(ATM *atm,int pin)=0;
        virtual void ejectCard(ATM *atm)=0;
        virtual void deposit(ATM *atm,double amount)=0;
        virtual bool withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash)=0;
        virtual double checkBalance(ATM *atm)=0;
};

class IdleState : public AtmState{
public:
    void insertCard(ATM *atm) override;
    bool enterPIN(ATM *atm,int pin) override;
    void ejectCard(ATM *atm) override;
    void deposit(ATM *atm,double amount) override;
    bool withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash) override;
    double checkBalance(ATM *atm) override;
};

class HasCardState : public AtmState{
public:
    void insertCard(ATM *atm) override;
    bool enterPIN(ATM *atm,int pin) override;
    void ejectCard(ATM *atm) override;
    void deposit(ATM *atm,double amount) override;
    bool withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash) override;
    double checkBalance(ATM *atm) override;
};

class AuthenticatedState : public AtmState{
public:
    void insertCard(ATM *atm) override;
    bool enterPIN(ATM *atm,int pin) override;
    void ejectCard(ATM *atm) override;
    void deposit(ATM *atm,double amount) override;
    bool withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash) override;
    double checkBalance(ATM *atm) override;
};


ATM::ATM(){
    bankingService = nullptr;
    cashDispenser = nullptr;
    card = nullptr;
    state = new IdleState();
}

void ATM::setState(AtmState* newState){
    state = newState;
}

void ATM::setBankingService(
    BankingService* bankingService
){
    this->bankingService = bankingService;
}

void ATM::setCashDispenser(
    CashDispenser* cashDispenser
){
    this->cashDispenser = cashDispenser;
}

void ATM::deposit(double amount){
    state->deposit(this, amount);
}

bool ATM::withdraw(
    double amount,
    map<Denomination,int>& returnCash
){
    return state->withdraw(
        this,
        amount,
        returnCash
    );
}

bool ATM::enterPIN(int pin){
    return state->enterPIN(this, pin);
}

double ATM::checkBalance(){
    return state->checkBalance(this);
}

void ATM::insertCard(Card* card){
    this->card = card;
    state->insertCard(this);
}

void ATM::ejectCard(){
    state->ejectCard(this);
    card = nullptr;
}

Card* ATM::getCard() const{
    return card;
}

BankingService* ATM::getBankingService() const{
    return bankingService;
}

bool ATM::validatePIN(int mpin){
    return bankingService->validatePIN(
        card->getAccountNumber(),
        mpin
    );
}

void ATM::depositAmount(double amount){
    if(bankingService != nullptr){
        bankingService->depositAmount(
            card->getAccountNumber(),
            amount
        );
    }
}

bool ATM::withdrawAmount(
    double amount,
    map<Denomination,int>& returnCash
){
    if(bankingService == nullptr){
        return false;
    }

    bool res =
        bankingService->withdrawAmount(
            card->getAccountNumber(),
            amount
        );

    if(res){

        bool dispenserRes =
            cashDispenser->dispenseCash(
                amount,
                returnCash
            );

        if(!dispenserRes){
            depositAmount(amount);
            return false;
        }

        return true;
    }

    return false;
}

double ATM::getBalance() const{
    if(bankingService == nullptr){
        return 0;
    }

    return bankingService->getAccountBalance(
        card->getAccountNumber()
    );
}
// ================= IdleState =================

void IdleState::insertCard(ATM *atm){
    cout << "Card Inserted" << '\n';
    atm->setState(new HasCardState());
}

bool IdleState::enterPIN(ATM *atm,int pin){
    cout << "Invalid state for given action" << '\n';
    return false;
}

void IdleState::ejectCard(ATM *atm){
    cout << "Invalid state for given action" << '\n';
}

void IdleState::deposit(ATM *atm,double amount){
    cout << "Invalid state for given action" << '\n';
}

bool IdleState::withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash){
    cout << "Invalid state for given action" << '\n';
    return false;
}

double IdleState::checkBalance(ATM *atm){
    cout << "Invalid state for given action" << '\n';
    return 0;
}


// ================= HasCardState =================

void HasCardState::insertCard(ATM *atm){
    cout << "Invalid state for given action" << '\n';
}

bool HasCardState::enterPIN(ATM *atm,int pin){
    bool res = atm->validatePIN(pin);

    if(res){
        atm->setState(new AuthenticatedState());
    }

    return res;
}

void HasCardState::ejectCard(ATM *atm){
    atm->setState(new IdleState());
}

void HasCardState::deposit(ATM *atm,double amount){
    cout << "Invalid state for given action" << '\n';
}

bool HasCardState::withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash){
    cout << "Invalid state for given action" << '\n';
    return false;
}

double HasCardState::checkBalance(ATM *atm){
    cout << "Invalid state for given action" << '\n';
    return 0;
}


// ================= AuthenticatedState =================

void AuthenticatedState::insertCard(ATM *atm){
    cout << "Invalid state for given action" << '\n';
}

bool AuthenticatedState::enterPIN(ATM *atm,int pin){
    cout << "Invalid state for given action" << '\n';
    return false;
}

void AuthenticatedState::ejectCard(ATM *atm){
    atm->setState(new IdleState());
}

void AuthenticatedState::deposit(ATM *atm,double amount){
    atm->depositAmount(amount);
}

bool AuthenticatedState::withdraw(ATM *atm,double amount,map<Denomination,int> &returnCash){
    return atm->withdrawAmount(amount,returnCash);
}

double AuthenticatedState::checkBalance(ATM *atm){
    return atm->getBalance();
}

int main(){

    // Create Account
    Account* account =
        new Account("ACC123", 1000);

    // Create Card
    Card* card =
        new Card("CARD123", "ACC123", 1234);

    // Create Bank Backend
    BankingService* bank =
        new BankingService();

    bank->addAccount(account);
    bank->addCard(card);

    // Create Cash Dispenser
    CashDispenser* dispenser =
        new CashDispenser();

    dispenser->addCash(
        Denomination::FIFTY,
        10
    );

    dispenser->addCash(
        Denomination::TWENTY,
        10
    );

    dispenser->addCash(
        Denomination::TEN,
        10
    );

    // Create ATM
    ATM atm;

    atm.setBankingService(bank);
    atm.setCashDispenser(dispenser);

    // Insert Card
    atm.insertCard(card);

    // Authenticate
    bool pinValid =
        atm.enterPIN(1234);

    cout << "PIN Valid: "
         << pinValid
         << endl;

    // Check Balance
    cout << "Balance: "
         << atm.checkBalance()
         << endl;

    // Withdraw
    map<Denomination,int> notes;

    bool success =
        atm.withdraw(
            180,
            notes
        );

    cout << "Withdraw Success: "
         << success
         << endl;

    cout << "\nDispensed Notes:\n";

    for(auto& entry : notes){

        cout
            << static_cast<int>(entry.first)
            << " -> "
            << entry.second
            << endl;
    }

    // Check Balance Again
    cout << "\nBalance After Withdrawal: "
         << atm.checkBalance()
         << endl;

    // Deposit
    atm.deposit(200);

    cout << "\nBalance After Deposit: "
         << atm.checkBalance()
         << endl;

    // Eject Card
    atm.ejectCard();

    return 0;
}