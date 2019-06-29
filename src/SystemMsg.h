//
//  SystemMsg.h
//  BSMS
//
//  Created by 许滨楠 on 2019/6/28.
//  Copyright © 2019 许滨楠. All rights reserved.
//

//    cout << "██████╗  ██████╗  ██████╗ ██╗  ██╗    ███████╗ █████╗ ██╗     ███████╗" << endl;
//    cout << "██╔══██╗██╔═══██╗██╔═══██╗██║ ██╔╝    ██╔════╝██╔══██╗██║     ██╔════╝" << endl;
//    cout << "██████╔╝██║   ██║██║   ██║█████╔╝     ███████╗███████║██║     █████╗  " << endl;
//    cout << "██╔══██╗██║   ██║██║   ██║██╔═██╗     ╚════██║██╔══██║██║     ██╔══╝  " << endl;
//    cout << "██████╔╝╚██████╔╝╚██████╔╝██║  ██╗    ███████║██║  ██║███████╗███████╗" << endl;
//    cout << "╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═╝    ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝" << endl;

#ifndef SystemMsg_h
#define SystemMsg_h

#include <mysql.h>
#include <string>
#include <vector>

using namespace std;

/*
 * display msg in the center of the interface with decorator on both sides
 */
void sm_center(char decorator, string msg) {
    int dnum = (67 - 2 - (int)msg.size()) / 2;
    for (int i = 0; i < dnum; ++i) cout << decorator;
    cout << " " << msg << " ";
    for (int i = 0; i < 65-dnum-msg.size(); ++i) cout << decorator;
    cout << endl;
}

/*
 * system alert unified format
 */
void sm_alert(const char * msg) {
    cout << "[SysAlert] " << msg << endl;
}

/*
 * error message unified format
 */
void sm_error(const char * msg) {
    sm_center('!', (string)"Errors occur during operation!");
    cout << endl;
    cout << "[ErrorMsg] " << msg << endl;
    sm_alert("Please check and try again");
    cout << endl;
}

//int sm_get_length(string k) {
//    if (k=="bno" || k=="pno" || k=="sno" || k=="rno") {
//        return 14;
//    }
//    if (k=="bamount" || k=="bsaled" || k=="bprice" || k=="boffer" || k=="pnum" || k=="snum" || k=="rnum") {
//        return 9;
//    }
//    return 20;
//}

/*
 * table line display assistant function II
 */
void sm_display_table_line(vector<int> widths, int col) {
    for (int i = 0; i < col; ++i) {
        cout << "+-";
        for (int j = 0; j <= widths[i]; ++j) {
            cout << '-';
        }
    }
    cout << "+" << endl;
}

/*
 * table display assistant function I
 */
void sm_display_table_func(vector<vector<string> > ele, vector<int> widths, int col, unsigned long long row) {
    sm_display_table_line(widths, col);
    for (int i = 0; i < col; ++i) {
        cout << "| " << setw(widths[i]) << setiosflags(ios::left) << ele[0][i] << " ";
    }
    cout << "|" << endl;
    sm_display_table_line(widths, col);
    for (int j = 1; j <= row; ++j) {
        for (int i = 0; i < col; ++i) {
            cout << "| " << setw(widths[i]) << setiosflags(ios::left) << ele[j][i] << " ";
        }
        cout << "|" << endl;
    }
    sm_display_table_line(widths, col);
}

/*
 * query result parse and function caller
 */
void sm_display_table(MYSQL_RES * res) {
    int i, j;
    vector<vector<string> > ele;
    vector<string> tele;
    vector<int> widths;
    int col = mysql_num_fields(res);
    unsigned long long row = mysql_num_rows(res);
    MYSQL_FIELD * field = mysql_fetch_fields(res);
    MYSQL_ROW rows;
    for (i = 0; i < col; ++i) {
        string t = field[i].name;
        widths.push_back((int)t.size());
        tele.push_back(t);
    }
    ele.push_back(tele);
    for (i = 1; i <= row; ++i) {
        tele.clear();
        rows = mysql_fetch_row(res);
        for (j = 0; j < col; ++j) {
            if (strlen(rows[j]) > widths[j]) widths[j] = (int)strlen(rows[j]);
            tele.push_back(rows[j]);
        }
        ele.push_back(tele);
    }
    sm_display_table_func(ele, widths, col, row);
}

/*
 * user command getter
 */
string sm_get_cmd() {
    string cmd;
    cout << "[CommandK] ";
    cin >> cmd;
    return cmd;
}

/*
 * (string) data information getter
 */
string sm_get_info(const char * msg, const char * key) {
    string str;
    sm_alert(msg);
    cout << "[" << key << "] ";
    cin >> str;
    return str;
}

/*
 * (int) data information getter
 */
int sm_get_amount(const char * msg, const char * key) {
    int amount;
    sm_alert(msg);
    cout << "[" << key << "] ";
    cin >> amount;
    return amount;
}

/*
 * welcome slogan printer
 */
void sm_welcome() {
    cout << "╔╗  ╔═╗ ╔═╗ ╦╔═   ╔═╗ ╔═╗ ╦   ╔═╗ ╔═╗   ╔╦╗ ╔═╗ ╔╗╔ ╔═╗ ╔═╗ ╔═╗ ╦═╗" << endl;
    cout << "╠╩╗ ║ ║ ║ ║ ╠╩╗   ╚═╗ ╠═╣ ║   ║╣  ╚═╗   ║║║ ╠═╣ ║║║ ╠═╣ ║ ╦ ║╣  ╠╦╝" << endl;
    cout << "╚═╝ ╚═╝ ╚═╝ ╩ ╩   ╚═╝ ╩ ╩ ╩═╝ ╚═╝ ╚═╝   ╩ ╩ ╩ ╩ ╝╚╝ ╩ ╩ ╚═╝ ╚═╝ ╩╚═" << endl;
    cout << endl;
    sm_center('~', (string)"Welcome to the Book Sales Management System");
}

/*
 * system memu printer
 */
void sm_menu() {
    cout << endl << endl;
    sm_center('-', "Please enter the command related to the operations in [?]");
    cout << endl;
    cout << "[Opr\'Menu] " << endl << endl;
    cout << "\t [0] -- Exit." << endl;
    cout << "\t [1] -- Show the book(s) in the store." << endl;
    cout << "\t [2] -- Show the stocking list of the store." << endl;
    cout << "\t [3] -- Purchase book(s) from the supplier(s)." << endl;
    cout << "\t [4] -- Sale book(s) to a customer." << endl;
    cout << "\t [5] -- Refund to a customer with book(s)." << endl;
    cout << "\t [6] -- Show the saling list of the store in time order." << endl;
    cout << "\t [7] -- Show the saling list of the store in popularity order." << endl;
    cout << endl;
}

/*
 * goodbye slogan printer
 */
void sm_bye() {
    cout << endl << endl;
    cout << "╔╗  ╦ ╦ ╔═╗   ╔╗  ╦ ╦ ╔═╗" << endl;
    cout << "╠╩╗ ╚╦╝ ║╣    ╠╩╗ ╚╦╝ ║╣ " << endl;
    cout << "╚═╝  ╩  ╚═╝   ╚═╝  ╩  ╚═╝" << endl;
    cout << endl << endl << endl;
}

#endif /* SystemMsg_h */
