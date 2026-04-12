#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream> 
#include <iomanip>
#include <ctime>
#include <windows.h> 

using namespace std;

// Số lượng tối đa cho phép
const int MAX_PRODUCTS = 100;
const int MAX_CART = 50;

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
    // Dùng mảng tĩnh thay cho vector
    Product products[MAX_PRODUCTS];
    int productCount = 0; // Biến quản lý số lượng phần tử thực tế trong mảng

    CartItem cart[MAX_CART];
    int cartCount = 0;

public:
    void loadFromFile(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Loi: Khong tim thay file " << filename << endl;
            return;
        }

        productCount = 0; // Reset số lượng
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
                products[productCount].price = stoll(priceStr);
                productCount++;
            }
        }
        file.close();
        cout << "=> Da load " << productCount << " san pham vao mang tinh.\n";
    }

    void showProducts() {
        cout << "\n--- DANH SACH SAN PHAM ---\n";
        cout << left << setw(8) << "Ma" << setw(20) << "Ten San Pham" << "Gia" << endl;
        cout << string(40, '-') << endl;
        for (int i = 0; i < productCount; i++) {
            cout << left << setw(8) << products[i].id 
                 << setw(20) << products[i].name 
                 << products[i].price << " VND" << endl;
        }
    }

    void addToCart() {
        if (cartCount >= MAX_CART) {
            cout << "Gio hang da day!\n";
            return;
        }

        string id;
        int qty;
        cout << "Nhap Ma SP muon mua: "; cin >> id;
        
        bool found = false;
        for (int i = 0; i < productCount; i++) {
            if (products[i].id == id) {
                cout << "Nhap so luong: "; cin >> qty;
                cart[cartCount].product = products[i];
                cart[cartCount].quantity = qty;
                cartCount++;
                cout << "Da them vao gio hang!\n";
                found = true;
                break;
            }
        }
        if (!found) cout << "Khong tim thay Ma SP nay!\n";
    }

    void checkout() {
        if (cartCount == 0) {
            cout << "Gio hang dang trong!\n";
            return;
        }

        string name, phone, addr;
        cin.ignore();
        cout << "\n--- NHAP THONG TIN GIAO HANG ---\n";
        cout << "Ho ten: "; getline(cin, name);
        cout << "So dien thoai: "; getline(cin, phone);
        cout << "Dia chi: "; getline(cin, addr);

        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateStr[100];
        strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M", ltm);

        time_t deliveryTime = now + (3 * 24 * 3600);
        tm* dtm = localtime(&deliveryTime);
        char delivStr[100];
        strftime(delivStr, sizeof(delivStr), "%d/%m/%Y", dtm);

        stringstream ss;
        ss << "   --- XAC NHAN DON HANG ---\n\n";
        ss << "Khach hang: " << name << "\n";
        ss << "SDT: " << phone << "\n";
        ss << "Dia chi: " << addr << "\n\n";
        ss << "DANH SACH SAN PHAM:\n";

        long long total = 0;
        for (int i = 0; i < cartCount; i++) {
            long long sub = cart[i].product.price * cart[i].quantity;
            ss << "+ " << cart[i].product.name << " (x" << cart[i].quantity << "): " << sub << " VND\n";
            total += sub;
        }

        long long ship = 30000;
        ss << "\n----------------------------------------\n";
        ss << "Thoi gian dat: " << dateStr << "\n";
        ss << "Du kien giao: " << delivStr << " (Sau 3 ngay)\n";
        ss << "TONG CONG: " << total + ship << " VND\n\n";
        ss << "Nhan OK de hoan tat!";

        MessageBoxA(NULL, ss.str().c_str(), "THONG BAO", MB_OK | MB_ICONINFORMATION);

        cout << "\nDAT HANG THANH CONG!\n";
        cartCount = 0; // Reset giỏ hàng
    }
};

int main() {
    OrderSystem system;
    system.loadFromFile("products.txt");

    int choice;
    do {
        cout << "\n1. Xem san pham\n2. Mua hang\n3. Thanh toan\n4. Thoat\nChon (1-4): ";
        cin >> choice;
        if (choice == 1) system.showProducts();
        else if (choice == 2) system.addToCart();
        else if (choice == 3) system.checkout();
    } while (choice != 4);

    return 0;
}