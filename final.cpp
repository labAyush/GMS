#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <chrono>
#include <ctime>
#include <regex> 
#include <stdexcept>
#include <cmath> // <<< FIX: Include for std::round

// Platform-specific headers for hidden password input
#ifdef _WIN32
#include <conio.h> 
#else
#include <termios.h> 
#include <unistd.h>  
#endif

using namespace std;

// =================================================================================
// ||                   MODULAR INPUT VALIDATION & UTILITIES                      ||
// =================================================================================
namespace InputUtils {

    // Gracefully handles EOF (Ctrl+D/Ctrl+Z) to prevent infinite loops on input failure.
    void handle_cin_eof() {
        if (cin.eof()) {
            cout << "\n\nOperation cancelled by user (EOF detected). Exiting." << endl;
            exit(0);
        }
    }

    // Securely gets password input, hiding characters with '*'.
    string get_hidden_password(const string& prompt) {
        cout << prompt;
        string password = "";
        char c;

    #ifdef _WIN32
        while ((c = _getch()) != '\r') { // '\r' is Enter
            handle_cin_eof();
            if (c == '\b') { // Handle backspace
                if (!password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
            } else {
                password += c;
                cout << '*';
            }
        }
    #else
        termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        while ((c = cin.get()) != '\n') {
            handle_cin_eof();
            if (c == 127 || c == 8) { // Handle backspace
                if (!password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
            } else {
                password += c;
                cout << '*';
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
        cout << endl;
        return password;
    }

    // Generic function to get a numeric input of any type (int, float, double).
    // It validates type, and optionally, a min/max range.
    template <typename T>
    T get_numeric_input(const string& prompt, T min = numeric_limits<T>::min(), T max = numeric_limits<T>::max()) {
        while (true) {
            cout << prompt;
            string line;
            getline(cin, line);
            handle_cin_eof();

            stringstream ss(line);
            T value;
            if (ss >> value && ss.eof()) { // Ensure the whole string was a valid number
                if (value >= min && value <= max) {
                    return value;
                } else {
                    cout << "Error: Input must be between " << min << " and " << max << ". Please try again.\n";
                }
            } else {
                cout << "Error: Invalid input. Please enter a valid number.\n";
            }
        }
    }

    // Ensures the user provides a string that is not empty or just whitespace.
    string get_non_empty_string(const string& prompt) {
        while (true) {
            cout << prompt;
            string value;
            getline(cin, value);
            handle_cin_eof();

            // Trim whitespace from start and end
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);

            if (!value.empty()) {
                return value;
            } else {
                cout << "Error: Input cannot be empty. Please try again.\n";
            }
        }
    }

    // Validates a contact number to ensure it is exactly 10 digits.
    string get_contact_number(const string& prompt) {
        while (true) {
            string contact = get_non_empty_string(prompt);
            if (contact.length() != 10) {
                cout << "Error: Contact number must be exactly 10 digits.\n";
            } else if (!all_of(contact.begin(), contact.end(), ::isdigit)) {
                cout << "Error: Contact number must contain only digits.\n";
            } else {
                return contact;
            }
        }
    }

    // Confirms a user action by requiring 'y' or 'n'.
    bool confirm_action(const string& prompt) {
        while (true) {
            string choice_str = get_non_empty_string(prompt);
            char choice = tolower(choice_str[0]);
            if (choice == 'y') {
                return true;
            } else if (choice == 'n') {
                return false;
            } else {
                cout << "Error: Please enter 'y' for yes or 'n' for no.\n";
            }
        }
    }
    
    // Validates schedule format (e.g., "Mon-10:00")
    string get_valid_schedule(const string& prompt) {
        // Regex for Day (case-insensitive) followed by HH:MM
        const regex pattern("(Mon|Tue|Wed|Thu|Fri|Sat|Sun)-(0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])", std::regex::icase);
        while (true) {
            string schedule = get_non_empty_string(prompt);
            if (regex_match(schedule, pattern)) {
                // Capitalize the day part for consistency
                schedule[0] = toupper(schedule[0]);
                schedule[1] = tolower(schedule[1]);
                schedule[2] = tolower(schedule[2]);
                return schedule;
            } else {
                cout << "Error: Invalid format. Please use Day-HH:MM (e.g., 'Mon-10:00' or 'Sat-14:30').\n";
            }
        }
    }
}
// =================================================================================


struct Trainer {
    int id;
    string name;
    string specialization;
    string contact;
    string password;

    Trainer() {}
    Trainer(int _id, string _name, string _specialization, string _contact, string _password)
        : id(_id), name(_name), specialization(_specialization), contact(_contact), password(_password) {}

    void updateProfile() {
        cout << "\n********************************************************************************\n";
        cout << "*                               UPDATE TRAINER PROFILE                         *\n";
        cout << "********************************************************************************\n";
        name = InputUtils::get_non_empty_string("Enter new name: ");
        specialization = InputUtils::get_non_empty_string("Enter new specialization: ");
        contact = InputUtils::get_contact_number("Enter new contact (10 digits): ");
        password = InputUtils::get_hidden_password("Enter new password: ");
        cout << "Profile updated successfully!\n";
    }

    string toString() const {
        return to_string(id) + "," + name + "," + specialization + "," + contact + "," + password;
    }

    static Trainer fromString(const string& line) {
        Trainer t;
        stringstream ss(line);
        string item;
        try {
            getline(ss, item, ','); t.id = stoi(item);
            getline(ss, t.name, ',');
            getline(ss, t.specialization, ',');
            getline(ss, t.contact, ',');
            getline(ss, t.password, ',');
        } catch (const invalid_argument& e) {
            cerr << "Error parsing Trainer from string: " << line << endl;
        }
        return t;
    }
};

struct Trainee {
    int id;
    string name;
    string contact;
    string password;
    string membership_package; // "Basic" or "Premium"
    int membership_duration_months; // 3 or 6
    string payment_status; // "Paid" or "Due"
    float height_m; // in meters
    float weight_kg; // in kg

    Trainee() : height_m(0), weight_kg(0), membership_duration_months(0) {}
    Trainee(int _id, string _name, string _contact, string _password, string package, int duration)
        : id(_id), name(_name), contact(_contact), password(_password), 
          membership_package(package), membership_duration_months(duration),
          payment_status("Paid"), height_m(0), weight_kg(0) {}

    float calculateBMI() {
        if (height_m > 0 && weight_kg > 0) {
            return weight_kg / (height_m * height_m);
        }
        return 0;
    }
    
    void updateProfile() {
        cout << "\n********************************************************************************\n";
        cout << "*                             UPDATE TRAINEE PROFILE                           *\n";
        cout << "********************************************************************************\n";
        name = InputUtils::get_non_empty_string("Enter new name: ");
        contact = InputUtils::get_contact_number("Enter new contact (10 digits): ");
        password = InputUtils::get_hidden_password("Enter new password: ");
        cout << "Profile updated successfully!\n";
    }

    string toString() const {
        return to_string(id) + "," + name + "," + contact + "," + password + "," + 
               membership_package + "," + to_string(membership_duration_months) + "," + payment_status + "," +
               to_string(height_m) + "," + to_string(weight_kg);
    }

    static Trainee fromString(const string& line) {
        Trainee t;
        stringstream ss(line);
        string item;
        try {
            getline(ss, item, ','); t.id = stoi(item);
            getline(ss, t.name, ',');
            getline(ss, t.contact, ',');
            getline(ss, t.password, ',');
            getline(ss, t.membership_package, ',');
            getline(ss, item, ','); t.membership_duration_months = stoi(item);
            getline(ss, t.payment_status, ',');
            getline(ss, item, ','); t.height_m = stof(item);
            getline(ss, item, ','); t.weight_kg = stof(item);
        } catch (const invalid_argument& e) {
            cerr << "Error parsing Trainee from string: " << line << endl;
        }
        return t;
    }
};

struct GymClass {
    string className;
    string schedule; // Format: "Day-HH:MM", e.g., "Mon-10:00"
    string trainerName;
    int capacity;
    int enrolled;
    vector<int> enrolledTraineeIds;

    GymClass() : capacity(0), enrolled(0) {}
    GymClass(string _className, string _schedule, string _trainerName, int _capacity)
        : className(_className), schedule(_schedule), trainerName(_trainerName),
          capacity(_capacity), enrolled(0) {}

    string toString() const {
        string result = className + "," + schedule + "," + trainerName + "," + 
                       to_string(capacity) + "," + to_string(enrolled) + ",";
        for (size_t i = 0; i < enrolledTraineeIds.size(); i++) {
            result += to_string(enrolledTraineeIds[i]);
            if (i < enrolledTraineeIds.size() - 1) result += ";";
        }
        return result;
    }

    static GymClass fromString(const string& line) {
        GymClass c;
        stringstream ss(line);
        string item;
        try {
            getline(ss, c.className, ',');
            getline(ss, c.schedule, ',');
            getline(ss, c.trainerName, ',');
            getline(ss, item, ','); c.capacity = stoi(item);
            getline(ss, item, ','); c.enrolled = stoi(item);
            
            getline(ss, item, ',');
            if (!item.empty()) {
                stringstream idStream(item);
                string idStr;
                while (getline(idStream, idStr, ';')) {
                    if (!idStr.empty()) {
                        c.enrolledTraineeIds.push_back(stoi(idStr));
                    }
                }
            }
        } catch (const invalid_argument& e) {
             cerr << "Error parsing GymClass from string: " << line << endl;
        }
        return c;
    }
};

class GymManagement {
private:
    const string TRAINEE_FILE = "trainees.txt";
    const string TRAINER_FILE = "trainers.txt";
    const string CLASS_FILE = "classes.txt";
    const string ADMIN_FILE = "admins.txt";

    int getCost(const string& package, int duration) {
        if (package == "Basic") {
            return (duration == 3) ? 100 : 180;
        } else if (package == "Premium") {
            return (duration == 3) ? 150 : 270;
        }
        return 0;
    }

public:
    GymManagement() {
        initializeAdminFile();
    }

    void initializeAdminFile() {
        ifstream file(ADMIN_FILE);
        if (!file.good()) {
            ofstream outFile(ADMIN_FILE);
            outFile << "admin,admin123\n";
            outFile.close();
        }
    }

    // --- Data Loading and Saving (unchanged, but now safer due to parsing checks) ---
    vector<Trainee> loadTrainees() {
        vector<Trainee> trainees;
        ifstream file(TRAINEE_FILE);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) trainees.push_back(Trainee::fromString(line));
        }
        file.close();
        return trainees;
    }
    void saveTrainees(const vector<Trainee>& trainees) {
        ofstream file(TRAINEE_FILE);
        for (const auto& t : trainees) file << t.toString() << "\n";
        file.close();
    }
    vector<Trainer> loadTrainers() { 
        vector<Trainer> trainers;
        ifstream file(TRAINER_FILE);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) trainers.push_back(Trainer::fromString(line));
        }
        file.close();
        return trainers;
    }
    void saveTrainers(const vector<Trainer>& trainers) { 
        ofstream file(TRAINER_FILE);
        for (const auto& t : trainers) file << t.toString() << "\n";
        file.close();
    }
    vector<GymClass> loadClasses() { 
        vector<GymClass> classes;
        ifstream file(CLASS_FILE);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) classes.push_back(GymClass::fromString(line));
        }
        file.close();
        return classes;
    }
    void saveClasses(const vector<GymClass>& classes) { 
        ofstream file(CLASS_FILE);
        for (const auto& c : classes) file << c.toString() << "\n";
        file.close();
    }

    // --- User Registration and Management (ENHANCED) ---
    void registerTrainee() {
        cout << "\n********************************************************************************\n";
        cout << "*                               REGISTER TRAINEE                              *\n";
        cout << "********************************************************************************\n";
        
        vector<Trainee> trainees = loadTrainees();
        int id;
        while (true) {
            id = InputUtils::get_numeric_input<int>("Enter new Trainee ID: ");
            bool id_exists = false;
            for (const auto& t : trainees) {
                if (t.id == id) {
                    id_exists = true;
                    break;
                }
            }
            if (id_exists) {
                cout << "Error: ID already exists. Please try a different ID.\n";
            } else {
                break;
            }
        }
        
        string name = InputUtils::get_non_empty_string("Enter Name: ");
        string contact = InputUtils::get_contact_number("Enter Contact (10 digits): ");

        cout << "--- Membership Packages ---\n";
        cout << "1. Basic (Access to gym floor)\n";
        cout << "2. Premium (Access to gym floor + all classes)\n";
        int package_choice = InputUtils::get_numeric_input<int>("Choose package (1-2): ", 1, 2);
        string package = (package_choice == 2) ? "Premium" : "Basic";

        cout << "--- Membership Duration ---\n";
        cout << "1. 3 Months\n";
        cout << "2. 6 Months\n";
        int duration_choice = InputUtils::get_numeric_input<int>("Choose duration (1-2): ", 1, 2);
        int package_duration = (duration_choice == 2) ? 6 : 3;

        int cost = getCost(package, package_duration);
        cout << "Total cost for " << package << " membership for " << package_duration << " months is $" << cost << ".\n";
        
        if (!InputUtils::confirm_action("Confirm registration? (y/n): ")) {
            cout << "Registration cancelled.\n";
            return;
        }

        string password = InputUtils::get_hidden_password("Create Password: ");

        trainees.push_back(Trainee(id, name, contact, password, package, package_duration));
        saveTrainees(trainees);
        cout << "Trainee registered and payment confirmed successfully!\n";
    }

    void addTrainer() {
        cout << "\n********************************************************************************\n";
        cout << "*                               ADD TRAINER                                   *\n";
        cout << "********************************************************************************\n";
        
        vector<Trainer> trainers = loadTrainers();
        int id;
        while (true) {
            id = InputUtils::get_numeric_input<int>("Enter Trainer ID: ");
            bool id_exists = false;
            for (const auto& t : trainers) {
                if (t.id == id) {
                    id_exists = true;
                    break;
                }
            }
            if (id_exists) {
                 cout << "Error: ID already exists. Operation aborted.\n";
            } else {
                break;
            }
        }
        
        string name = InputUtils::get_non_empty_string("Enter Name: ");
        string specialization = InputUtils::get_non_empty_string("Enter Specialization: ");
        string contact = InputUtils::get_contact_number("Enter Contact (10 digits): ");
        string password = InputUtils::get_hidden_password("Enter Password: ");

        trainers.push_back(Trainer(id, name, specialization, contact, password));
        saveTrainers(trainers);
        cout << "Trainer added successfully!\n";
    }
    
    // --- Login Functions (ENHANCED) ---
    bool adminLogin() {
        cout << "\n********************************************************************************\n";
        cout << "*                                ADMIN LOGIN                                 *\n";
        cout << "********************************************************************************\n";
        string username = InputUtils::get_non_empty_string("Username: ");
        string password = InputUtils::get_hidden_password("Password: ");
        
        ifstream file(ADMIN_FILE);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string storedUsername, storedPassword;
            getline(ss, storedUsername, ',');
            getline(ss, storedPassword, ',');
            
            if (storedUsername == username && storedPassword == password) {
                cout << "Admin login successful!\n";
                file.close();
                return true;
            }
        }
        file.close();
        cout << "Invalid credentials!\n";
        return false;
    }

    bool trainerLogin(Trainer& loggedInTrainer) {
        cout << "\n********************************************************************************\n";
        cout << "*                               TRAINER LOGIN                               *\n";
        cout << "********************************************************************************\n";
        int id = InputUtils::get_numeric_input<int>("Trainer ID: ");
        string password = InputUtils::get_hidden_password("Password: ");
        
        vector<Trainer> trainers = loadTrainers();
        for (const auto& t : trainers) {
            if (t.id == id && t.password == password) {
                cout << "Trainer login successful!\n";
                loggedInTrainer = t;
                return true;
            }
        }
        cout << "Invalid credentials!\n";
        return false;
    }

    bool traineeLogin(Trainee& loggedInTrainee) {
        cout << "\n********************************************************************************\n";
        cout << "*                              TRAINEE LOGIN                                *\n";
        cout << "********************************************************************************\n";
        int id = InputUtils::get_numeric_input<int>("Trainee ID: ");
        string password = InputUtils::get_hidden_password("Password: ");
        
        vector<Trainee> trainees = loadTrainees();
        for (const auto& t : trainees) {
            if (t.id == id && t.password == password) {
                cout << "Login successful!\n";
                loggedInTrainee = t;
                return true;
            }
        }
        cout << "Invalid credentials!\n";
        return false;
    }

    // --- Class and Schedule Management (ENHANCED) ---
    void addClass() {
        cout << "\n********************************************************************************\n";
        cout << "*                               ADD CLASS                                    *\n";
        cout << "********************************************************************************\n";
        string className = InputUtils::get_non_empty_string("Enter Class Name (e.g., 'Leg Day', 'Yoga', 'HIIT'): ");
        string schedule = InputUtils::get_valid_schedule("Enter Schedule (Format: Day-HH:MM, e.g., 'Mon-10:00'): ");
        string trainerName = InputUtils::get_non_empty_string("Enter Trainer Name (must exist): ");
        int capacity = InputUtils::get_numeric_input<int>("Enter Capacity: ", 1, 100);

        vector<Trainer> trainers = loadTrainers();
        bool found = false;
        for (const auto& t : trainers) {
            if (t.name == trainerName) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Trainer name not found! Class not added.\n";
            return;
        }
        
        vector<GymClass> classes = loadClasses();
        classes.push_back(GymClass(className, schedule, trainerName, capacity));
        saveClasses(classes);
        cout << "Class added successfully!\n";
    }

    void displayWeeklySchedule() {
        cout << "\n********************************************************************************\n";
        cout << "*                             WEEKLY CLASS SCHEDULE                            *\n";
        cout << "********************************************************************************\n";
        vector<GymClass> classes = loadClasses();
        if (classes.empty()) {
            cout << "No classes have been scheduled for the week.\n";
            return;
        }

        sort(classes.begin(), classes.end(), [](const GymClass& a, const GymClass& b) {
            return a.schedule < b.schedule;
        });

        vector<string> days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        for (const auto& day : days) {
            cout << "\n--- " << day << " --------------------------------------------------------------------\n";
            bool foundClassForDay = false;
            for (const auto& c : classes) {
                if (c.schedule.rfind(day, 0) == 0) { 
                    string time = c.schedule.substr(c.schedule.find('-') + 1);
                    cout << "  " << time << "   " << c.className 
                         << " (" << c.trainerName << ")"
                         << "   -   Enrolled: " << c.enrolled << "/" << c.capacity << "\n";
                    foundClassForDay = true;
                }
            }
            if (!foundClassForDay) {
                cout << "  No classes scheduled for this day.\n";
            }
        }
    }

    void displayDailySchedule() {
        auto now = chrono::system_clock::now();
        time_t tt = chrono::system_clock::to_time_t(now);
        
        #ifdef _WIN32
            tm local_tm;
            localtime_s(&local_tm, &tt);
        #else
            tm local_tm = *localtime(&tt);
        #endif

        vector<string> days = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        string today = days[local_tm.tm_wday];

        cout << "\n********************************************************************************\n";
        cout << "*                        CLASSES FOR TODAY (" << today << ")                             *\n";
        cout << "********************************************************************************\n";
        
        vector<GymClass> classes = loadClasses();
        bool foundClassToday = false;

        sort(classes.begin(), classes.end(), [](const GymClass& a, const GymClass& b) {
            return a.schedule < b.schedule;
        });

        for (const auto& c : classes) {
            if (c.schedule.rfind(today, 0) == 0) {
                 string time = c.schedule.substr(c.schedule.find('-') + 1);
                 cout << "  " << time << "   " << c.className 
                      << " (" << c.trainerName << ")"
                      << "   -   Enrolled: " << c.enrolled << "/" << c.capacity << "\n";
                 foundClassToday = true;
            }
        }
        if (!foundClassToday) {
            cout << "No classes are scheduled for today. Take a rest day!\n";
        }
    }

    void signUpForClass(Trainee& trainee, string className) {
        if (trainee.membership_package != "Premium") {
            cout << "Class sign-up is a Premium feature. Please upgrade your membership.\n";
            return;
        }
        vector<GymClass> classes = loadClasses();
        bool classFound = false;
        
        for (auto& c : classes) {
            if (c.className == className) {
                classFound = true;
                
                if (find(c.enrolledTraineeIds.begin(), c.enrolledTraineeIds.end(), trainee.id) != c.enrolledTraineeIds.end()) {
                    cout << "You are already enrolled in this class.\n";
                    return;
                }
                
                if (c.enrolled < c.capacity) {
                    c.enrolled++;
                    c.enrolledTraineeIds.push_back(trainee.id);
                    saveClasses(classes);
                    cout << trainee.name << " signed up successfully for " << className << "!\n";
                } else {
                    cout << "Class is full!\n";
                }
                return;
            }
        }
        
        if (!classFound) {
            cout << "Class not found!\n";
        }
    }
    
    void viewPaymentStatus() {
        cout << "\n********************************************************************************\n";
        cout << "*                            TRAINEE PAYMENT STATUS                            *\n";
        cout << "********************************************************************************\n";
        vector<Trainee> trainees = loadTrainees();
        if (trainees.empty()) {
            cout << "No trainees registered.\n";
            return;
        }
        
        for (const auto& t : trainees) {
            cout << "ID: " << t.id << ", Name: " << t.name 
                 << ", Package: " << t.membership_package << " (" << t.membership_duration_months << " months)"
                 << ", Status: " << t.payment_status << "\n";
        }
    }

    // --- Generic Utility Functions (Mostly unchanged, now called by robust functions) ---
    void updateTraineeInFile(const Trainee& updatedTrainee) {
        vector<Trainee> trainees = loadTrainees();
        for (auto& t : trainees) {
            if (t.id == updatedTrainee.id) {
                t = updatedTrainee;
                break;
            }
        }
        saveTrainees(trainees);
    }
    
    Trainee* searchTraineeByID(int id, vector<Trainee>& trainees) { 
        for (auto& t : trainees) {
            if (t.id == id) return &t;
        }
        return nullptr;
    }
    void displayTrainees() { 
        cout << "\n********************************************************************************\n";
        cout << "*                               TRAINEES LIST                               *\n";
        cout << "********************************************************************************\n";
        
        vector<Trainee> trainees = loadTrainees();
        if (trainees.empty()) {
            cout << "No trainees enrolled.\n";
            return;
        }
        
        for (const auto& t : trainees) {
            cout << "ID: " << t.id << ", Name: " << t.name << ", Contact: " << t.contact << ", Membership: " << t.membership_package << "\n";
        }
    }
        void provideBmiFeedback(float bmi) {
        cout << "\n--- General Fitness Feedback ---\n";
        if (bmi < 18.5) {
            cout << "Category: Underweight\n";
            cout << "Suggestion: Focus on strength training to build healthy muscle mass. Consider consulting a nutritionist to ensure you're getting enough calories and nutrients.\n";
        } else if (bmi >= 18.5 && bmi < 25) {
            cout << "Category: Normal Weight\n";
            cout << "Suggestion: Great job! Maintain your health with a balanced routine of cardiovascular exercise (like running or cycling) and strength training.\n";
        } else if (bmi >= 25 && bmi < 30) {
            cout << "Category: Overweight\n";
            cout << "Suggestion: A combination of consistent cardiovascular exercise and resistance training is recommended. Seeking professional dietary advice can also be very beneficial.\n";
        } else { // bmi >= 30
            cout << "Category: Obese\n";
            cout << "Suggestion: It's recommended to combine consistent cardiovascular exercise with resistance training. Please consider seeking professional dietary advice for a personalized plan.\n";
        }
        cout << "\n** DISCLAIMER **\n";
        cout << "These suggestions are for informational purposes only and do not constitute professional medical advice. Always consult with a healthcare provider before starting any new fitness or diet program.\n";
    }
    void deleteTrainee(int id) { 
        vector<Trainee> trainees = loadTrainees();
        auto it = remove_if(trainees.begin(), trainees.end(), 
                           [id](const Trainee& t) { return t.id == id; });
        
        if (it != trainees.end()) {
            trainees.erase(it, trainees.end());
            saveTrainees(trainees);
            
            vector<GymClass> classes = loadClasses();
            for (auto& c : classes) {
                auto idIt = find(c.enrolledTraineeIds.begin(), c.enrolledTraineeIds.end(), id);
                if (idIt != c.enrolledTraineeIds.end()) {
                    c.enrolledTraineeIds.erase(idIt);
                    c.enrolled--;
                }
            }
            saveClasses(classes);
            
            cout << "Trainee deleted successfully!\n";
        } else {
            cout << "Trainee not found!\n";
        }
    }
    void deleteTrainer(int id) { 
        vector<Trainer> trainers = loadTrainers();
        string trainerNameToDelete;
        bool found = false;

        for (const auto& t : trainers) {
            if (t.id == id) {
                trainerNameToDelete = t.name;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Trainer not found!\n";
            return;
        }
        
        auto it = remove_if(trainers.begin(), trainers.end(), 
                           [id](const Trainer& t) { return t.id == id; });
        trainers.erase(it, trainers.end());
        saveTrainers(trainers);

        vector<GymClass> classes = loadClasses();
        auto classIt = remove_if(classes.begin(), classes.end(),
                               [&trainerNameToDelete](const GymClass& c) { return c.trainerName == trainerNameToDelete; });
        if (classIt != classes.end()) {
            classes.erase(classIt, classes.end());
            saveClasses(classes);
            cout << "Trainer and associated classes deleted successfully!\n";
        } else {
            cout << "Trainer deleted successfully! (No associated classes found)\n";
        }
    }
    void searchTrainerByID(int id) { 
        vector<Trainer> trainers = loadTrainers();
        for (const auto& t : trainers) {
            if (t.id == id) {
                cout << "\n********************************************************************************\n";
                cout << "*                              TRAINER DETAILS                                *\n";
                cout << "********************************************************************************\n";
                cout << "ID: " << t.id << "\n";
                cout << "Name: " << t.name << "\n";
                cout << "Specialization: " << t.specialization << "\n";
                cout << "Contact: " << t.contact << "\n";
                return;
            }
        }
        cout << "Trainer not found!\n";
    }
    void deleteClass(const string& className) { 
        vector<GymClass> classes = loadClasses();
        auto it = remove_if(classes.begin(), classes.end(), 
                           [className](const GymClass& c) { return c.className == className; });
        
        if (it != classes.end()) {
            classes.erase(it, classes.end());
            saveClasses(classes);
            cout << "Class '" << className << "' deleted successfully!\n";
        } else {
            cout << "Class not found!\n";
        }
    }
    void displayTrainers() { 
        cout << "\n********************************************************************************\n";
        cout << "*                               TRAINERS LIST                                *\n";
        cout << "********************************************************************************\n";
        
        vector<Trainer> trainers = loadTrainers();
        if (trainers.empty()) {
            cout << "No trainers registered.\n";
            return;
        }
        
        for (const auto& t : trainers) {
            cout << "ID: " << t.id << ", Name: " << t.name << ", Specialization: " << t.specialization << ", Contact: " << t.contact << "\n";
        }
    }
    void updateTrainerInFile(const Trainer& updatedTrainer) { 
        vector<Trainer> trainers = loadTrainers();
        for (auto& t : trainers) {
            if (t.id == updatedTrainer.id) {
                t = updatedTrainer;
                break;
            }
        }
        saveTrainers(trainers);
    }
    void displayClassesForTrainer(const Trainer& trainer) { 
        cout << "\n********************************************************************************\n";
        cout << "*                      CLASSES TAUGHT BY " << trainer.name << "                     *\n";
        cout << "********************************************************************************\n";
        
        vector<GymClass> classes = loadClasses();
        bool foundAny = false;
        
        for (const auto& c : classes) {
            if (c.trainerName == trainer.name) {
                cout << "Class: " << c.className << ", Schedule: " << c.schedule << ", Capacity: " << c.capacity << ", Enrolled: " << c.enrolled << "\n";
                foundAny = true;
            }
        }
        
        if (!foundAny) cout << "No classes assigned.\n";
    }
    void displayTraineesForTrainer(const Trainer& trainer) { 
        cout << "\n********************************************************************************\n";
        cout << "*                  TRAINEES IN CLASSES TAUGHT BY " << trainer.name << "               *\n";
        cout << "********************************************************************************\n";
        
        vector<GymClass> classes = loadClasses();
        vector<Trainee> trainees = loadTrainees();
        bool foundAny = false;
        
        for (const auto& c : classes) {
            if (c.trainerName == trainer.name) {
                cout << "Class: " << c.className << "\n";
                if (c.enrolledTraineeIds.empty()) {
                    cout << "  No trainees enrolled.\n";
                } else {
                    for (auto tid : c.enrolledTraineeIds) {
                        for (const auto& trainee : trainees) {
                            if (trainee.id == tid) {
                                cout << "  ID: " << trainee.id << ", Name: " << trainee.name << "\n";
                                break;
                            }
                        }
                    }
                }
                foundAny = true;
            }
        }
        
        if (!foundAny) cout << "No classes assigned, thus no trainees.\n";
    }
};

// =================================================================================
// ||                           MAIN PROGRAM LOGIC                                ||
// =================================================================================

int main() {
    GymManagement gym;

    cout << "********************************************************************************\n";
    cout << "*                                                                              *\n";
    cout << "*                     WELCOME TO THE GYM MANAGEMENT SYSTEM                     *\n";
    cout << "*                                                                              *\n";
    cout << "********************************************************************************\n";

    while (true) {
        cout << "\n------------------------------------------------------------------------------\n";
        cout << "| Are you an ADMIN, TRAINER, or TRAINEE? (Enter 'exit' to quit)              |\n";
        cout << "------------------------------------------------------------------------------\n";
        string userType = InputUtils::get_non_empty_string("Enter your role: ");
        transform(userType.begin(), userType.end(), userType.begin(), ::tolower); // convert to lowercase

        if (userType == "exit") break;

        if (userType == "admin") {
            if (gym.adminLogin()) {
                while (true) {
                    cout << "\n------------------------- ADMIN MENU -------------------------\n";
                    cout << "1. Add Trainer              7. Delete Trainee\n";
                    cout << "2. Add Class                8. Delete Trainer\n";
                    cout << "3. Display Weekly Schedule  9. Delete Class\n";
                    cout << "4. Display Today's Schedule 10. Display All Trainees\n";
                    cout << "5. Search Trainee by ID     11. Display All Trainers\n";
                    cout << "6. Search Trainer by ID     12. View Trainee Payments\n";
                    cout << "13. Logout\n";
                    cout << "------------------------------------------------------------\n";
                    int choice = InputUtils::get_numeric_input<int>("Enter choice (1-13): ", 1, 13);

                    if (choice == 1) gym.addTrainer();
                    else if (choice == 2) gym.addClass();
                    else if (choice == 3) gym.displayWeeklySchedule();
                    else if (choice == 4) gym.displayDailySchedule();
                    else if (choice == 5) {
                        int id = InputUtils::get_numeric_input<int>("Enter Trainee ID to search: ");
                        vector<Trainee> trainees = gym.loadTrainees();
                        Trainee* t = gym.searchTraineeByID(id, trainees);
                        if (t) cout << "ID: " << t->id << ", Name: " << t->name << ", Contact: " << t->contact << ", Membership: " << t->membership_package << "\n";
                        else cout << "Trainee not found!\n";
                    }
                    else if (choice == 6) {
                        int id = InputUtils::get_numeric_input<int>("Enter Trainer ID to search: ");
                        gym.searchTrainerByID(id);
                    }
                    else if (choice == 7) {
                        int id = InputUtils::get_numeric_input<int>("Enter Trainee ID to delete: ");
                        gym.deleteTrainee(id);
                    }
                    else if (choice == 8) {
                        int id = InputUtils::get_numeric_input<int>("Enter Trainer ID to delete: ");
                        gym.deleteTrainer(id);
                    }
                    else if (choice == 9) {
                        string className = InputUtils::get_non_empty_string("Enter Class Name to delete: ");
                        gym.deleteClass(className);
                    }
                    else if (choice == 10) gym.displayTrainees();
                    else if (choice == 11) gym.displayTrainers();
                    else if (choice == 12) gym.viewPaymentStatus();
                    else if (choice == 13) break; // Logout
                }
            }
        } else if (userType == "trainer") {
            Trainer trainer;
            if (gym.trainerLogin(trainer)) {
                while (true) {
                    cout << "\n------------------- TRAINER MENU (" << trainer.name << ") -------------------\n";
                    cout << "1. View Profile\n";
                    cout << "2. View Your Classes\n";
                    cout << "3. View Your Trainees\n";
                    cout << "4. Update Profile\n";
                    cout << "5. Logout\n";
                    cout << "------------------------------------------------------------\n";
                    int choice = InputUtils::get_numeric_input<int>("Enter choice (1-5): ", 1, 5);
                    
                    if (choice == 1) cout << "\nTrainer Profile:\nName: " << trainer.name << "\nSpecialization: " << trainer.specialization << "\nContact: " << trainer.contact << "\n";
                    else if (choice == 2) gym.displayClassesForTrainer(trainer);
                    else if (choice == 3) gym.displayTraineesForTrainer(trainer);
                    else if (choice == 4) {
                        trainer.updateProfile();
                        gym.updateTrainerInFile(trainer);
                    }
                    else if (choice == 5) break;
                }
            }
        } else if (userType == "trainee") {
            cout << "\n------------------------------------------------------------\n";
            cout << "|                    TRAINEE / USER MENU                   |\n";
            cout << "------------------------------------------------------------\n";
            cout << "| 1. Register                                              |\n";
            cout << "| 2. Login                                                 |\n";
            cout << "------------------------------------------------------------\n";
            int choice = InputUtils::get_numeric_input<int>("Enter choice (1-2): ", 1, 2);
            
            if (choice == 1) {
                gym.registerTrainee();
            } else if (choice == 2) {
                Trainee trainee;
                if (gym.traineeLogin(trainee)) {
                    while (true) {
                        cout << "\n------------------- TRAINEE MENU (" << trainee.name << ") -------------------\n";
                        cout << "1. View Profile              5. Calculate BMI\n";
                        cout << "2. View Today's Schedule     6. Sign Up for a Class\n";
                        cout << "3. View Weekly Schedule      7. Logout\n";
                        cout << "4. Update Profile\n";
                        cout << "------------------------------------------------------------\n";
                        int menu_choice = InputUtils::get_numeric_input<int>("Enter choice (1-7): ", 1, 7);
                        
                        if (menu_choice == 1) {
                             cout << "\nProfile:\nName: " << trainee.name << "\nContact: " << trainee.contact 
                                 << "\nMembership: " << trainee.membership_package << " (" << trainee.membership_duration_months << " months)"
                                 << "\nPayment Status: " << trainee.payment_status << "\n";
                        }
                        else if (menu_choice == 2) gym.displayDailySchedule();
                        else if (menu_choice == 3) gym.displayWeeklySchedule();
                        else if (menu_choice == 4) {
                            trainee.updateProfile();
                            gym.updateTraineeInFile(trainee);
                        }
                        else if (menu_choice == 5) {
                            trainee.height_m = InputUtils::get_numeric_input<float>("Enter height (meters): ", 0.5f, 3.0f);
                            trainee.weight_kg = InputUtils::get_numeric_input<float>("Enter weight (kg): ", 20.0f, 300.0f);
                            float bmi = trainee.calculateBMI();
                            if (bmi > 0) {
                            // <<< FIX: Changed round to std::round and added <cmath> header
                            cout << "Your BMI is: " << std::round(bmi * 100) / 100 << "\n";
                            gym.provideBmiFeedback(bmi); // <<< ADD THIS LINE
                            gym.updateTraineeInFile(trainee);
                            } else{
                                cout << "Could not calculate BMI with the provided values.\n";
                            }
                        }
                        else if (menu_choice == 6) {
                            string className = InputUtils::get_non_empty_string("Enter the full Class Name to sign up for: ");
                            gym.signUpForClass(trainee, className);
                        }
                        else if (menu_choice == 7) break; // Logout
                    }
                }
            }
        } else {
            cout << "\n------------------------------------------------------------------------------\n";
            cout << "| Invalid input! Please enter 'admin', 'trainer' or 'trainee'.               |\n";
            cout << "------------------------------------------------------------------------------\n";
        }
    }
    
    cout << "\nExiting Gym Management System. Goodbye!\n";
    return 0;
}