#include <iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;

enum class ProductType{
   Chips, 
   SoftDrink,
   Chocolate,
};

class Product{
    private:
        int price;
        ProductType productType;
    
    public:
        Product(int price,ProductType productType){
            cout<<"Product constructor called"<<'\n';
            this->price=price;
            this->productType=productType;
        }
        ProductType getProductType() const{
            return productType;
        }
        int getProductPrice() const{
            return price;
        }
};

class Inventory{
    private:
        map<ProductType,int> productQuantity;

    public:
        void addProductToInventory(ProductType productType, int quantity){
            productQuantity[productType]+=quantity;
        }
        int getProductQuantity(ProductType productType) const{
            auto it = productQuantity.find(productType);
            if(it == productQuantity.end()) {
                return 0;
            }
            return it->second;
        }
        bool isAvailable(ProductType productType) const{
           return getProductQuantity(productType) > 0;
        }
        void removeProductFromInventory(ProductType productType,int quantity){
            if(getProductQuantity(productType) >= quantity){
                productQuantity[productType]-=quantity;
            }
        }
};

class Transaction{
    private:
        ProductType productType;
        int quantity;
        int amountPaid;
    public:
        Transaction(ProductType productType,int quantity,int amountPaid){
            this->productType=productType;
            this->quantity=quantity;
            this->amountPaid=amountPaid;
        }
        int getProductQuantity() const{
            return quantity;
        }
        ProductType getProductType() const{
            return productType;
        }
        int getPaidAmount() const{
            return amountPaid;
        }
};

class VendingMachine{
    private:
        map<ProductType,Product> productCatalog;
        Inventory inventory;
        mutable mutex mtx;

    public:
        void addNewProduct(Product &product){
            productCatalog.emplace(product.getProductType(),product);
        }
        void addProductQuantity(ProductType productType,int quantity){
            inventory.addProductToInventory(productType,quantity);
        }        
        void removeProductQuantity(ProductType productType,int quantity){
            inventory.removeProductFromInventory(productType,quantity);
        }
        Product getProductByType(ProductType productType) const{
            return productCatalog.at(productType);
        }
        int getProductQuantity(ProductType productType) const{
            return inventory.getProductQuantity(productType);
        }
        int processTransaction(Transaction &transaction){
            lock_guard<mutex> lock(mtx);
            if(inventory.getProductQuantity(transaction.getProductType())< transaction.getProductQuantity()) return -1;
            int productPrice = productCatalog.at(transaction.getProductType()).getProductPrice();
            int requiredAmount = productPrice * transaction.getProductQuantity();

            if(transaction.getPaidAmount()< requiredAmount){
                return -2;
            }

            inventory.removeProductFromInventory(
                transaction.getProductType(),
                transaction.getProductQuantity()
            );

            return transaction.getPaidAmount()-requiredAmount;
        }
};


int main() {

    VendingMachine machine;

    // Register products
    Product chips(20, ProductType::Chips);
    Product softDrink(40, ProductType::SoftDrink);
    Product chocolate(10, ProductType::Chocolate);

    machine.addNewProduct(chips);
    machine.addNewProduct(softDrink);
    machine.addNewProduct(chocolate);

    // Add inventory
    machine.addProductQuantity(ProductType::Chips, 10);
    machine.addProductQuantity(ProductType::SoftDrink, 5);
    machine.addProductQuantity(ProductType::Chocolate, 2);

    cout << "Initial Chips Quantity: "
         << machine.getProductQuantity(ProductType::Chips)
         << endl;

    // Successful transaction
    Transaction txn1(
        ProductType::Chips,
        2,      // quantity
        50      // amount paid
    );

    int change = machine.processTransaction(txn1);

    if(change >= 0){
        cout << "Transaction Successful" << endl;
        cout << "Change Returned: " << change << endl;
    }
    else if(change == -1){
        cout << "Out of Stock" << endl;
    }
    else{
        cout << "Insufficient Funds" << endl;
    }

    cout << "Remaining Chips Quantity: "
         << machine.getProductQuantity(ProductType::Chips)
         << endl;

    cout << "---------------------" << endl;

    // Insufficient funds
    Transaction txn2(
        ProductType::SoftDrink,
        1,
        20
    );

    int result = machine.processTransaction(txn2);

    if(result == -2){
        cout << "Insufficient Funds Test Passed" << endl;
    }

    cout << "---------------------" << endl;

    // Out of stock
    Transaction txn3(
        ProductType::Chocolate,
        5,
        100
    );

    result = machine.processTransaction(txn3);

    if(result == -1){
        cout << "Out Of Stock Test Passed" << endl;
    }

    return 0;
}