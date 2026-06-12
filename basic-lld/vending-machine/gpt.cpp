#include <iostream>
#include <map>

using namespace std;

enum class ProductType {
    Chips,
    SoftDrink,
    Chocolate,
};

class Product {
private:
    int price;
    ProductType productType;

public:
    Product(int price, ProductType productType)
        : price(price), productType(productType) {}

    ProductType getProductType() const {
        return productType;
    }

    int getProductPrice() const {
        return price;
    }
};

class Inventory {
private:
    map<ProductType, int> productQuantity;

public:
    void addProductToInventory(ProductType productType, int quantity) {
        productQuantity[productType] += quantity;
    }

    int getProductQuantity(ProductType productType) const {
        auto it = productQuantity.find(productType);

        if (it == productQuantity.end()) {
            return 0;
        }

        return it->second;
    }

    bool isAvailable(ProductType productType) const {
        return getProductQuantity(productType) > 0;
    }

    void removeProductFromInventory(ProductType productType, int quantity) {
        if (getProductQuantity(productType) >= quantity) {
            productQuantity[productType] -= quantity;
        }
    }
};

class Transaction {
private:
    ProductType productType;
    int quantity;
    int amountPaid;

public:
    Transaction(
        ProductType productType,
        int quantity,
        int amountPaid)
        : productType(productType),
          quantity(quantity),
          amountPaid(amountPaid) {}

    ProductType getProductType() const {
        return productType;
    }

    int getQuantity() const {
        return quantity;
    }

    int getPaidAmount() const {
        return amountPaid;
    }
};

class VendingMachine {
private:
    map<ProductType, Product> productCatalog;
    Inventory inventory;

public:
    void addNewProduct(const Product& product) {
        productCatalog.insert_or_assign(
            product.getProductType(),
            product
        );
    }

    void addProductQuantity(
        ProductType productType,
        int quantity) {
        inventory.addProductToInventory(
            productType,
            quantity
        );
    }

    void removeProductQuantity(
        ProductType productType,
        int quantity) {
        inventory.removeProductFromInventory(
            productType,
            quantity
        );
    }

    Product getProductByType(
        ProductType productType) const {
        return productCatalog.at(productType);
    }

    int getProductQuantity(
        ProductType productType) const {
        return inventory.getProductQuantity(productType);
    }

    // -1 => Out of stock
    // -2 => Insufficient funds
    // >=0 => Change to return
    int processTransaction(
        const Transaction& transaction) {

        if (inventory.getProductQuantity(
                transaction.getProductType())
            < transaction.getQuantity()) {
            return -1;
        }

        int productPrice =
            productCatalog
                .at(transaction.getProductType())
                .getProductPrice();

        int requiredAmount =
            productPrice * transaction.getQuantity();

        if (transaction.getPaidAmount()
            < requiredAmount) {
            return -2;
        }

        inventory.removeProductFromInventory(
            transaction.getProductType(),
            transaction.getQuantity()
        );

        return transaction.getPaidAmount()
               - requiredAmount;
    }
};