// Implementation for Problem 055 - ACM EXPRESS
#include <iostream>
#include <algorithm>
#include "base.hpp"
using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
    friend istream &operator>>(istream &is, date &dt) {
        return is >> dt.year >> dt.month >> dt.day;
    }
    bool operator<(const date &o) const {
        if (year != o.year) return year < o.year;
        if (month != o.month) return month < o.month;
        return day < o.day;
    }
    bool operator==(const date &o) const {
        return year == o.year && month == o.month && day == o.day;
    }
};

static inline long to_days(const date &d) {
    return static_cast<long>(d.year) * 360L + static_cast<long>(d.month - 1) * 30L + static_cast<long>(d.day - 1);
}

class mail : public object {
protected:
    string postmark;
    date send_date{};
    date arrive_date{};

public:
    mail() = default;
    mail(string c, string p, date s, date a) : object(c) {
        postmark = std::move(p);
        send_date = s;
        arrive_date = a;
    }
    ~mail() override = default;

    string send_status(int, int, int) override {
        return "not send";
    }
    string type() override { return "no type"; }
    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << '\n';
    }
    void copy(object *o) override {
        auto *m = reinterpret_cast<mail *>(o);
        contain = m->contain;
        postmark = m->postmark;
        send_date = m->send_date;
        arrive_date = m->arrive_date;
    }
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date{};
    date land_date{};

public:
    air_mail() = default;
    air_mail(string c, string p, date s, date a, date t, date l, string al)
        : mail(std::move(c), std::move(p), s, a) {
        take_off_date = t;
        land_date = l;
        airlines = std::move(al);
    }
    ~air_mail() override = default;

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        else if (q < take_off_date) return "wait in airport";
        else if (q < land_date) return "in flight";
        else if (q < arrive_date) return "already land";
        else return "already arrive";
    }
    string type() override { return "air"; }
    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << '\n';
    }
    void copy(object *o) override {
        auto *a = reinterpret_cast<air_mail *>(o);
        mail::copy(o);
        airlines = a->airlines;
        take_off_date = a->take_off_date;
        land_date = a->land_date;
    }
};

class train_mail : public mail {
protected:
    string *station_name = nullptr;
    date *station_time = nullptr;
    int len = 0;

public:
    train_mail() = default;
    train_mail(string c, string p, date s, date a, string *sname, date *stime, int station_num)
        : mail(std::move(c), std::move(p), s, a) {
        len = station_num;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        }
    }
    ~train_mail() override {
        delete[] station_name;
        delete[] station_time;
    }

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        // Exact arrival at a station
        for (int i = 0; i < len; ++i) {
            if (q == station_time[i]) return string("arrive ") + station_name[i];
        }
        if (len == 0) {
            return (q < arrive_date) ? string("depart ") + string("") : string("already arrive");
        }
        if (q < station_time[0]) return string("arrive ") + station_name[0];
        for (int i = 0; i < len - 1; ++i) {
            if (station_time[i] < q && q < station_time[i + 1]) return string("depart ") + station_name[i];
        }
        if (q < arrive_date) return string("depart ") + station_name[len - 1];
        return "already arrive";
    }
    string type() override { return "train"; }
    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << '\n';
    }
    void copy(object *o) override {
        auto *t = reinterpret_cast<train_mail *>(o);
        mail::copy(o);
        delete[] station_name;
        delete[] station_time;
        len = t->len;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = t->station_name[i];
                station_time[i] = t->station_time[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }
};

class car_mail : public mail {
protected:
    int total_mile = 0;
    string driver;

public:
    car_mail() = default;
    car_mail(string c, string p, date s, date a, int mile, string d)
        : mail(std::move(c), std::move(p), s, a) {
        total_mile = mile;
        driver = std::move(d);
    }
    ~car_mail() override = default;

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        if (q < arrive_date) {
            double elapsed = static_cast<double>(to_days(q) - to_days(send_date));
            double total = static_cast<double>(to_days(arrive_date) - to_days(send_date));
            double current_mile = (total > 0.0) ? (elapsed / total) * static_cast<double>(total_mile) : 0.0;
            return to_string(current_mile);
        }
        return "already arrive";
    }
    string type() override { return "car"; }
    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << '\n';
    }
    void copy(object *o) override {
        auto *c = reinterpret_cast<car_mail *>(o);
        mail::copy(o);
        total_mile = c->total_mile;
        driver = c->driver;
    }
};

void obj_swap(object *&x, object *&y) {
    object *t = x;
    x = y;
    y = t;
}
