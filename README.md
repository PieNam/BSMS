# 数据库课程设计报告

## 开发环境与开发工具

> 开发语言：**C++**、**MySQL**
>
> 操作系统：**macOS Mojave Version 10.14.5**
>
> 开发工具：**Xcode IDE**、**iTerm 命令行终端**
>
> 终端环境：**iTerm + zsh**、**Xcode 终端调试台**
>
> MySQL Server 版本：**5.7.25 MySQL Community Server (GPL)**
>
>  
>
> 运行方式：`cd` 到目录下 `make` 可完成编译连接，而后直接 `./bsms` 即可运行



## 系统需求分析

&emsp;&emsp;当今的图书销售行业有数据量大、数据结构复杂、数据变化和流动频繁的特点，加上如今的网商平台越来越发达，各行各业的电子化信息化管理的趋势非常明显，图书销售的管理将是一个非常具有实用性的工具系统。有了这样的系统，我们可以通过事务命令的封装、提供比较友好的用户界面，使管理人员便于使用和操作，便捷地实现以往繁琐的图书销售管理操作。

&emsp;&emsp;设计实现中要完成的数据库管理系统，要实现几个功能模块，方便管理人员对图书的销售进行比较全面的管理。具体来说大致具有以下的功能：

- **基本管理**
	- 打印图书完整库存信息，包括编号、名称、作者、库存量、销售量、单价、供应商、供应商单价；
- **进货**
	- 可以显示当前所有图书的库存量、销售情况、供应商单价情况等基本信息；
	- 支持管理员通过信息录入对具体图书进行进货操作，自动更新数据库，打印进货单；
- **退货**
	- 支持管理员通过信息录入对具体图书进行退货操作，自动更新数据库，打印退货单；
- **统计**
	- 可以根据销售情况输出统计报表，具体内容有每月销售总额、总量和排行榜；
- **销售**
	- 支持管理用通过信息录入输入用户要购买书籍的信息，自动更新数据库，打印购买情况；



### 系统功能模块图

> **见 repo.pdf**

## 系统设计

### 数据概念结构设计

> 根据需求分析和系统设计，管理系统的系统 ER 图**见 repo.pdf**



### 数据库关系模式设计

> **<u>加粗下划线</u>** 的属性为主码，***加粗斜体*** 的属性为外码

- **图书库存**（ <u>**编号**</u>，名称，作者，库存量，销售量，单价，供应商，供应商单价 ）

	**books** ( <u>**bno**</u>, bname, bauthor, bamount, bsaled, bprice, bsupplier, boffer )

- **图书进货**（ <u>**订单号**</u>，编号，数量，供应商，供应商单价 ）

	**purchase** ( <u>**pno**</u>, ***bno***, pnum, bsupplier, boffer )

- **图书销售** （ <u>**销售单号**</u>，编号，数量 ）

	**sale** ( <u>**sno**</u>, ***bno***, snum)

- **图书退货** （ <u>**退货单号**</u>，编号，数量）

	**refund** ( <u>**rno**</u>, ***bno***, rnum)



### 数据库物理结构设计

> 该系统的数据库物理结构设计通过创建表的各个操作的 SQL 语句命令呈现。

#### 数据库创建

```mysql
create database bsms character set=utf8; /* bsm for book sales management */
use bsms;
```

#### 表的创建

##### 创建图书库存表

```mysql
create table books (
bno varchar(16) NOT NULL,
bname varchar(64) NOT NULL,
bauthor varchar(64),
bamount int NOT NULL,
bsaled int NOT NULL,
bprice int NOT NULL,
bsupplier varchar(64),
boffer int,
primary key(bno),
index (bno)
) engine=innodb default charset=utf8;

/* test insert */
insert into books(bno,bname,bauthor,bamount,bsaled,bprice,bsupplier,boffer)
value("testnum","testname","testauth",99,9,9999,"testpbls",1);
```

##### 创建图书进货表

```mysql
create table purchase (
pno varchar(16) NOT NULL,
bno varchar(16) NOT NULL,
pnum int NOT NULL,
bsupplier varchar(64),
boffer int,
primary key(pno),
foreign key(bno) references books(bno)
) engine=innodb default charset=utf8;

/* test insert */
insert into purchase(pno,bno,pnum,bsupplier,boffer)
value("testpurchase","testnum",123,"testsupplier",99);
```

##### 创建销售表

```mysql
create table sale (
sno varchar(16) NOT NULL,
bno varchar(16) NOT NULL,
snum int NOT NULL,
primary key(sno),
foreign key(bno) references books(bno)
) engine=innodb default charset=utf8;

/* test insert */
insert into sale(sno,bno,snum)
value("testsale","testnum",2);
```

##### 创建退货表

```mysql
create table refund (
rno varchar(16) NOT NULL,
bno varchar(16) NOT NULL,
rnum int NOT NULL,
primary key(rno),
foreign key(bno) references books(bno)
)  engine=innodb default charset=utf8;

/* test insert */
insert into refund(rno,bno,rnum)
value("testrefund","testnum",1);
```

#### 完整性约束触发器

##### 进货操作之后的相关更新

```mysql
/* delimiter // */
/* don't have to and can't user delimiter in C API */
create trigger purchase_update
after insert on purchase
for each row
    begin
        update books set bamount=bamount+new.pnum where bno=new.bno;
        update books set bsupplier=new.bsupplier where bno=new.bno;
        update books set boffer=new.boffer where bno=new.bno;
    end
/* //
delimiter ; */
```

##### 销售操作之后的相关更新

```mysql
create trigger sale_update
after insert on sale
for each row
    begin
        update books set bamount=bamount-new.snum where bno=new.bno;
        update books set bsaled=bsaled+new.snum where bno=new.bno;
    end
```

##### 退货操作之后的相关更新

```mysql
create trigger refund_update
after insert on refund
for each row
    begin
        update books set bamount=bamount+new.rnum where bno=new.bno;
        update books set bsaled=bsaled-new.rnum where bno=new.bno;
        update sale set snum=snum-new.rnum where bno=new.bno;
    end
```

#### 表格查询与连接

##### 显示所有库存图书

```mysql
select *
from books
order by bno asc;
```

##### 显示所有进货信息

```mysql
select p.pno,b.bno,b.bname,p.pnum,b.bsupplier,b.boffer
from purchase p
join books b
    on b.bno=p.bno
order by pno desc;
```

##### 按照时间降序显示销售单详细信息

```mysql
select s.sno,s.snum,b.bno,b.bname,b.bauthor,b.bamount,b.bsaled,b.bprice
from books b
join sale s
    on s.bno=b.bno
order by sno desc;
```

##### 按照销售量降序显示图书销售情况

```mysql
select b.bno,b.bname,b.bauthor,b.bamount,b.bsaled,b.bprice
from books b
order by bsaled desc;
```



## 系统功能的实现

> 对系统运行的大致流程和其相关功能的代码实现进行分析。
>
> 源码中按照函数注释格式给出了大致的英文注释，这里为了方便分析和补充说明重新添加注释。

### 主要功能模块的实现过程简述

#### 主要框架

先看到整个系统的主体框架：

```c++
int main() {
    sm_welcome(); // 欢迎页面打印 系统起始界面
    // 数据库连接 运行时需要先行手动在MySQL上创建数据库 bsms
    mysql_init(&mysql);
    if (NULL == mysql_real_connect(&mysql, "localhost", username, password, database, 3306, 0, 0)) {
        sm_error(mysql_error(&mysql));
        return -1;
    }
    // 数据库初始化：创建表格、触发器约束
    init();
    // 循环获取用户操作，各对应操作用函数进行封装处理
    bool infinite = true;
    while (infinite) {
        sm_menu(); // 菜单打印
        string tcmd = sm_get_cmd(); // 指令获取和处理
        int cmd = tcmd[0] - '0';
        switch (cmd) {
            case 0:sm_bye();return 0;break;
            case 1:book();break;
            case 2:stock();break;
            case 3:purchase();break;
            case 4:sale()；break;
            case 5:refund();break;
            case 6:sale_time();break;
            case 7:sale_ppl();break;
            default:sm_error("Invalid command input detected");break;
        }
        sleep(1); // 每次都挂起一秒
                  // 提示用户注意操作后的系统信息以免直接被菜单覆盖
                  // 防止生成的年月日时分秒时间戳重复
    }
    mysql_close(&mysql);
    return 0;
}
```

主体的框架非常简单清晰，连接数据库成功后不断阻塞式地获取用户指令进行处理，利用 switch case 代码段进行功能划分和函数调用，接下来看到一些功能性函数库的基本信息，以便展示系统底层函数功能和实现支持。

#### 功能库

##### Global.h

> 该头文件保存了全局性的可修改的变量和宏定义，方便统一管理、使用和修改。

```c++
#ifndef Global_h
#define Global_h

#include <iomanip>
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

MYSQL mysql;
char username[16] = "root";
char password[16] = "database2019";
char database[16] = "bsms";

#endif /* Global_h */
```

##### SystemMsg.h

> 该头文件申明定义了一些系统命令行用户界面的输出显示设置和各个层次都可以使用的一些显示类的功能性函数。
>
> 下面使用函数申明列表进行解释。

```c++
// 系统界面打印辅助函数，以 decorator 作为左右占位符号将 msg 内容在屏幕上居中输出
void sm_center(char decorator, string msg)；

// 系统消息提示统一模板，将提示信息 msg 在命令行界面以统一格式输出，用于输出正常操作后的提示信息
void sm_alert(const char * msg)；

// 系统消息提示统一模板，将错误信息 msg 在命令行界面以统一格式输出，用于输出异常操作后的错误提示
void sm_error(const char * msg)；

// 系统表格打印辅助函数，模仿 MySQL 命令行终端的表格打印方法打印表格中的横向分隔符
void sm_display_table_line(vector<int> widths, int col)；

// 系统表格打印辅助函数，打印整个表格信息
void sm_display_table_func(vector<vector<string>> ele, vector<int> widths, int col, unsigned long long row)；

// 系统表格打印函数，将 mysql 查询得到的结果进行解析保存，调用辅助函数完成打印
// 获取每个数据属性信息并保存在二维vector中，获取每列数据最大长度并保存作为底层表格打印函数参数
void sm_display_table(MYSQL_RES * res)；

// 系统获取用户指令统一模板，显示获取指令提示信息并接收用户输入
string sm_get_cmd()；

// 系统获取字符串类型数据信息统一模板，显示提示输入 msg 和属性关键字 key 并接受用户录入的数据
string sm_get_info(const char * msg, const char * key)；

// 系统获取整数类型数据信息统一模板，显示提示输入 msg 和属性关键字 key 并接受用户录入的数据
int sm_get_amount(const char * msg, const char * key)；

// 系统欢迎界面打印函数，会输出 ascii 字符拼接海报体的 BOOK SALE 字样
void sm_welcome()；

// 系统菜单打印函数，会输出系统功能列表以及对应的代表关键字方便用户操作
void sm_menu()；

// 系统关闭退出信息打印函数，模仿 MySQL 命令行终端输出海报体 Bye
void sm_bye()；
```

##### MySQLOpr.h

> 该头文件将插入数据的操作进行了封装，提供插入数据 API 接受相关数据变量完成数据插入。

```c++
// 以其中一个函数为例，初始化查询字符串并将接收到的参数利用 sprintf 整合其中
// 进行查询并完成错误处理和结果返回，更详细有针对性的错误信息会在上层调用中进行打印
bool insert_book(string bno, string bname, string bauthor, int bamount, int bsaled, int bprice, string bsupplier, int boffer) {
    char querys[256];
    sprintf(querys, "insert into books(bno,\
                                       bname,\
                                       bauthor,\
                                       bamount,\
                                       bsaled,\
                                       bprice,\
                                       bsupplier,\
                                       boffer) value\
                                       (\"%s\",\"%s\",\"%s\",%d,%d,%d,\"%s\",%d);\
                                       ", bno.c_str(), bname.c_str(), 
                                       bauthor.c_str(), bamount, bsaled,
                                       bprice, bsupplier.c_str(), boffer);
    if (0 == mysql_query(&mysql, querys)) {
        return true;
    }
    sm_error(mysql_error(&mysql));
    return false;
}

bool insert_purchase(string pno, string bno, int pnum, string bsupplier, int boffer);

bool insert_sale(string sno, string bno, int snum);

bool insert_refund(string rno, string bno, int rnum);
```

#### 系统功能库

##### SystemOpr.h

```c++
// 根据前述数据库物理结构设计中的表格、触发器创建语句完成表格和触发器创建
void init();

// 获取年月日时分秒14位时间戳作为唯一的订单、进货单、退货单标识
// 因为每次功能执行都会挂起一秒，所以不会出现重复情况
string get_time_stamp()；

// 打印图书列表，查询所有图书，按照编号升序排列并调用功能函数完成打印
void book()；

// 进货列表打印，查询进货列表并对图书列表进行join拼接，打印详细进货信息
void stock() ；

// 完成进货操作
// 要求用户输入要进货的书编号、数量、选择供应商和对应报价
// 如果书不存在，即说明是新书进货，要求用户录入完整书籍信息完成插入再进行下一步
// 如果书已经存在，或者已经完成插入，则按照编号和数量对书库、进货单两个表格进行更新
void purchase()；

// 完成销售操作
// 要求用户输入要售卖的书编号、数量
// 判断书库中是否有该书，如果有数量是否足够
// 根据实际情况完成正常销售或是打印错误信息建议管理员进货
void sale()；

// 完成退货操作
// 要求用户输入客户要退货的书编号、数量
// 判断该书是否在书库中，如果有是否售卖出该数量以上的书
// 根据实际情况完成退货或是打印错误信息提醒该书可能不是本店售出
void refund()；


// 根据订单完成时间（即订单号）从新到旧打印销售单
void sale_time()；

// 根据图书热度、受欢迎程度（总销售量）从高到低打印图书销售情况
void sale_ppl()；
```



### 运行界面

> 下面演示正常操作下的用户操作流程，因为错误处理的运行分支过多，这里不进行详细演示。实际系统中已对各个步骤可能产生的错误进行妥善处理和用户提示。

**见 repo.pdf**



## 总结

### 本课程设计中用到的《数据库系统》理论课概念与知识

- 使用基本 SQL 语句和命令创建数据库和数据库中的表格；
- 运用了索引，在创建最主要的 books 书库表格的时候添加了索引 bno；
- 使用应用比较广泛的 UTF8 编码字符集创建数据库和表，保证了显示的兼容性；
- 设计物理结构创建表格时运用基本数据库知识添加完整性约束，定义了表格主码、外码；
- 设计物理结构的时候设计创建了数据库中的触发器 trigger，对相关的表格进行联动更改；
- 运用 procedure 和 transaction rollback 的思想，对创建数据库时的基本数据合法性进行了检查，如果不合法将进行事务回滚，取消创建操作；