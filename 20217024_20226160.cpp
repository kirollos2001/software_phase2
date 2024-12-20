#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <limits>
using namespace std;
enum class Role {
    ADMIN,
    LIBRARIAN,
    USER
};

enum class AccountStatus {
    ACTIVE,
    BANNED,
    DELETED
};

// Book and User classes remain the same as before
class Book {
private:
    std::string bookId;
    std::string name;
    std::string genre;
    int copies;

public:
    Book(const std::string& name, const std::string& genre, int copies)
        : name(name), genre(genre), copies(copies) {
        bookId = "B" + std::to_string(rand());
    }

    std::string getName() const { return name; }
    std::string getGenre() const { return genre; }
    int getCopies() const { return copies; }
    std::string getBookId() const { return bookId; }
};

class User {
protected:
    std::string userId;
    std::string name;
    std::string email;
    std::string password;
    int age;
    std::string gender;
    Role role;
    AccountStatus status;

public:
    User(const std::string& name, const std::string& email,
         const std::string& password, int age, const std::string& gender, Role role)
        : name(name), email(email), password(password),
          age(age), gender(gender), role(role), status(AccountStatus::ACTIVE) {
        userId = "U" + std::to_string(rand());
    }

    virtual ~User() = default;

    std::string getUserId() const { return userId; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    Role getRole() const { return role; }
    AccountStatus getStatus() const { return status; }

    virtual void editProfile(const std::string& newName, const std::string& newPassword,
                           int newAge, const std::string& newGender) {
        if (!newName.empty()) name = newName;
        if (!newPassword.empty()) password = newPassword;
        if (newAge > 0) age = newAge;
        if (!newGender.empty()) gender = newGender;
    }

    bool authenticate(const std::string& inputPassword) const {
        return password == inputPassword;
    }

    void setStatus(AccountStatus newStatus) { status = newStatus; }

    virtual void displayInfo() const {
        std::cout << "User ID: " << userId << "\nName: " << name
                 << "\nEmail: " << email << "\nAge: " << age
                 << "\nGender: " << gender << "\nStatus: "
                 << (status == AccountStatus::ACTIVE ? "Active" :
                     status == AccountStatus::BANNED ? "Banned" : "Deleted")
                 << std::endl;
    }
};

class Admin : public User {
public:
    Admin(const std::string& name, const std::string& email,
          const std::string& password, int age, const std::string& gender)
        : User(name, email, password, age, gender, Role::ADMIN) {}

    void deleteAccount(User& user) {
        user.setStatus(AccountStatus::DELETED);
        std::cout << "Account deleted successfully.\n";
    }

    void banAccount(User& user) {
        user.setStatus(AccountStatus::BANNED);
        std::cout << "Account banned successfully.\n";
    }

    void unbanAccount(User& user) {
        user.setStatus(AccountStatus::ACTIVE);
        std::cout << "Account unbanned successfully.\n";
    }

    void changePassword(User& user, const std::string& newPassword) {
        user.editProfile("", newPassword, 0, "");
        std::cout << "Password changed successfully.\n";
    }
};

class Librarian : public User {
public:
    Librarian(const std::string& name, const std::string& email,
              const std::string& password, int age, const std::string& gender)
        : User(name, email, password, age, gender, Role::LIBRARIAN) {}

    std::shared_ptr<Book> addBook(const std::string& name, const std::string& genre, int copies) {
        return std::make_shared<Book>(name, genre, copies);
    }
};

class LibrarySystem {
private:
    std::map<std::string, std::shared_ptr<User>> users;
    std::map<std::string, std::shared_ptr<Book>> books;

    void clearScreen() {
        std::cout << std::string(50, '\n');
    }

    void waitForEnter() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

public:
    void start() {
        while (true) {
            clearScreen();
            std::cout << "=== Library Management System ===\n"
                     << "1. Sign Up\n"
                     << "2. Login\n"
                     << "3. Exit\n"
                     << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1:
                    handleSignUp();
                    break;
                case 2:
                    handleLogin();
                    break;
                case 3:
                    std::cout << "Goodbye!\n";
                    return;
                default:
                    std::cout << "Invalid option!\n";
            }
            waitForEnter();
        }
    }

private:
    void handleSignUp() {
        std::string name, email, password, gender;
        int age;
        Role role;

        std::cout << "=== Sign Up ===\n";
        std::cout << "Enter name: ";
        std::getline(std::cin, name);
        std::cout << "Enter email: ";
        std::getline(std::cin, email);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);
        std::cout << "Enter age: ";
        std::cin >> age;
        std::cin.ignore();
        std::cout << "Enter gender (M/F): ";
        std::getline(std::cin, gender);

        std::cout << "Select role:\n"
                 << "1. Admin\n"
                 << "2. Librarian\n"
                 << "3. User\n"
                 << "Choice: ";
        int roleChoice;
        std::cin >> roleChoice;

        switch (roleChoice) {
            case 1:
                role = Role::ADMIN;
                break;
            case 2:
                role = Role::LIBRARIAN;
                break;
            default:
                role = Role::USER;
        }

        std::shared_ptr<User> newUser;
        switch (role) {
            case Role::ADMIN:
                newUser = std::make_shared<Admin>(name, email, password, age, gender);
                break;
            case Role::LIBRARIAN:
                newUser = std::make_shared<Librarian>(name, email, password, age, gender);
                break;
            default:
                newUser = std::make_shared<User>(name, email, password, age, gender, Role::USER);
        }

        users[newUser->getUserId()] = newUser;
        std::cout << "Account created successfully!\n";
    }

    void handleLogin() {
        std::string email, password;
        std::cout << "=== Login ===\n";
        std::cout << "Enter email: ";
        std::getline(std::cin, email);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        std::shared_ptr<User> user;
        for (const auto& pair : users) {
            if (pair.second->getEmail() == email && pair.second->authenticate(password)) {
                user = pair.second;
                break;
            }
        }

        if (user && user->getStatus() == AccountStatus::ACTIVE) {
            std::cout << "Login successful!\n";
            handleUserMenu(user);
        } else {
            std::cout << "Invalid credentials or account is not active!\n";
        }
    }

    void handleUserMenu(const std::shared_ptr<User>& user) {
        while (true) {
            clearScreen();
            std::cout << "=== Welcome " << user->getName() << " ===\n";

            switch (user->getRole()) {
                case Role::ADMIN:
                    handleAdminMenu(std::dynamic_pointer_cast<Admin>(user));
                    break;
                case Role::LIBRARIAN:
                    handleLibrarianMenu(std::dynamic_pointer_cast<Librarian>(user));
                    break;
                case Role::USER:
                    handleRegularUserMenu(user);
                    break;
            }

            std::cout << "\nDo you want to logout? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') break;
        }
    }

    void handleAdminMenu(const std::shared_ptr<Admin>& admin) {
        while (true) {
            std::cout << "\n=== Admin Menu ===\n"
                     << "1. View All Users\n"
                     << "2. Ban User\n"
                     << "3. Unban User\n"
                     << "4. Delete User\n"
                     << "5. Change User Password\n"
                     << "6. Edit Own Profile\n"
                     << "7. Logout\n"
                     << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 7) break;

            switch (choice) {
                case 1:
                    viewAllUsers();
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                    handleUserManagement(admin, choice);
                    break;
                case 6:
                    handleProfileEdit(admin);
                    break;
                default:
                    std::cout << "Invalid option!\n";
            }
            waitForEnter();
        }
    }

    void handleLibrarianMenu(const std::shared_ptr<Librarian>& librarian) {
        while (true) {
            std::cout << "\n=== Librarian Menu ===\n"
                     << "1. Add Book\n"
                     << "2. View All Books\n"
                     << "3. Edit Own Profile\n"
                     << "4. Logout\n"
                     << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 4) break;

            switch (choice) {
                case 1:
                    handleAddBook(librarian);
                    break;
                case 2:
                    viewAllBooks();
                    break;
                case 3:
                    handleProfileEdit(librarian);
                    break;
                default:
                    std::cout << "Invalid option!\n";
            }
            waitForEnter();
        }
    }

    void handleRegularUserMenu(const std::shared_ptr<User>& user) {
        while (true) {
            std::cout << "\n=== User Menu ===\n"
                     << "1. View Available Books\n"
                     << "2. Edit Profile\n"
                     << "3. Logout\n"
                     << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 3) break;

            switch (choice) {
                case 1:
                    viewAllBooks();
                    break;
                case 2:
                    handleProfileEdit(user);
                    break;
                default:
                    std::cout << "Invalid option!\n";
            }
            waitForEnter();
        }
    }

    void viewAllUsers() {
        std::cout << "\n=== All Users ===\n";
        for (const auto& pair : users) {
            pair.second->displayInfo();
            std::cout << "---------------\n";
        }
    }

    void viewAllBooks() {
        std::cout << "\n=== All Books ===\n";
        for (const auto& pair : books) {
            const auto& book = pair.second;
            std::cout << "Book ID: " << book->getBookId()
                     << "\nTitle: " << book->getName()
                     << "\nGenre: " << book->getGenre()
                     << "\nCopies: " << book->getCopies()
                     << "\n---------------\n";
        }
    }

    void handleUserManagement(const std::shared_ptr<Admin>& admin, int action) {
        std::string userId;
        std::cout << "Enter user ID: ";
        std::getline(std::cin, userId);

        auto it = users.find(userId);
        if (it == users.end()) {
            std::cout << "User not found!\n";
            return;
        }

        auto& targetUser = *it->second;
        switch (action) {
            case 2:
                admin->banAccount(targetUser);
                break;
            case 3:
                admin->unbanAccount(targetUser);
                break;
            case 4:
                admin->deleteAccount(targetUser);
                break;
            case 5:
                std::cout << "Enter new password: ";
                std::string newPassword;
                std::getline(std::cin, newPassword);
                admin->changePassword(targetUser, newPassword);
                break;
        }
    }

    void handleProfileEdit(const std::shared_ptr<User>& user) {
        std::string newName, newPassword, newGender;
        int newAge;

        std::cout << "Enter new name (or press Enter to skip): ";
        std::getline(std::cin, newName);
        std::cout << "Enter new password (or press Enter to skip): ";
        std::getline(std::cin, newPassword);
        std::cout << "Enter new age (or 0 to skip): ";
        std::cin >> newAge;
        std::cin.ignore();
        std::cout << "Enter new gender (or press Enter to skip): ";
        std::getline(std::cin, newGender);

        user->editProfile(newName, newPassword, newAge, newGender);
        std::cout << "Profile updated successfully!\n";
    }

    void handleAddBook(const std::shared_ptr<Librarian>& librarian) {
        std::string name, genre;
        int copies;

        std::cout << "Enter book name: ";
        std::getline(std::cin, name);
        std::cout << "Enter genre: ";
        std::getline(std::cin, genre);
        std::cout << "Enter number of copies: ";
        std::cin >> copies;

        auto book = librarian->addBook(name, genre, copies);
        books[book->getBookId()] = book;
        std::cout << "Book added successfully!\n";
    }
};

int main() {
    LibrarySystem system;
    system.start();
    return 0;
}
