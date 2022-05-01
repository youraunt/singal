#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sys/syscall.h>
#include <sys/types.h>

void safe_sleep(int);

static void daysLeft(const char *date);

static void sigint_handler(int i1);

static void sigalrm_handler(int num);

bool loopProgram();

bool userIn();

static inline void sig_error() {
    std::cerr << "Somethings wrong I cannot get your signal." << std::endl;
}

/// @brief Exits program successfully with message
inline std::string exitProgram() {
    std::cout << "Exiting program!" << std::endl;
    pid_t tid;
    tid = syscall(SYS_gettid);
    syscall(SYS_tgkill, getpid(), tid);
    exit(EXIT_SUCCESS);
}
///

inline void unknownInput() {
    std::cerr << "\nError?! Unknown input.\n" << std::endl;
    userIn();
}

#define WHAT_IS(x) std::cerr << #x << " is " << x << std::endl;

bool userIn() {
    char ch = '\0';
    printf("Do you want to continue: y/n");
    scanf(" %c", &ch);
    std::tolower(ch, std::locale());
    if (ch != 'y' && ch != 'n')unknownInput();
    if (ch == 'n')exitProgram();
    return ch != 'n';
}

///
/// \return


bool loopProgram() {
    int z;
    z = sizeof(int) * 128;
    do {
        --z;
        if (signal(SIGINT, sigint_handler) == SIG_ERR)
            sig_error();
        if (signal(SIGALRM, sigalrm_handler) == SIG_ERR)
            sig_error();
        alarm(1);
        pause();
        if (z == 0) break;
    } while (true);
    return false;
}

class lab4 {
private:

    int alarm_counter{0};
public:
    void setAlarmCounter(int i) {
        alarm_counter = i;
    }

    [[nodiscard]] inline int getAlarmCounter() {
        return alarm_counter;
    }

    void inline iterateAlarmCounter(int i) {
        alarm_counter += i;
    }

/// posix safe local time I work in all the os
/// \param timer
/// \return
    [[nodiscard]] static inline std::tm localtime_os(std::time_t timer) {
        std::tm local_t{};
#if defined(__unix__)
        localtime_r(&timer, &local_t);
#elif defined(_MSC_VER)
        localtime_s(&tm, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    tm = *std::localtime(&timer);
#endif
        return local_t;

    }

    ///
    /// default = "YYYY-MM-DD HH:MM:SS"
    inline static std::string time_stamp(const std::string &fmt = "%F %T") {
        auto tp = localtime_os(std::time(nullptr));
        char buffer[64];
        return {buffer, std::strftime(buffer, sizeof(buffer), fmt.c_str(), &tp)};
    }

    ///
    /// requirement 2019-04-06 Saturday 11:45:44 PM, MDT
    inline static std::string time_stamp_class_format(const std::string &fmt = "%F %A %T %p, %Z") {
        auto tp = localtime_os(std::time(nullptr));
        char buffer[64];
        return {buffer, std::strftime(buffer, sizeof(buffer), fmt.c_str(), &tp)};
    }

    lab4() {
//        time_t t = time(0);
//        localtime_r(&t, &tm);
    }


};

lab4 lab;

class Date {
private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

public:
    friend std::ostream &operator<<(std::ostream &os, const Date &date);

    [[nodiscard]]  int getYear() const {
        return year;
    }

    [[nodiscard]] int getMonth() const {
        return month;
    }

    [[nodiscard]] int getDay() const {
        return day;
    }

    [[nodiscard]] int getHour() const {
        return hour;
    }

    void setHour(int i) {
        Date::hour = i;
    }

    [[nodiscard]] int getMinute() const {
        return minute;
    }

    void setMinute(int i) {
        Date::minute = i;
    }

    [[nodiscard]] int getSecond() const {
        return second;
    }

    void setSecond(int i) {
        Date::second = i;
    }

    ///
    /// \param str
    explicit Date(std::string str) {
        if (dateValidator(str)) {
            year = (int) strtol(&str[0], nullptr, 10);
            month = (int) strtol(&str[5], nullptr, 10);
            day = (int) strtol(&str[8], nullptr, 10);
            hour = (int) strtol(&str[11], nullptr, 10);
            minute = (int) strtol(&str[14], nullptr, 10);
            second = (int) strtol(&str[17], nullptr, 10);
        } else {
            throw std::invalid_argument("Invalid date");
        }
    }


    ///
    /// \param date_string
    /// \return YYYY-MM-DD
    static bool dateValidator(std::string date_string) {
        if (date_string[4] != '-' || date_string[7] != '-') return false;
        if (!isdigit(date_string[0]) || !isdigit(date_string[1]) || !isdigit(date_string[2]) ||
            !isdigit(date_string[3]))
            return false;
        if (!isdigit(date_string[5]) || !isdigit(date_string[6])) return false;
        if (!isdigit(date_string[8]) || !isdigit(date_string[9])) return false;

        if (date_string.length() == 19 && (!isdigit(date_string[11]) || !isdigit(date_string[12]))) return false;

        auto year = (int) strtol(&date_string[0], nullptr, 10);
        auto month = (int) strtol(&date_string[5], nullptr, 10);
        auto day = (int) strtol(&date_string[8], nullptr, 10);

        if (year <= 0 || month <= 0 || day <= 0) return false;
        if (month > 12) return false;

        switch (month) {
            case 2:
                if (day > 29) return false;
                break;
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                if (day > 31) return false;
                break;
            default:
                if (day > 30) return false;
                break;
        }
        return true;
    }


};

///
/// \param os
/// \param date
/// \return
std::ostream &operator<<(std::ostream &os, const Date &date) {
    os << " year: " << date.year
       << " month: " << date.month
       << " day: " << date.day
       << " hour: " << date.hour
       << " minute: " << date.minute
       << " second: " << date.second;
    return os;
}

///
/// \param d1
/// \param d2
/// \return
int diffDays(Date d1, Date d2) {
    auto d1m = (d1.getMonth() + 9) % 12;
    auto d1y = d1.getYear() - d1m / 10;
    auto d2m = (d2.getMonth() + 9) % 12;
    auto d2y = d2.getYear() - d2m / 10;
    auto days1 = 365 * d1y + d1y / 4 - d1y / 100 + d1y / 400 + (d1m * 306 + 5) / 10 + (d1.getDay() - 1);
    auto days2 = 365 * d2y + d2y / 4 - d2y / 100 + d2y / 400 + (d2m * 306 + 5) / 10 + (d2.getDay() - 1);
    return days2 - days1;
}

class Now {
private:
    struct tm local_t{};
public:
    Now() {
        time_t t = time(nullptr);
        localtime_r(&t, &local_t);
    }

    std::string getDate(const char *fmt) const {
        char out[64];
        size_t result = std::strftime(out, sizeof out, fmt, &local_t);
        return std::string{out,
                           out + result};///Braced initializer avoids repeating the return type from the declaration
    }

    [[nodiscard]] std::string getDateString() const { return getDate("%F %A, %r, %Z "); }
};
//
/////
///// \param sec
//void safe_sleep(int sec) {
//    lab.iterateAlarmCounter(1);
//    struct sigaction sa{};
//    sigset_t mask{};
//    sa.sa_handler = &sigalrm_handler;
//    sa.sa_flags = SA_RESETHAND;
//    sigfillset(&sa.sa_mask);
//    sigaction(SIGALRM, &sa, nullptr);
//    sigprocmask(0, nullptr, &mask);
//    sigdelset(&mask, SIGALRM);
//    alarm(sec);
//    sigsuspend(&mask);
//
//}

///
/// \param i
static void sigint_handler(int i) {
    int x;
    x = 1;
    std::cout << "\n" << x << " by SIGINT" << std::endl;
    auto user_date = "2022-07-04";
    ++x;
    daysLeft(user_date);

    if (signal(SIGINT, sigint_handler) == SIG_ERR)
        sig_error();
    lab.iterateAlarmCounter(1);
    userIn();
}

///
/// \param num
static void sigalrm_handler(int num) {
    int x;
    auto date = "2022-12-25";
    x = lab.getAlarmCounter();
    if (num == SIGALRM) {
        printf("\n%d by SIGALRM:\n", x);
        if (x % 3 == 0 && x != 0) {
            if (signal(SIGALRM, sigalrm_handler) == SIG_ERR)
                sig_error();
            daysLeft(date);
            alarm(8);
            sleep(8);
            lab.iterateAlarmCounter(1);


        } else {
            if (signal(SIGALRM, sigalrm_handler) == SIG_ERR)
                sig_error();
            daysLeft(date);
            alarm(3);
            sleep(3);
            lab.iterateAlarmCounter(1);

        }

    }
}

///
/// \param date
static void daysLeft(const char *date) {
    Now now_date;
    struct tm tm{};

    std::string temp = now_date.getDate("%Y-%m-%d");
    auto now_storage = temp.data();

    strptime(now_storage, "%Y-%m-%d", &tm);
    time_t t = mktime(&tm);
    strptime(date, "%Y-%m-%d", &tm);
    time_t t1 = mktime(&tm);
    std::string ds1 = lab4::time_stamp();

    if (!Date::dateValidator(date)) std::cout << "Invalid date, man!" << std::endl;
    else {
        Date date1(date);
        Date date2(now_storage);
        double days = diffDays(date2, date1);
        auto sec = difftime(t1, t);
        auto hour = days * 24;
        auto minutes = hour * 60;
        auto seconds = minutes * 60;
        std::cout << "Current date and time: " << lab4::time_stamp_class_format()
                  << "\nDifference In: \n"
                  << std::fixed << std::setprecision(2) << "\tDays : "
                  << days << '\n'
                  << "\tHours: "
                  << hour << '\n'
                  << "\tMinutes: "
                  << minutes << '\n'
                  << "\tSeconds: "
                  << seconds << '\n'
                  << "\tdifftime() seconds: "
                  << sec << std::endl;

    }

}


int main() {

    unsigned cpu;
    unsigned node;
    lab4 lab4;
    lab4.setAlarmCounter(0);

    /// one care on my laptop is dying --
    ///  could not find a  glibc wrapper
    syscall(SYS_getcpu, &cpu, &node, NULL);

    printf("This program is running on CPU core %u and NUMA node %u.\n\n", cpu, node);
    try {
        loopProgram();
    } catch (const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        unknownInput();
    }

}