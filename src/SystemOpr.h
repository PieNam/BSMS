//
//  SystemOpr.h
//  BSMS
//
//  Created by 许滨楠 on 2019/6/28.
//  Copyright © 2019 许滨楠. All rights reserved.
//

#ifndef SystemOpr_h
#define SystemOpr_h

#include <time.h>

#include "Global.h"
#include "MySQLOpr.h"

/*
 * init the database: create the tables we need and add triggers
 */
void init() {
    mysql_query(&mysql,
                "create table books (\
                 bno varchar(16) NOT NULL,\
                 bname varchar(64) NOT NULL,\
                 bauthor varchar(64),\
                 bamount int NOT NULL,\
                 bsaled int NOT NULL,\
                 bprice int NOT NULL,\
                 bsupplier varchar(64),\
                 boffer int,\
                 primary key(bno),\
                 index (bno)\
                ) engine=innodb default charset=utf8;");
    mysql_query(&mysql,
                "create table purchase (\
                 pno varchar(16) NOT NULL,\
                 bno varchar(16) NOT NULL,\
                 pnum int NOT NULL,\
                 bsupplier varchar(64),\
                 boffer int,\
                 primary key(pno),\
                 foreign key(bno) references books(bno)\
                 ) engine=innodb default charset=utf8;");
    mysql_query(&mysql,
                "create table sale (\
                 sno varchar(16) NOT NULL,\
                 bno varchar(16) NOT NULL,\
                 snum int NOT NULL,\
                 primary key(sno),\
                 foreign key(bno) references books(bno)\
                 ) engine=innodb default charset=utf8;");
    mysql_query(&mysql,
                "create table refund (\
                 rno varchar(16) NOT NULL,\
                 bno varchar(16) NOT NULL,\
                 rnum int NOT NULL,\
                 primary key(rno),\
                 foreign key(bno) references books(bno)\
                 )  engine=innodb default charset=utf8;");
    
    
    mysql_query(&mysql,
                "create trigger purchase_update\
                 after insert on purchase\
                 for each row\
                 begin\
                 update books set bamount=bamount+new.pnum where bno=new.bno;\
                 update books set bsupplier=new.bsupplier where bno=new.bno;\
                 update books set boffer=new.boffer where bno=new.bno;\
                 end");
    mysql_query(&mysql,
                "create trigger sale_update\
                 after insert on sale\
                 for each row\
                 begin\
                 update books set bamount=bamount-new.snum where bno=new.bno;\
                 update books set bsaled=bsaled+new.snum where bno=new.bno;\
                 end");
    mysql_query(&mysql,
                "create trigger refund_update\
                 after insert on refund\
                 for each row\
                 begin\
                 update books set bamount=bamount+new.rnum where bno=new.bno;\
                 update books set bsaled=bsaled-new.rnum where bno=new.bno;\
                 update sale set snum=snum-new.rnum where bno=new.bno;\
                 end");
}

/*
 * get time stamp for number of the lists
 */
string get_time_stamp() {
    time_t tt = time(NULL);
    struct tm *stm = localtime(&tt);
    char tmp[32];
    sprintf(tmp, "%04d%02d%02d%02d%02d%02d", 1900+stm->tm_year, 1+stm->tm_mon, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
    return tmp;
}

/*
 * display book
 */
void book() {
    mysql_query(&mysql, "select * from books order by bno asc;");
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        sm_alert("It's an empty set.");
    } else {
        sm_display_table(res);
    }
}

/*
 * display stock record
 */
void stock() {
    mysql_query(&mysql, "select p.pno,b.bno,b.bname,p.pnum,b.bsupplier,b.boffer from purchase p join books b on b.bno=p.bno order by pno desc;");
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        sm_alert("It's an empty set.");
    } else {
        sm_display_table(res);
    }
}

/*
 * purchase book from supplier
 */
void purchase() {
    string pno, bno, bname, bauthor, bsupplier;
    int pnum, boffer, bamount, bsaled, bprice;
    pno = get_time_stamp();
    bno = sm_get_info("Please input the NO. of the Book", "bno     ");
    pnum = sm_get_amount("Please input the Number of the Book to Purchase", "pnum    ");
    bsupplier = sm_get_info("Please input the Supplier of the Book", "supplier");
    boffer = sm_get_amount("Please input the Price of the Book from the Supplier", "boffer  ");
    // check if the book existed in the store currently
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        // there is no such book among the current books
        // require to get more information
        sm_alert("It's a new book in the store, \n\t\t\tplease complete the following information");
        bname = sm_get_info("Please input the Name of the Book", "pname   ");
        bauthor = sm_get_info("Please input the Author of the Book", "bauthor ");
        bprice = sm_get_amount("Please input the Price of one copy of this Book", "bprice  ");
        bamount = bsaled = 0;
        
        if (insert_book(bno, bname, bauthor, bamount, bsaled, bprice, bsupplier, boffer)) {
            sm_alert("Book Inserted Successfully!");
        } else {
            sm_alert("Book Inserted Failed!");
        }
    }
    // there is already some copies of this book
    // we are a returned customer
    // or we've inserted the new book to the store
    if (insert_purchase(pno, bno, pnum, bsupplier, boffer)) {
        sm_alert("Book Purchased Successfully!");
        char msg[80];
        sprintf(msg, "Purchase %d copies of %s from %s at the cost of $%d.", pnum, bname.c_str(), bsupplier.c_str(), pnum*boffer);
        sm_alert(msg);
    } else {
        sm_alert("Book Purchased Failed!");
    }
    mysql_free_result(res);
}

/*
 * sale book to customer
 */
void sale() {
    string sno, bno;
    int snum;
    sno = get_time_stamp();
    bno = sm_get_info("Please input the NO. of the Book", "bno     ");
    snum = sm_get_amount("Please input the Number of the Book to Sale", "snum    ");
    // check if the book existed in the store currently
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        // there is no such book among the current books
        // refuse to sale
        sm_error("There isn't such book in the store!");
        sm_alert("Please purchase some if needed with the command [3]");
    } else {
        // there is this book in the store
        // judge the amount
        unsigned int field_num = mysql_num_fields(res);
        MYSQL_FIELD * fields = mysql_fetch_fields(res);
        MYSQL_ROW row = mysql_fetch_row(res);
        int bamount = 0;
        for (int i = 0; i < field_num; ++i) {
            if (0 == strcmp(fields[i].name, "bamount")) {
                bamount = atoi(row[i]);
                break;
            }
        }
        if (bamount >= snum) {
            // there are enough book(s) to sale
            if (insert_sale(sno, bno, snum)) {
                sm_alert("Book Saled Successfully");
                char msg[80];
                sprintf(msg, "Sold %d copies of %s.", snum, bno.c_str());
                sm_alert(msg);
            } else {
                sm_alert("Book Saled Failed");
            }
        } else {
            // there aren't enough book(s) to sale
            sm_error("There aren't enough book(s) to sale!");
            sm_alert("Please purchase some extra books if needed with the command [3]");
        }
    }
    mysql_free_result(res);
}

/*
 * deal with the refund requirement from a customer
 */
void refund() {
    string rno, bno;
    int rnum;
    rno = get_time_stamp();
    bno = sm_get_info("Please input the NO. of the Book", "bno     ");
    rnum = sm_get_amount("Please input the Number of the Book to Refund", "rnum    ");
    
    // check if the book existed in the store currently
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    
    // check if we've saled enough book to refund
    string if_enough = "select * from sale where bno=\"";
    if_enough += bno; if_enough += "\"";
    mysql_query(&mysql, if_enough.c_str());
    res = mysql_store_result(&mysql);
    unsigned long long res_num_sale = mysql_num_rows(res);
    int snum = 0;
    if (res_num_sale != 0) {
        unsigned int field_num = mysql_num_fields(res);
        MYSQL_FIELD * fields = mysql_fetch_fields(res);
        MYSQL_ROW row = mysql_fetch_row(res);
        for (int i = 0; i < field_num; ++i) {
            if (0 == strcmp(fields[i].name, "snum")) {
                snum = atoi(row[i]);
                break;
            }
        }
    }
    if (0 == res_num) {
        // there is no such book among the current books
        // refuse to sale
        sm_error("There isn't such book in the store!");
        sm_alert("Maybe the book is not saled in this store");
    } else if (snum < rnum) {
        // we've not saled this much and can't make a refund
        sm_error("There aren't so many books saled in the store!");
        sm_alert("Maybe the books are not saled in this store");
    } else {
        // there is this book in the store
        // judge the amount
        if (insert_refund(rno, bno, rnum)) {
            sm_alert("Book Refund Successfully!");
            char msg[80];
            sprintf(msg, "Refund %d copies of %s.", rnum, bno.c_str());
            sm_alert(msg);
        } else {
            sm_alert("Book Refund Failed!");
        }
    }
    mysql_free_result(res);
}


/*
 * display the sale list in time order
 */
void sale_time() {
    mysql_query(&mysql, "select s.sno,s.snum,b.bno,b.bname,b.bauthor,b.bamount,b.bsaled,b.bprice from books b join sale s on s.bno=b.bno order by sno desc;");
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        sm_alert("It's an empty set.");
    } else {
        sm_display_table(res);
    }
}

/*
 * display the sale list in popularity order
 */
void sale_ppl() {
    mysql_query(&mysql, "select b.bno,b.bname,b.bauthor,b.bamount,b.bsaled,b.bprice from books b order by bsaled desc;");
    MYSQL_RES * res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);
    if (0 == res_num) {
        sm_alert("It's an empty set.");
    } else {
        sm_display_table(res);
    }
}

#endif /* SystemOpr_h */
