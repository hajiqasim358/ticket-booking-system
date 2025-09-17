# 🎟️ Ticket Booking System (C++)

![C++](https://img.shields.io/badge/C++-17-blue.svg)  
![Platform](https://img.shields.io/badge/Platform-Console-lightgrey.svg)  
![Status](https://img.shields.io/badge/Status-Completed-brightgreen.svg)  

A **console-based Ticket Booking System** developed in **C++** as a semester project.  
It provides **user and admin dashboards**, seat management, booking IDs, and a simulated payment system.  

---

## 📌 Features
- **User Dashboard**
  - Browse categories: 🎬 Movies, 🎶 Concerts, 🚌 Buses  
  - View shows with timings, ticket prices, and available seats  
  - Select seats and confirm bookings  
  - Simulated payment gateway with card details input  
  - Generate unique booking ID (e.g., `BK12345`)  

- **Admin Dashboard**
  - Login with admin credentials  
  - Manage categories (add, delete, rename)  
  - Manage shows (add, edit, delete)  
  - View all booked tickets with details  

- **System Highlights**
  - Seat availability tracking (row × column)  
  - OOP-based modular design (`Seat`, `Show`, `Category`, `Booking`, `PaymentProcessor`, `TicketBookingSystem`)  
  - Cross-platform (Windows/Linux support)  

---

## 🛠️ Technologies Used
- **Language:** C++  
- **Libraries:** STL (`vector`, `string`, `ctime`, etc.)  
- **Platform:** Console-based (supports Windows/Linux)  

---

## 🚀 Getting Started

### 1. Clone Repository
```bash
git clone https://github.com/your-username/ticket-booking-system.git
cd ticket-booking-system
