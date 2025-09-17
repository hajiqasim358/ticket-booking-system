// Ticket Booking System
//

// main.cpp

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


using namespace std;

// Helper to convert various types to string (no std::to_string in C++98) 
template <typename T>
string toString(const T& value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

// Represents a single seat in a show
class Seat {
public:
    int row;
    int number;
    bool isBooked;
    Seat() : row(0), number(0), isBooked(false) {}
    Seat(int r, int n) : row(r), number(n), isBooked(false) {}
};

// Represents an individual show (concert, movie or bus trip)
class Show {
private:
    string title;
    string dateTime;
    Seat** seats;
    int rows, cols;
    double price;       // ticket price in PKR

public:
    Show(const string& t, const string& dt, int r, int c, double p)
        : title(t), dateTime(dt), rows(r), cols(c), price(p)
    {
        // allocate a 2D array of Seat
        seats = new Seat*[rows];
        for (int i = 0; i < rows; ++i) {
            seats[i] = new Seat[cols];
            for (int j = 0; j < cols; ++j) {
                seats[i][j] = Seat(i+1, j+1);
            }
        }
    }

    ~Show() {
        // free the 2D array
        for (int i = 0; i < rows; ++i) {
            delete[] seats[i];
        }
        delete[] seats;
    }

    string getTitle()    const { return title; }
    string getDateTime() const { return dateTime; }
    double getPrice()    const { return price;   }

    void setTitle(const string& t)      { title = t; }
    void setDateTime(const string& dt)  { dateTime = dt; }

    // Print all unbooked seats
    void displayAvailableSeats() const {
        cout << "Available seats for \"" << title 
             << "\" on " << dateTime
             << " [Ticket Price: PKR " << price << "]\n";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (!seats[i][j].isBooked) {
                    cout << "[" << seats[i][j].row
                         << "," << seats[i][j].number << "] ";
                }
            }
            cout << "\n";
        }
    }

    // Try to book a seat; return false if out of range or already booked
    bool bookSeat(int r, int c) {
        if (r < 1 || r > rows || c < 1 || c > cols) return false;
        if (seats[r-1][c-1].isBooked)             return false;
        seats[r-1][c-1].isBooked = true;
        return true;
    }
};




// Holds a dynamic list of Shows under one category

class Category {
private:
    string name;
    vector<Show*> shows;

public:
    Category(const string& n) : name(n) {}

    ~Category() {
        for (size_t i = 0; i < shows.size(); ++i) {
            delete shows[i];
        }
    }

    string getName() const { return name; }
    void   setName(const string& n) { name = n; }

    int    getCount() const { return (int)shows.size(); }

    // Return pointer to the i-th show, or NULL if out of range
    Show*  getShow(int idx) const {
        if (idx < 0 || idx >= getCount()) return NULL;
        return shows[idx];
    }

    // List all shows in this category
  /*  void listShows() const {
        for (int i = 0; i < getCount(); ++i) {
            Show* s = shows[i];
            cout << (i+1) << ". "
                 << s->getTitle()
                 << " at " << s->getDateTime() << "\n";
        }
    } */
    


void listShows() const {
    time_t now = time(NULL);
    tm currentTime = *localtime(&now);
    
    for (int i = 0; i < getCount(); ++i) {
        Show* s = shows[i];
        tm showTime = s->getDateTime();
        
        // Calculate time difference in days
        double diff = difftime(mktime(&showTime), mktime(&currentTime)) / (60 * 60 * 24);
        
        // Display only shows within the next three days
        if (diff >= 0 && diff <= 3) {
            std::cout << (i+1) << ". "
                      << s->getTitle()
                      << " at " << s->getDateTime() << "\n";
        }
    }
}

    // Add a new show (including ticket price)
    void addShow(const string& title,
                 const string& dt,
                 int rows, int cols,
                 double price)
    {
        shows.push_back(new Show(title, dt, rows, cols, price));
    }

    // Remove a show by index; returns false if index invalid
    bool removeShow(int idx) {
        if (idx < 0 || idx >= getCount()) return false;
        delete shows[idx];
        shows.erase(shows.begin() + idx);
        return true;
    }

    // Edit an existing show's title and/or datetime (price unchanged)
    bool editShow(int idx,
                  const string& newTitle,
                  const string& newDateTime)
    {
        Show* s = getShow(idx);
        if (!s) return false;
        s->setTitle(newTitle);
        s->setDateTime(newDateTime);
        return true;
    }
};






// Records a completed booking
struct Booking {
    string id;
    Show*  show;
    int    row, col;
    Booking(const string& i, Show* s, int r, int c)
        : id(i), show(s), row(r), col(c) {}
};




// Simulates a payment gateway
class PaymentProcessor {
public:
    static bool processPayment(double amount) {
        cout << "\n=== Payment Processing ===\n";
        cout << "Amount to pay: PKR " << amount << "\n";

        cout << "Enter card number: ";
        string card; cin >> card;

        cout << "Enter expiry date (MM/YY): ";
        string exp;  cin >> exp;

        cout << "Enter CVV: ";
        string cvv;  cin >> cvv;

        cout << "Processing";
        // simple delay animation
        for (int i = 0; i < 3; ++i) {
            cout << ".";
            cout.flush();
#ifdef _WIN32
            Sleep(500);
#else
            usleep(500000);
#endif
        }
        cout << "\nPayment successful!\n";
        return true;
    }
};






// Main system orchestrating categories, shows, bookings and admin/user dashboards
class TicketBookingSystem {
private:
    vector<Category*> categories;
    vector<Booking*>  bookings;
    const string      adminUser = "admin";
    const string      adminPass = "admin";

    // Generate a random 5-digit booking ID
    string generateBookingID() {
        static bool seeded = false;
        if (!seeded) {
            srand((unsigned)time(NULL));
            seeded = true;
        }
        int id = rand() % 90000 + 10000;  // 10000–99999
        return "BK" + toString(id);
    }

    // Record a successful booking
    void addBookingRecord(const string& id, Show* s, int r, int c) {
        bookings.push_back(new Booking(id, s, r, c));
    }

    // Admin dashboard
    void adminMenu() {
        while (true) {
            cout << "\n--- Admin Menu ---\n"
                 << "1. View booked tickets\n"
                 << "2. New Booking & Manage categories\n"
                 << "0. Logout\n"
                 << "Choice: ";
            int choice; cin >> choice;

            if (choice == 1) {
                if (bookings.empty()) {
                    cout << "No bookings have been made yet.\n";
                } else {
                    cout << "\nBooked Tickets:\n";
                    for (size_t i = 0; i < bookings.size(); ++i) {
                        Booking* b = bookings[i];
                        cout << (i+1) << ". ID: " << b->id
                             << ", Show: " << b->show->getTitle()
                             << ", When: " << b->show->getDateTime()
                             << ", Seat: [" << b->row << "," << b->col << "]\n";
                    }
                }
            }
            else if (choice == 2) {
                // Category management
                while (true) {
                    cout << "\n--- Book Tickets ---\n";
                    for (size_t i = 0; i < categories.size(); ++i) {
                        cout << (i+1) << ". " << categories[i]->getName() << "\n";
                    }
                    
                    // Add new Category
                    cout << "\n--- Manage Categories ---\n";
                    cout << (categories.size()+1)
                         << ". Add new category\n"
                         << (categories.size()+2)
                         << ". Delete category\n"
                         << (categories.size()+3)
                         << ". Rename category\n"
                         << (categories.size()+4)
                         << ". Back\n"
                         << "Choice: ";
                    int c; cin >> c;

					
                    int n = (int)categories.size();
                    if (c >= 1 && c <= n) {
                        // manage a specific category
                        Category* cat = categories[c-1];
                        while (true) {
                            cout << "\n--- Category: " << cat->getName() << " ---\n"
                                 << "1. List shows\n"
                                 << "2. Add show\n"
                                 << "3. Edit show\n"
                                 << "4. Delete show\n"
                                 << "0. Back\n"
                                 << "Choice: ";
                            int sub; cin >> sub;
                            if (sub == 1) {
                                cat->listShows();
                            }
                            else if (sub == 2) {
                                string t, dt;
                                int r, co;
                                double p;
                                cout << "Enter new show title: ";
                                cin.ignore(); getline(cin, t);
                                cout << "Enter date/time: ";
                                getline(cin, dt);
                                cout << "Enter rows and cols: ";
                                cin >> r >> co;
                                cout << "Enter ticket price (PKR): ";
                                cin >> p;
                                cat->addShow(t, dt, r, co, p);
                                cout << "Show added.\n";
                            }
                            else if (sub == 3) {
                                cat->listShows();
                                cout << "Enter show number to edit: ";
                                int si; cin >> si;
                                if (si < 1 || si > cat->getCount()) {
                                    cout << "Invalid number.\n";
                                } else {
                                    string nt, ndt;
                                    cout << "Enter new title: ";
                                    cin.ignore(); getline(cin, nt);
                                    cout << "Enter new date/time: ";
                                    getline(cin, ndt);
                                    cat->editShow(si-1, nt, ndt);
                                    cout << "Show updated.\n";
                                }
                            }
                            else if (sub == 4) {
                                cat->listShows();
                                cout << "Enter show number to delete: ";
                                int si; cin >> si;
                                if (cat->removeShow(si-1)) {
                                    cout << "Show deleted.\n";
                                } else {
                                    cout << "Invalid number.\n";
                                }
                            }
                            else if (sub == 0) {
                                break;
                            }
                            else {
                                cout << "Invalid choice.\n";
                            }
                        }
                    }
                    else if (c == n+1) {
                        // add category
                        cout << "Enter new category name: ";
                        string nm;
                        cin.ignore(); getline(cin, nm);
                        categories.push_back(new Category(nm));
                        cout << "Category added.\n";
                    }
                    else if (c == n+2) {
                        // delete category
                        cout << "Enter category number to delete: ";
                        int di; cin >> di;
                        if (di >= 1 && di <= n) {
                            delete categories[di-1];
                            categories.erase(categories.begin() + (di-1));
                            cout << "Category deleted.\n";
                        } else {
                            cout << "Invalid number.\n";
                        }
                    }
                    else if (c == n+3) {
                        // rename category
                        cout << "Enter category number to rename: ";
                        int ri; cin >> ri;
                        if (ri >= 1 && ri <= n) {
                            cout << "Enter new name: ";
                            string nn;
                            cin.ignore(); getline(cin, nn);
                            categories[ri-1]->setName(nn);
                            cout << "Category renamed.\n";
                        } else {
                            cout << "Invalid number.\n";
                        }
                    }
                    else if (c == n+4) {
                        break;
                    }
                    else {
                        cout << "Invalid choice.\n";
                    }
                }
            }
            else if (choice == 0) {
                cout << "Logging out of admin.\n";
                break;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }

    // User dashboard
    void userMenu() {
        while (true) {
            cout << "\n--- User Menu ---\n";
            for (size_t i = 0; i < categories.size(); ++i) {
                cout << (i+1) << ". " << categories[i]->getName() << "\n";
            }
            cout << (categories.size()+1) << ". Back to main menu\n";
            cout << "Select category: ";
            int c; cin >> c;
            if (c == (int)categories.size()+1) break;
            if (c < 1 || c > (int)categories.size()) {
                cout << "Invalid category.\n";
                continue;
            }
            Category* cat = categories[c-1];
            cout << "\n--- " << cat->getName() << " ---\n";
            cat->listShows();
            cout << "Select show number (or 0 to go back): ";
            int s; cin >> s;
            if (s == 0) continue;
            Show* sel = cat->getShow(s-1);
            if (!sel) {
                cout << "Invalid show.\n";
                continue;
            }
            sel->displayAvailableSeats();
            cout << "Enter row and seat number to book: ";
            int r, co; cin >> r >> co;
            if (!sel->bookSeat(r, co)) {
                cout << "Seat unavailable.\n";
                continue;
            }
            double price = sel->getPrice();
            if (!PaymentProcessor::processPayment(price)) {
                cout << "Payment failed.\n";
                continue;
            }
            string bid = generateBookingID();
            cout << "\nBooking confirmed! ID = " << bid << "\n";
            addBookingRecord(bid, sel, r, co);
        }
    }

    // Display help information
    void displayHelp() {
        cout << "\n=== Help ===\n";
        cout << "1. Select 'Admin Login' to manage shows and bookings (admin credentials required).\n";
        cout << "2. Select 'User' to browse categories and book tickets.\n";
        cout << "3. Follow on-screen prompts to select shows, seats, and complete payment.\n";
        cout << "4. For support, use the 'Contact Us' option.\n";
    }

    // Display contact information
    void displayContact() {
        cout << "\n=== Contact Us ===\n";
        cout << "FAST University Islamabad\n";
        cout << "Phone: (051) 111 128 128\n";
        cout << "Email: info@fastbookingsystem.com\n";
    }

public:
    TicketBookingSystem() {
        // Movies
        Category* movies = new Category("Movies");
        movies->addShow("Umro Ayyar: A New Beginning",   "2025-04-14 10:00", 5, 10, 800.0);
        movies->addShow("Paddington in Peru",            "2025-04-14 14:00", 5, 10, 750.0);
        movies->addShow("Despicable Me 4",               "2025-04-14 18:00", 5, 10, 700.0);
        movies->addShow("Khel Khel Mein",                "2025-04-15 10:00", 5, 10, 820.0);
        movies->addShow("Lilo & Stitch",                 "2025-04-15 14:00", 5, 10, 770.0);
        movies->addShow("The Family Plan 2",             "2025-04-15 18:00", 5, 10, 730.0);
        movies->addShow("Dushman-e-Jaan",                "2025-04-16 10:00", 5, 10, 810.0);
        movies->addShow("How to Train Your Dragon",      "2025-04-16 14:00", 5, 10, 760.0);
        movies->addShow("Encanto",                       "2025-04-16 18:00", 5, 10, 720.0);
        movies->addShow("Peechay Tou Dekho",             "2025-04-17 10:00", 5, 10, 830.0);
        movies->addShow("The Super Mario Bros. Movie",   "2025-04-17 14:00", 5, 10, 780.0);
        movies->addShow("Minions: The Rise of Gru",      "2025-04-17 18:00", 5, 10, 740.0);
        movies->addShow("Laal Kabootar",                 "2025-04-18 10:00", 5, 10, 840.0);
        movies->addShow("Wish",                          "2025-04-18 14:00", 5, 10, 790.0);
        movies->addShow("Kung Fu Panda 4",               "2025-04-18 18:00", 5, 10, 750.0);
        categories.push_back(movies);

        // Concerts
        Category* concerts = new Category("Concerts");
        concerts->addShow("Pakistan Fest 2025 @ Jilani Park, Lahore",      "2025-02-14 to 02-16", 10, 20, 2500.0);
        concerts->addShow("Shaam-e-Suroor (Qawwali & DJ Night)",           "2025-05-14",           8, 16, 1800.0);
        concerts->addShow("Soundwaves S1 (Mustafa Zahid Live)",            "2025-05-18",           8, 16, 2000.0);
        concerts->addShow("Colour Fest Islamabad",                        "2025-05-24",           8, 16, 1500.0);
        concerts->addShow("Soul Fest @ Dring Stadium, Bahawalpur",        "2025-05-24 to 05-25",  8, 16, 1600.0);
        concerts->addShow("PSL X Opening Ceremony ft. Abida Parveen",     "2025-04-11",           8, 16, 3000.0);
        concerts->addShow("MHB Tribute to Nusrat Fateh Ali Khan",         "2025-04-19",           8, 16, 2200.0);
        concerts->addShow("Mekaal Hasan Band Live @ Lok Virsa",           "2025-04-26",           8, 16, 2100.0);
        concerts->addShow("Biggest Sufi & Qawwali Night 2025",            "2025-02-01",           8, 16, 1900.0);
        concerts->addShow("6th Sindh Sufi Melo 2025",                     "2025-02-08 to 02-09",  8, 16, 1750.0);
        concerts->addShow("Banjo ke Rung: Ustad Sabzal ke Sang",          "2024-04-27",           8, 16, 1600.0);
        concerts->addShow("The Raah e Ishq Live Show",                    "2024-05-25",           8, 16, 1700.0);
        concerts->addShow("Mehfil-e-Qawwali",                             "2024-06-02",           8, 16, 1550.0);
        concerts->addShow("Summer Fiesta (Aima Baig, Bilal & DJ Night)",  "TBA",                  8, 16,    0.0);
        concerts->addShow("MediaBiz Music Fest (Gul Panra Live)",         "2019-04-27",           8, 16, 1400.0);
        categories.push_back(concerts);

        // Buses
        Category* buses = new Category("Buses");
        buses->addShow("Lahore to Islamabad (Business Class)",    "07:00–11:00", 5, 4, 1200.0);
        buses->addShow("Karachi to Multan (Economy Class)",       "09:30–15:15", 5, 4,  850.0);
        buses->addShow("Islamabad to Peshawar (Executive Class)", "18:00–20:30", 5, 4, 1000.0);
        buses->addShow("Rawalpindi to Swat (Economy Class)",      "08:00–12:00", 5, 4,  900.0);
        buses->addShow("Multan to Lahore (Business Class)",       "17:00–20:00", 5, 4, 1150.0);
        buses->addShow("Quetta to Karachi (Sleeper Class)",       "21:00–07:00", 5, 4, 1400.0);
        buses->addShow("Peshawar to Muzaffarabad (Std Class)",    "10:00–15:00", 5, 4,  950.0);
        buses->addShow("Hyderabad to Sukkur (Economy Class)",     "06:30–10:45", 5, 4,  800.0);
        buses->addShow("Faisalabad to Rawalpindi (Business)",     "13:00–17:30", 5, 4, 1100.0);
        buses->addShow("Sialkot to Lahore (Economy Class)",       "07:30–09:00", 5, 4,  700.0);
        buses->addShow("Gilgit to Islamabad (Executive Class)",   "06:00–12:00", 5, 4, 1300.0);
        buses->addShow("Bahawalpur to Karachi (Sleeper)",         "20:00–06:00", 5, 4, 1450.0);
        buses->addShow("Gwadar to Quetta (Standard Class)",       "16:00–22:00", 5, 4, 1250.0);
        buses->addShow("Skardu to Lahore (Business Class)",       "09:00–15:00", 5, 4, 1350.0);
        buses->addShow("Kashmir to Islamabad (Economy Class)",    "11:00–14:30", 5, 4,  780.0);
        categories.push_back(buses);
    }

    ~TicketBookingSystem() {
        for (size_t i = 0; i < categories.size(); ++i) {
            delete categories[i];
        }
        for (size_t i = 0; i < bookings.size(); ++i) {
            delete bookings[i];
        }
    }

    // Entry point
    void run() {
        while (true) {
            cout << "\n**********************************\n";
            cout << "       FAST BOOKING SYSTEM       \n";
            cout << "**********************************\n";
            cout << "Address: FAST University Islamabad\n";
            cout << "Contact: (051) 111 128 128\n";
            cout << "\n=== Main Menu ===\n";
            cout << "1. Admin Login\n";
            cout << "2. User\n";
            cout << "3. Help\n";
            cout << "4. Contact Us\n";
            cout << "0. Exit\n";
            cout << "Choice: ";
            int choice; cin >> choice;

            if (choice == 1) {
                // Admin authentication
                cout << "Username: ";
                string u; cin >> u;
                cout << "Password: ";
                string p; cin >> p;
                if (u == adminUser && p == adminPass) {
                    cout << "Login successful.\n";
                    adminMenu();
                } else {
                    cout << "Invalid credentials.\n";
                }
            }
            else if (choice == 2) {
                userMenu();
            }
            else if (choice == 3) {
                displayHelp();
            }
            else if (choice == 4) {
                displayContact();
            }
            else if (choice == 0) {
                cout << "Goodbye!\n";
                break;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }
};




int main() {
    TicketBookingSystem app;
    app.run();
    return 0;
}

