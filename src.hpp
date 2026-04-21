#include <iostream>
#include <algorithm>
#include "base.hpp"
using namespace std;

struct date{int year,month,day;date()=default;date(int y,int m,int d):year(y),month(m),day(d){};friend istream& operator>>(istream& is,date& dt){return is>>dt.year>>dt.month>>dt.day;}bool operator<(const date& o)const{if(year!=o.year) return year<o.year; if(month!=o.month) return month<o.month; return day<o.day;}bool operator==(const date& o)const{return year==o.year&&month==o.month&&day==o.day;}};

class mail: public object{protected:string postmark; date send_date, arrive_date;public: mail()=default; mail(string c,string p,date s,date a):object(c){postmark=p;send_date=s;arrive_date=a;} virtual ~mail()=default; virtual string send_status(int,int,int) override {return string(not