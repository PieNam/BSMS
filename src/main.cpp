//
//  main.cpp
//  BSMS
//
//  Created by 许滨楠 on 2019/6/28.
//  Copyright © 2019 许滨楠. All rights reserved.
//

#include <unistd.h>

#include "Global.h"
#include "SystemMsg.h"
#include "SystemOpr.h"

int main() {
    sm_welcome();
    
    mysql_init(&mysql);
    if (NULL == mysql_real_connect(&mysql, "localhost", username, password, database, 3306, 0, 0)) {
        sm_error(mysql_error(&mysql));
        return -1;
    }
    
    init();
    
    bool infinite = true;
    while (infinite) {
        sm_menu();
        string tcmd = sm_get_cmd();
        int cmd = tcmd[0] - '0';
        switch (cmd) {
            case 0:
                sm_bye();
                return 0;
                break;
            case 1:
                book();
                break;
            case 2:
                stock();
                break;
            case 3:
                purchase();
                break;
            case 4:
                sale();
                break;
            case 5:
                refund();
                break;
            case 6:
                sale_time();
                break;
            case 7:
                sale_ppl();
                break;
            default:
                sm_error("Invalid command input detected");
                break;
        }
        sleep(1);
    }
    
    mysql_close(&mysql);
    return 0;
}
