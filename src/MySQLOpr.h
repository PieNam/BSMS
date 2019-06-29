//
//  MySQLOpr.h
//  BSMS
//
//  Created by 许滨楠 on 2019/6/29.
//  Copyright © 2019 许滨楠. All rights reserved.
//

#ifndef MySQLOpr_h
#define MySQLOpr_h

#include "Global.h"
#include "SystemMsg.h"

bool insert_book(string bno, string bname, string bauthor, int bamount, int bsaled, int bprice, string bsupplier, int boffer) {
    char querys[256];
    sprintf(querys, "insert into books(bno,bname,bauthor,bamount,bsaled,bprice,bsupplier,boffer) value (\"%s\",\"%s\",\"%s\",%d,%d,%d,\"%s\",%d);", bno.c_str(), bname.c_str(), bauthor.c_str(), bamount, bsaled, bprice, bsupplier.c_str(), boffer);
    if (0 == mysql_query(&mysql, querys)) {
        return true;
    }
    sm_error(mysql_error(&mysql));
    return false;
}

bool insert_purchase(string pno, string bno, int pnum, string bsupplier, int boffer) {
    char querys[256];
    sprintf(querys, "insert into purchase(pno,bno,pnum,bsupplier,boffer) value (\"%s\",\"%s\",%d,\"%s\",%d);", pno.c_str(), bno.c_str(), pnum, bsupplier.c_str(), boffer);
    if (0 == mysql_query(&mysql, querys)) {
        return true;
    }
    sm_error(mysql_error(&mysql));
    return false;
}

bool insert_sale(string sno, string bno, int snum) {
    char querys[256];
    sprintf(querys, "insert into sale(sno,bno,snum) value (\"%s\",\"%s\",%d);", sno.c_str(), bno.c_str(), snum);
    if (0 == mysql_query(&mysql, querys)) {
        return true;
    }
    sm_error(mysql_error(&mysql));
    return false;
}

bool insert_refund(string rno, string bno, int rnum) {
    char querys[256];
    sprintf(querys, "insert into refund(rno,bno,rnum) value (\"%s\",\"%s\",%d);", rno.c_str(), bno.c_str(), rnum);
    if (0 == mysql_query(&mysql, querys)) {
        return true;
    }
    sm_error(mysql_error(&mysql));
    return false;
}

#endif /* MySQLOpr_h */
