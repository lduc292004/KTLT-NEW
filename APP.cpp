#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream> 
#include <iomanip>
#include <ctime>
#include <windows.h> 

using namespace std;

const int MAX_PRODUCTS = 100;
const int MAX_CART = 50;

// Ham dinh dang so tien: 21990000 -> 21.990.000
string formatCurrency(long long money) {
    if (money == 0) return "0";
    string s = to_string(money);
    int n = (int)s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ".");
    }
    return s;
}

struct Product {
    string id;
    string name;
    long long price; 
};

struct CartItem {
    Product product;
    int quantity;
};

class OrderSystem {
private:
    Product products[MAX_PRODUCTS];
    int productCount = 0;
    CartItem cart[MAX_CART];
    int cartCount = 0;

public:
    void loadFromFile(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Loi: Khong tim thay file " << filename << endl;
            return;
        }

        productCount = 0; 
        string line;
        while (getline(file, line) && productCount < MAX_PRODUCTS) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, name, priceStr;
            
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, priceStr, ',');

            if (!id.empty() && !name.empty()) {
                products[productCount].id = id;
                products[productCount].name = name;
                try {
                    products[productCount].price = (long long)stod(priceStr);
                } catch (...) { products[productCount].price = 0; }
                productCount++;
            }
        }
        file.close();
        cout << "=> Da nap " << productCount << " san pham.\n";
    }

    void showProducts() {
        cout << "\n" << left << setw(8) << "Ma" << setw(35) << "Ten San Pham" << "Gia (VND)" << endl;
        cout << string(60, '-') << endl;
        for (int i = 0; i < productCount; i++) {
            cout << left << setw(8) << products[i].id 
                 << setw(35) << products[i].name 
                 << formatCurrency(products[i].price) << " VND" << endl;
        }
    }

    void addToCart() {
        if (cartCount >= MAX_CART) {
            cout << "Gio hang da day!\n";
            return;
        }
        string id; int qty;
        cout << "Nhap Ma SP muon mua: "; cin >> id;
        for (int i = 0; i < productCount; i++) {
            if (products[i].id == id) {
                cout << "Nhap so luong: "; cin >> qty;
                if (qty <= 0) return;
                cart[cartCount].product = products[i];
                cart[cartCount].quantity = qty;
                cartCount++;
                cout << "Da them vao gio hang!\n";
                return;
            }
        }
        cout << "Khong tim thay ma san pham!\n";
    }

    void checkout() {
        if (cartCount == 0) {
            cout << "Gio hang dang trong!\n";
            return;
        }
        string name, phone, addr;
        cin.ignore();
        cout << "\n--- THONG TIN GIAO HANG ---\n";
        cout << "Ho ten: "; getline(cin, name);
        cout << "So dien thoai: "; getline(cin, phone);
        cout << "Dia chi: "; getline(cin, addr);

        // --- XU LY THOI GIAN ---
        time_t now = time(0);
        
        // Thoi gian dat hang hien tai
        char dateStr[100];
        strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M", localtime(&now));

        // Thoi gian du kien giao hang (hien tai + 3 ngay)
        time_t deliveryTime = now + (3 * 24 * 3600);
        char delivStr[100];
        strftime(delivStr, sizeof(delivStr), "%d/%m/%Y", localtime(&deliveryTime));

        // --- TAO NOI DUNG HOA DON ---
        stringstream ss;
        ss << "     --- HOA DON BAN HANG ---\n\n";
        ss << "Khach hang: " << name << "\n";
        ss << "SDT: " << phone << "\n";
        ss << "Dia chi: " << addr << "\n\n";
        ss << "CHI TIET SAN PHAM:\n";

        long long total = 0;
        for (int i = 0; i < cartCount; i++) {
            long long sub = cart[i].product.price * cart[i].quantity;
            ss << "+ " << cart[i].product.name << " (x" << cart[i].quantity << "): " 
               << formatCurrency(sub) << " VND\n";
            total += sub;
        }

        long long ship = 30000;
        ss << "\n----------------------------------------\n";
        ss << "Tam tinh:    " << formatCurrency(total) << " VND\n";
        ss << "Phi ship:    " << formatCurrency(ship) << " VND\n";
        ss << "TONG CONG:   " << formatCurrency(total + ship) << " VND\n";
        ss << "----------------------------------------\n";
        ss << "Thoi gian dat: " << dateStr << "\n";
        ss << "Du kien giao:  " << delivStr << " (Sau 3 ngay)\n\n";
        ss << "Nhan OK de xac nhan don hang!";

        // Hien thi Popup xac nhan
        MessageBoxA(NULL, ss.str().c_str(), "XAC NHAN DON HANG", MB_OK | MB_ICONINFORMATION);
        
        cout << "\nTHANH TOAN HOAN TAT! Don hang se duoc giao vao ngay " << delivStr << endl;
        cartCount = 0; 
    }
};

int main() {
    // Luu y: Da bo SetConsoleOutputCP vi khong dung dau tieng Viet nua
    
    OrderSystem system;
    system.loadFromFile("products.txt");

    int choice;
    do {
        cout << "\n1. Danh sach | 2. Mua hang | 3. Thanh toan | 4. Thoat: ";
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n'); continue;
        }
        if (choice == 1) system.showProducts();
        else if (choice == 2) system.addToCart();
        else if (choice == 3) system.checkout();
    } while (choice != 4);

    return 0;
}