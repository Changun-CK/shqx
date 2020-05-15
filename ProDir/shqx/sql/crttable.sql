/*==============================================================*/
/* DBMS name:      ORACLE Version 11g                           */
/* Created on:     2020/4/8 21:17:01                            */
/*==============================================================*/


alter table T_SURFDATA
   drop constraint FK_SURFDA_OBTCOD;

drop index IDX_ACIDOBT_4;

drop index IDX_ACIDOBT_3;

drop index IDX_ACIDOBT_2;

drop index IDX_ACIDOBT_1;

drop table T_ACIDOBT cascade constraints;

drop table T_AREACODE cascade constraints;

drop index IDX_AUTOSTVIS_5;

drop index IDX_AUTOSTVIS_4;

drop index IDX_AUTOSTVIS_3;

drop index IDX_AUTOSTVIS_2;

drop index IDX_AUTOSTVIS_1;

drop table T_AUTOSTVIS cascade constraints;

drop index IDX_CLOUDOBT_4;

drop index IDX_CLOUDOBT_3;

drop index IDX_CLOUDOBT_2;

drop index IDX_CLOUDOBT_1;

drop table T_CLOUDOBT cascade constraints;

drop index IDX_COUNTRYSIGNAL_5;

drop index IDX_COUNTRYSIGNAL_4;

drop index IDX_COUNTRYSIGNAL_3;

drop index IDX_COUNTRYSIGNAL_2;

drop index IDX_COUNTRYSIGNAL_1;

drop table T_COUNTRYSIGNAL cascade constraints;

drop table T_OBTCODE cascade constraints;

drop index IDX_REFINEDATA_4;

drop index IDX_REFINEDATA_3;

drop index IDX_REFINEDATA_2;

drop index IDX_REFINEDATA_1;

drop table T_REFINEDATA cascade constraints;

drop table T_SIGNALLOG cascade constraints;

drop index IDX_SURFDATA_3;

drop index IDX_SURFDATA_2;

drop index IDX_SURFDATA_1;

drop table T_SURFDATA cascade constraints;

drop table T_SURFZFILES cascade constraints;

drop table T_TYPHOON_BEST cascade constraints;

/*==============================================================*/
/* Table: T_ACIDOBT                                             */
/*==============================================================*/
drop sequence SEQ_ACIDOBT;
create sequence SEQ_ACIDOBT increment by 1 start with 1;
create table T_ACIDOBT 
(
   acidobt              char(5)              not null,
   lat                  char(7),
   lon                  char(8),
   height               char(6),
   obmethod             char(1),
   ddatetime            date                 not null,
   tbegin               date,
   tend                 date,
   prept                number(4),
   t                    number(3),
   PH1                  number(4),
   PH2                  number(4),
   PH3                  number(4),
   PHarv                number(4),
   K1                   number(5),
   K2                   number(5),
   K3                   number(5),
   Karv                 number(5),
   ret                  number(3),
   rePH1                number(4),
   rePH2                number(4),
   rePH3                number(4),
   rePHarv              number(4),
   reK1                 number(5),
   reK2                 number(5),
   reK3                 number(5),
   reKarv               number(5),
   wddir14              char(3),
   wdsp14               number(3),
   wddir20              char(3),
   wdsp20               number(3),
   wddir02              char(3),
   wdsp02               number(3),
   wddir08              char(3),
   wdsp08               number(3),
   wtph1                char(2),
   wtph2                char(2),
   wtph3                char(2),
   wtph4                char(2),
   acidnote             char(5),
   crttime              date                 default sysdate not null,
   keyid                number               not null,
   constraint PK_ACIDOBT primary key (acidobt, ddatetime)
)
tablespace USERS;

comment on table T_ACIDOBT is
'酸雨观测数据。';

comment on column T_ACIDOBT.acidobt is
'酸雨观测站代号，由5位数字组成，前2位为区号，后3位为站号。';

comment on column T_ACIDOBT.lat is
'纬度，最后一位S、N分别表示南、北纬。';

comment on column T_ACIDOBT.lon is
'经度，最后一位E、W分别表示东、西经。';

comment on column T_ACIDOBT.height is
'观测场拔海高度，由6位数字组成，第1位为拔海高度参数，实测为“0”，约测为“1”。后5位为拔海高度，单位为“0.1m”，位数不足，高位补“0”。若测站位于海平面以下，第2位录入“-”号。';

comment on column T_ACIDOBT.obmethod is
'观测方式，由1位数组成。人工采样，编报1；自动降水采样器采样，编报2。';

comment on column T_ACIDOBT.ddatetime is
'酸雨观测日期。';

comment on column T_ACIDOBT.tbegin is
'降水开始时间。';

comment on column T_ACIDOBT.tend is
'降水结束时间。';

comment on column T_ACIDOBT.prept is
'酸雨观测样品对应的降水量，单位0.1mm。';

comment on column T_ACIDOBT.t is
'初测时的降水样品温度，单位0.1摄氏度。';

comment on column T_ACIDOBT.PH1 is
'初测时的降水样品第1次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.PH2 is
'初测时的降水样品第2次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.PH3 is
'初测时的降水样品第3次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.PHarv is
'初测时的降水样品pH值平均值，取两位小数。';

comment on column T_ACIDOBT.K1 is
'初测时的降水样品第1次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.K2 is
'初测时的降水样品第2次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.K3 is
'初测时的降水样品第3次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.Karv is
'降水样品25℃时的初测K值平均值，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.ret is
'复测时的降水样品温度，单位0.1摄氏度。';

comment on column T_ACIDOBT.rePH1 is
'复测时的降水样品第1次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.rePH2 is
'复测时的降水样品第2次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.rePH3 is
'复测时的降水样品第3次pH值测量读数，取两位小数。';

comment on column T_ACIDOBT.rePHarv is
'降水样品的复测pH平均值，取两位小数。';

comment on column T_ACIDOBT.reK1 is
'复测时的降水样品第1次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.reK2 is
'复测时的降水样品第2次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.reK3 is
'复测时的降水样品第3次K值测量读数，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.reKarv is
'降水样品25℃时的复测K值平均值，以0.1μS·m-1为单位。';

comment on column T_ACIDOBT.wddir14 is
'降水采样日界内14时自记或十分钟平均风向。';

comment on column T_ACIDOBT.wdsp14 is
'降水采样日界内14时自记或十分钟平均风速。单位0.1m?s-1。';

comment on column T_ACIDOBT.wddir20 is
'降水采样日界内20时自记或十分钟平均风向。';

comment on column T_ACIDOBT.wdsp20 is
'降水采样日界内20时自记或十分钟平均风速。单位0.1m?s-1。';

comment on column T_ACIDOBT.wddir02 is
'降水采样日界内02时自记或十分钟平均风向。';

comment on column T_ACIDOBT.wdsp02 is
'降水采样日界内02时自记或十分钟平均风速。单位0.1m?s-1。';

comment on column T_ACIDOBT.wddir08 is
'降水采样日界内08时自记或十分钟平均风向。';

comment on column T_ACIDOBT.wdsp08 is
'降水采样日界内08时自记或十分钟平均风速。单位0.1m?s-1。';

comment on column T_ACIDOBT.wtph1 is
'天气现象，具体详见说明文档附表10-1。';

comment on column T_ACIDOBT.wtph2 is
'天气现象，具体详见说明文档附表10-1。';

comment on column T_ACIDOBT.wtph3 is
'天气现象，具体详见说明文档附表10-1。';

comment on column T_ACIDOBT.wtph4 is
'天气现象，具体详见说明文档附表10-1。';

comment on column T_ACIDOBT.acidnote is
'酸雨观测备注，具体详见说明文档。';

comment on column T_ACIDOBT.crttime is
'入库时间，默认sysdate。';

comment on column T_ACIDOBT.keyid is
'记录序号，';

/*==============================================================*/
/* Index: IDX_ACIDOBT_1                                         */
/*==============================================================*/
create index IDX_ACIDOBT_1 on T_ACIDOBT (
   acidobt ASC
);

/*==============================================================*/
/* Index: IDX_ACIDOBT_2                                         */
/*==============================================================*/
create index IDX_ACIDOBT_2 on T_ACIDOBT (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_ACIDOBT_3                                         */
/*==============================================================*/
create index IDX_ACIDOBT_3 on T_ACIDOBT (
   crttime ASC
);

/*==============================================================*/
/* Index: IDX_ACIDOBT_4                                         */
/*==============================================================*/
create index IDX_ACIDOBT_4 on T_ACIDOBT (
   keyid ASC
);

/*==============================================================*/
/* Table: T_AREACODE                                            */
/*==============================================================*/
create table T_AREACODE 
(
   obtid                char(4)              not null,
   obtname              varchar2(30)         not null,
   lat                  number(8,2),
   lon                  number(8,2),
   rsts                 number(1)            default 1 not null
      constraint CKC_RSTS_T_AREACO check (rsts in (1,2)),
   constraint PK_T_AREACODE primary key (obtid)
)
tablespace USERS;

comment on table T_AREACODE is
'本市预警发布站点。';

comment on column T_AREACODE.obtid is
'街道代码，预警分区街道字母代码。';

comment on column T_AREACODE.obtname is
'街道名称，预警分区街道的名称。';

comment on column T_AREACODE.lat is
'纬度。';

comment on column T_AREACODE.lon is
'经度。';

comment on column T_AREACODE.rsts is
'状态，1-正常，2-启用';

/*==============================================================*/
/* Table: T_AUTOSTVIS                                           */
/*==============================================================*/
drop sequence SEQ_AUTOSTVIS;
create sequence SEQ_AUTOSTVIS increment by 1 start with 1;
create table T_AUTOSTVIS 
(
   statid               char(5)              not null,
   ddatetime            date                 not null,
   hourminu             char(5)              not null,
   minuvis              number(6),
   minumaxvis           number(6),
   minumaxvist          char(5),
   minuminvis           number(6),
   minuminvist          char(5),
   minuvis10            number(6),
   minumaxvis10         number(6),
   minumaxvist10        char(5),
   minuminvis10         number(6),
   minuminvist10        char(5),
   qccode               char(10),
   crttime              date                 default sysdate not null,
   keyid                number               not null,
   constraint PK_AUTOSTVIS primary key (statid, ddatetime, hourminu)
)
tablespace USERS;

comment on table T_AUTOSTVIS is
'自动站能见度数据';

comment on column T_AUTOSTVIS.statid is
'站点代号。';

comment on column T_AUTOSTVIS.ddatetime is
'数据时间。';

comment on column T_AUTOSTVIS.hourminu is
'时分。';

comment on column T_AUTOSTVIS.minuvis is
'分钟能见度。';

comment on column T_AUTOSTVIS.minumaxvis is
'分钟最大能见度。';

comment on column T_AUTOSTVIS.minumaxvist is
'分钟最大能见度时间。';

comment on column T_AUTOSTVIS.minuminvis is
'分钟最小能见度。';

comment on column T_AUTOSTVIS.minuminvist is
'分钟最小能见度时间。';

comment on column T_AUTOSTVIS.minuvis10 is
'10分钟能见度。';

comment on column T_AUTOSTVIS.minumaxvis10 is
'10分钟最大能见度。';

comment on column T_AUTOSTVIS.minumaxvist10 is
'10分钟最大能见度时间。';

comment on column T_AUTOSTVIS.minuminvis10 is
'10分钟最小能见度。';

comment on column T_AUTOSTVIS.minuminvist10 is
'10分钟最小能见度时间。';

comment on column T_AUTOSTVIS.qccode is
'质控控制码。';

comment on column T_AUTOSTVIS.crttime is
'入库时间，默认为sysdate。';

comment on column T_AUTOSTVIS.keyid is
'记录序号，从SEQ_AUTOSTVIS中获取。';

/*==============================================================*/
/* Index: IDX_AUTOSTVIS_1                                       */
/*==============================================================*/
create index IDX_AUTOSTVIS_1 on T_AUTOSTVIS (
   statid ASC
);

/*==============================================================*/
/* Index: IDX_AUTOSTVIS_2                                       */
/*==============================================================*/
create index IDX_AUTOSTVIS_2 on T_AUTOSTVIS (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AUTOSTVIS_3                                       */
/*==============================================================*/
create index IDX_AUTOSTVIS_3 on T_AUTOSTVIS (
   hourminu ASC
);

/*==============================================================*/
/* Index: IDX_AUTOSTVIS_4                                       */
/*==============================================================*/
create index IDX_AUTOSTVIS_4 on T_AUTOSTVIS (
   crttime ASC
);

/*==============================================================*/
/* Index: IDX_AUTOSTVIS_5                                       */
/*==============================================================*/
create index IDX_AUTOSTVIS_5 on T_AUTOSTVIS (
   keyid ASC
);

/*==============================================================*/
/* Table: T_CLOUDOBT                                            */
/*==============================================================*/
drop sequence SEQ_CLOUDOBT;
create sequence SEQ_CLOUDOBT increment by 1 start with 1;
create table T_CLOUDOBT 
(
   statno               char(6)              not null,
   servtype             char(2),
   devflag              char(4),
   devid                char(3),
   ddatetime            date                 not null,
   framelabel           char(2),
   obfeatures           char(2),
   devstatus            char(2),
   fname1               char(5),
   fvalue1              number(4),
   fname2               char(5),
   fvalue2              number(4),
   fname3               char(5),
   fvalue3              number(4),
   fname4               char(5),
   fvalue4              number(4),
   fname5               char(5),
   fvalue5              number(4),
   fname6               char(5),
   fvalue6              number(4),
   fname7               char(5),
   fvalue7              number(4),
   fname8               char(5),
   fvalue8              number(4),
   qualitybit           varchar(10),
   sname1               char(5),
   svalue1              number(1),
   sname2               char(5),
   svalue2              number(1),
   sname3               char(5),
   svalue3              number(1),
   sname4               char(5),
   svalue4              number(1),
   sname5               char(5),
   svalue5              number(1),
   sname6               char(5),
   svalue6              number(1),
   sname7               char(5),
   svalue7              number(1),
   sname8               char(5),
   svalue8              number(1),
   sname9               char(5),
   svalue9              number(1),
   sname10              char(5),
   svalue10             number(1),
   checkcode            char(4),
   crttime              date                 default sysdate not null,
   keyid                number               not null,
   constraint PK_CLOUDOBT primary key (statno, ddatetime)
)
tablespace USERS;

comment on table T_CLOUDOBT is
'云量观测数据，缺少质量控制位的数据被丢弃。';

comment on column T_CLOUDOBT.statno is
'台站号。';

comment on column T_CLOUDOBT.servtype is
'服务类型，00~99。';

comment on column T_CLOUDOBT.devflag is
'设备标识位，4位字母。';

comment on column T_CLOUDOBT.devid is
'设备ID，000~999。';

comment on column T_CLOUDOBT.ddatetime is
'观测时间，yyyy-mm-dd hh24:mi:ss。';

comment on column T_CLOUDOBT.framelabel is
'帧标示，分钟数据';

comment on column T_CLOUDOBT.obfeatures is
'观测要素变量数，00~99。';

comment on column T_CLOUDOBT.devstatus is
'设备状态变量数，00~99。';

comment on column T_CLOUDOBT.fname1 is
'总云量（AL0B）。';

comment on column T_CLOUDOBT.fvalue1 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue2 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue3 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue4 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue5 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue6 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue7 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.fvalue8 is
'小数点后扩大10倍。';

comment on column T_CLOUDOBT.qualitybit is
'质量控制位。';

comment on column T_CLOUDOBT.sname1 is
'外接电源（ZDA）。';

comment on column T_CLOUDOBT.svalue1 is
'0交流，1直流';

comment on column T_CLOUDOBT.svalue2 is
'0偏低，1正常。';

comment on column T_CLOUDOBT.svalue3 is
'0偏低，1正常，2偏高。';

comment on column T_CLOUDOBT.svalue4 is
'0偏低，1正常，2偏高。';

comment on column T_CLOUDOBT.svalue5 is
'0偏低，1正常，2偏高。';

comment on column T_CLOUDOBT.svalue6 is
'0偏低，1正常，2偏高，3停止。';

comment on column T_CLOUDOBT.svalue7 is
'0偏低，1正常，2偏高，3停止。';

comment on column T_CLOUDOBT.svalue8 is
'0偏低，1正常，2偏高，3停止。';

comment on column T_CLOUDOBT.svalue9 is
'0偏低，1正常，2偏高，3停止。';

comment on column T_CLOUDOBT.svalue10 is
'0偏低，1正常，2偏高，3停止。';

comment on column T_CLOUDOBT.checkcode is
'校验码。';

comment on column T_CLOUDOBT.crttime is
'入库时间。';

comment on column T_CLOUDOBT.keyid is
'记录序号，从SEQ_CLOUDOBT中获取。';

/*==============================================================*/
/* Index: IDX_CLOUDOBT_1                                        */
/*==============================================================*/
create index IDX_CLOUDOBT_1 on T_CLOUDOBT (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_CLOUDOBT_2                                        */
/*==============================================================*/
create index IDX_CLOUDOBT_2 on T_CLOUDOBT (
   servtype ASC
);

/*==============================================================*/
/* Index: IDX_CLOUDOBT_3                                        */
/*==============================================================*/
create index IDX_CLOUDOBT_3 on T_CLOUDOBT (
   statno ASC
);

/*==============================================================*/
/* Index: IDX_CLOUDOBT_4                                        */
/*==============================================================*/
create index IDX_CLOUDOBT_4 on T_CLOUDOBT (
   checkcode ASC
);

/*==============================================================*/
/* Table: T_COUNTRYSIGNAL                                       */
/*==============================================================*/
drop sequence SEQ_COUNTRYSIGNAL;
create sequence SEQ_COUNTRYSIGNAL increment by 1 start with 1;
create table T_COUNTRYSIGNAL 
(
   province             varchar2(20)         not null,
   city                 varchar2(20)         not null,
   county               varchar2(20)         not null,
   categoryid           char(4)              not null,
   categoryname         varchar2(10),
   levelid              char(4),
   levelname            varchar2(10),
   datetime             date                 not null,
   content              varchar2(1000),
   warsignalid          varchar2(200),
   webhtml              varchar2(200),
   crttime              date                 default sysdate not null,
   keyid                number(6)            not null,
   constraint AK_PK_COUNTRYSIGNAL_T_COUNTR unique (province, city, county, categoryid, datetime)
)
tablespace USERS;

comment on table T_COUNTRYSIGNAL is
'全国预警信号, 采用JSON数据格式';

comment on column T_COUNTRYSIGNAL.province is
'预警发布单位的省级名称';

comment on column T_COUNTRYSIGNAL.city is
'预警发布单位的市级名称';

comment on column T_COUNTRYSIGNAL.county is
'预警发布单位的县级名称';

comment on column T_COUNTRYSIGNAL.categoryid is
'预警类别编号 ';

comment on column T_COUNTRYSIGNAL.categoryname is
'预警类别名称';

comment on column T_COUNTRYSIGNAL.levelid is
'预警级别编号';

comment on column T_COUNTRYSIGNAL.levelname is
'预警级别名称';

comment on column T_COUNTRYSIGNAL.datetime is
'预警发布时间';

comment on column T_COUNTRYSIGNAL.content is
'预警发布内容 ';

comment on column T_COUNTRYSIGNAL.warsignalid is
'预警ID';

comment on column T_COUNTRYSIGNAL.webhtml is
'网页链接';

comment on column T_COUNTRYSIGNAL.crttime is
'入库时间。';

comment on column T_COUNTRYSIGNAL.keyid is
'记录序号，从SEQ_COUNTRYSIGNAL中获取。';

/*==============================================================*/
/* Index: IDX_COUNTRYSIGNAL_1                                   */
/*==============================================================*/
create index IDX_COUNTRYSIGNAL_1 on T_COUNTRYSIGNAL (
   province ASC
);

/*==============================================================*/
/* Index: IDX_COUNTRYSIGNAL_2                                   */
/*==============================================================*/
create index IDX_COUNTRYSIGNAL_2 on T_COUNTRYSIGNAL (
   city ASC
);

/*==============================================================*/
/* Index: IDX_COUNTRYSIGNAL_3                                   */
/*==============================================================*/
create index IDX_COUNTRYSIGNAL_3 on T_COUNTRYSIGNAL (
   county ASC
);

/*==============================================================*/
/* Index: IDX_COUNTRYSIGNAL_4                                   */
/*==============================================================*/
create index IDX_COUNTRYSIGNAL_4 on T_COUNTRYSIGNAL (
   categoryid ASC
);

/*==============================================================*/
/* Index: IDX_COUNTRYSIGNAL_5                                   */
/*==============================================================*/
create index IDX_COUNTRYSIGNAL_5 on T_COUNTRYSIGNAL (
   datetime ASC
);

/*==============================================================*/
/* Table: T_OBTCODE                                             */
/*==============================================================*/
create table T_OBTCODE 
(
   obtid                CHAR(5)              not null,
   obtname              varchar2(30)         not null,
   provname             varchar2(30)         not null,
   lat                  number(5,2),
   lon                  number(5,2),
   height               number(8,2),
   rsts                 number(1)            default 1 not null
      constraint CKC_RSTS_T_OBTCOD check (rsts in (1,2,3)),
   constraint PK_OBTCODE primary key (obtid)
)
tablespace USERS;

comment on table T_OBTCODE is
'该表存放了全国气象站点参数，2019年全国气象站点有839个站点。';

comment on column T_OBTCODE.obtid is
'站点代码';

comment on column T_OBTCODE.obtname is
'站点名称，一般用城市的名称。';

comment on column T_OBTCODE.provname is
'所属省，采用省的简称。如新疆维吾尔自治区写作新疆';

comment on column T_OBTCODE.lat is
'纬度';

comment on column T_OBTCODE.lon is
'经度';

comment on column T_OBTCODE.height is
'高度，指气压传感器的安装的海拔高度。';

comment on column T_OBTCODE.rsts is
'状态，1-正常，2-禁用，3-故障。';

/*==============================================================*/
/* Table: T_REFINEDATA                                          */
/*==============================================================*/
drop sequence SEQ_REFINEDATA;
create sequence SEQ_REFINEDATA increment by 1 start with 1;
create table T_REFINEDATA 
(
   prodcode             char(6)              not null,
   ddatetime            date                 not null,
   totalstat            number(4),
   obtid                char(6)              not null,
   lon                  number(5,2),
   lat                  number(4,2),
   height               number(5,1),
   agings               number(2),
   forprods             number(2),
   timestep             number(3)            not null,
   temperature          number(5,1),
   rehumid              number(5,1),
   wddirect             number(5,1),
   wdspeed              number(5,1),
   pressure             number(5,1),
   precipit             number(5,1),
   totalcloud           number(5,1),
   lowcloud             number(5,1),
   pweather             number(5,1),
   visibility           number(5,1),
   maxtemper            number(5,1),
   mintemper            number(5,1),
   maxrehumid           number(5,1),
   minrehumid           number(5,1),
   hcp24                number(5,1),
   hcp12                number(5,1),
   tcloud12             number(5,1),
   lcloud12             number(5,1),
   pweath12             number(5,1),
   wddir12              number(5,1),
   wdspeed12            number(5,1),
   crttime              date,
   keyid                number,
   constraint PK_REFINEDATA primary key (prodcode, ddatetime, obtid, timestep)
)
tablespace USERS;

comment on table T_REFINEDATA is
'全国城市精细化天气预报数据';

/*==============================================================*/
/* Index: IDX_REFINEDATA_1                                      */
/*==============================================================*/
create index IDX_REFINEDATA_1 on T_REFINEDATA (
   prodcode ASC
);

/*==============================================================*/
/* Index: IDX_REFINEDATA_2                                      */
/*==============================================================*/
create index IDX_REFINEDATA_2 on T_REFINEDATA (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_REFINEDATA_3                                      */
/*==============================================================*/
create index IDX_REFINEDATA_3 on T_REFINEDATA (
   obtid ASC
);

/*==============================================================*/
/* Index: IDX_REFINEDATA_4                                      */
/*==============================================================*/
create index IDX_REFINEDATA_4 on T_REFINEDATA (
   timestep ASC
);

/*==============================================================*/
/* Table: T_SIGNALLOG                                           */
/*==============================================================*/
drop sequence SEQ_SIGNALLOG;
create sequence SEQ_SIGNALLOG increment by 1 start with 1;
create table T_SIGNALLOG 
(
   obtid                char(4)              not null,
   ddatetime            date                 not null,
   signalname           char(1)              not null
      constraint CKC_SIGNALNAME_T_SIGNAL check (signalname in ('A','B','C','D','E','F','G','H','I','J','Q','R','P')),
   signalcolor          char(1)             
      constraint CKC_SIGNALCOLOR_T_SIGNAL check (signalcolor is null or (signalcolor in ('0','1','2','3','4','5'))),
   crttime              date                 default sysdate not null,
   keyid                number(6)            not null,
   constraint PK_T_SIGNALLOG primary key (obtid, ddatetime, signalname)
)
tablespace USERS;

comment on table T_SIGNALLOG is
'本市预警发布日志。';

comment on column T_SIGNALLOG.obtid is
'街道代码，预警分区分街道字母代码。';

comment on column T_SIGNALLOG.ddatetime is
'发布时间，预警信号发布的时间，北京时间。';

comment on column T_SIGNALLOG.signalname is
'信号名称，A－台风，B－暴雨，C－高温，D－寒冷，E－大雾，F－灰霾，H－道路结冰，I－冰雹，J－森林火险，Q－干旱，R－大风，P－雷电。';

comment on column T_SIGNALLOG.signalcolor is
'信号颜色，1－白色，2－蓝色，3－黄色，4－橙色，5－红色，0－解除。';

comment on column T_SIGNALLOG.crttime is
'入库时间，default sysdate。';

comment on column T_SIGNALLOG.keyid is
'记录序号，从SEQ_SIGNALLOG中获取。';

/*==============================================================*/
/* Table: T_SURFDATA                                            */
/*==============================================================*/
drop sequence seq_surfdata;
create sequence seq_surfdata increment by 1 start with 1;
create table T_SURFDATA 
(
   obtid                CHAR(5)              not null,
   ddatetime            DATE                 not null,
   t                    number(6,1),
   p                    number(6,1),
   u                    number(3),
   wd                   number(3),
   wf                   number(6,1),
   r                    number(6,1),
   vis                  number(7,1),
   crttime              DATE                 default sysdate not null,
   keyid                number(8)            not null,
   constraint PK_SURFDATA primary key (obtid, ddatetime)
)
tablespace USERS;

comment on table T_SURFDATA is
'该表存放了全国气象测试数据。';

comment on column T_SURFDATA.obtid is
'站点代码';

comment on column T_SURFDATA.ddatetime is
'数据时间';

comment on column T_SURFDATA.t is
'空气温度，单位，0.1摄氏度。';

comment on column T_SURFDATA.p is
'本站气压，0.1百帕。';

comment on column T_SURFDATA.u is
'相对湿度，0-100之间的值。';

comment on column T_SURFDATA.wd is
'风向，0-360之间的值。';

comment on column T_SURFDATA.wf is
'风速，单位0.1m/s。';

comment on column T_SURFDATA.r is
'降水，0.1mm。';

comment on column T_SURFDATA.vis is
'能见度，0.1米。';

comment on column T_SURFDATA.crttime is
'入库时间，default ，sysdate。';

comment on column T_SURFDATA.keyid is
'记录序号，从SEQ_SURFDATA中获取。';

/*==============================================================*/
/* Index: IDX_SURFDATA_1                                        */
/*==============================================================*/
create unique index IDX_SURFDATA_1 on T_SURFDATA (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_SURFDATA_2                                        */
/*==============================================================*/
create index IDX_SURFDATA_2 on T_SURFDATA (
   obtid ASC
);

/*==============================================================*/
/* Index: IDX_SURFDATA_3                                        */
/*==============================================================*/
create index IDX_SURFDATA_3 on T_SURFDATA (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_SURFZFILES                                          */
/*==============================================================*/
create table T_SURFZFILES 
(
   statid               char(5)              not null,
   ddatetime            date                 not null,
   lat                  char(9),
   lon                  char(10),
   obheight             number(5),
   senheight            number(5),
   obmethod             char(1),
   qclogo               char(3),
   filecd               char(3),
   statairp             number(5),
   sealp                number(5),
   trans3               number(4),
   trans24              number(4),
   maxstatp             number(5),
   maxstatpt            char(5),
   minstatp             number(5),
   minstatpt            char(5),
   airtmp               number(4),
   maxairtmp            number(4),
   maxairtmpt           char(5),
   minairtmp            number(4),
   minairtmpt           char(5),
   tmpchange24          number(4),
   maxtmpago24          number(4),
   mintmpago24          number(4),
   dptmp                number(4),
   rehum                number(3),
   minrehum             number(3),
   minrehumt            char(5),
   waterp               number(3),
   hourprecip           number(4),
   precipago3           number(5),
   precipago6           number(5),
   precipago12          number(5),
   precipago24          number(5),
   meoopdtp             char(2),
   meoop                number(5),
   houreva              number(4),
   wdminu2              number(3),
   wsminu2              number(3),
   wdminu10             number(3),
   wsmiu10              number(3),
   wdmaxws              number(3),
   wsmax                number(3),
   wsmaxt               char(5),
   wdins                number(3),
   wsins                number(3),
   wdgrtws              number(3),
   wsgrt                number(3),
   wsgrtt               char(5),
   wsgrtago6            number(3),
   wdgrtago6            number(3),
   wsgrtago12           number(3),
   wdgrtago12           number(3),
   gsurftmp             number(4),
   gsurfmaxtmp          number(4),
   gsurfmaxtmpt         char(5),
   gsurfmintmp          number(4),
   gsurfmintmpt         char(5),
   gsurfmintmpago12     number(4),
   gtmpcm5              number(4),
   gtmpcm10             number(4),
   gtmpcm15             number(4),
   gtmpcm20             number(4),
   gtmpcm40             number(4),
   gtmpcm80             number(4),
   gtmpcm160            number(4),
   gtmpcm320            number(4),
   gstmp                number(4),
   gsmaxtmp             number(4),
   gsmaxtmpt            char(5),
   gsmintmp             number(4),
   gsmintmpt            char(5),
   visarvminu1          number(5),
   visarvminu10         number(5),
   vismin               number(5),
   vismint              char(5),
   vis                  number(4),
   totalcloud           number(3),
   lowcloud             number(3),
   repcloud             number(3),
   highcloud            number(5),
   stcloud              char(24),
   stcloudid            char(3),
   wcodenow             char(2),
   pwdtp                char(2),
   wago1                char(1),
   wago2                char(1),
   gsurfst              char(2),
   constraint PK_T_SURFZFILES primary key (statid, ddatetime)
);

comment on table T_SURFZFILES is
'自动站数据Z文件。';

/*==============================================================*/
/* Table: T_TYPHOON_BEST                                        */
/*==============================================================*/
drop sequence SEQ_TYPHOON_BEST;
create sequence SEQ_TYPHOON_BEST increment by 1 start with 1;
create table T_TYPHOON_BEST 
(
   cycid                char(4)              not null,
   ddatetime            date                 not null,
   innaid               char(4)              not null,
   egname               char(20)             not null,
   cycno                char(4)              not null,
   classmk              char(5)              default '66666',
   fmtdate              date,
   pathno               number(3),
   intervals            number(3)            default 6,
   endsign              number(1)           
      constraint CKC_ENDSIGN_T_TYPHOO check (endsign is null or (endsign in (0,1,2,3))),
   intensitysign        number(1)           
      constraint CKC_INTENSITYSIGN_T_TYPHOO check (intensitysign is null or (intensitysign in (0,1,2,3,4,5,6,9))),
   lat                  number(3),
   lon                  number(4),
   pressure             number(4),
   wdmaxspeed           number(3),
   wdavespeed           char(3),
   crttime              date                 default sysdate not null,
   keyid                number(6)            not null,
   constraint PK_T_TYPHOON_BEST primary key (ddatetime, egname, cycno)
)
tablespace USERS;

comment on column T_TYPHOON_BEST.cycid is
'国内编号，我国对该热带气旋的编号。';

comment on column T_TYPHOON_BEST.ddatetime is
'数据时间，路径的具体时间，北京时间。';

comment on column T_TYPHOON_BEST.innaid is
'国际编号,后两位数+两位数的编号。';

comment on column T_TYPHOON_BEST.egname is
'英文名称，热带气旋的英文名称, 名称后加 "(-1)n" 表示副中心及其序号';

comment on column T_TYPHOON_BEST.cycno is
'气旋序号，包括热带低压在内的热带气旋序号。';

comment on column T_TYPHOON_BEST.classmk is
'分类标志, ''66666''表示最佳路径资料。';

comment on column T_TYPHOON_BEST.fmtdate is
'数据集形成的日期';

comment on column T_TYPHOON_BEST.pathno is
'路径数据记录的行数';

comment on column T_TYPHOON_BEST.intervals is
'每行路径间隔小时数, 目前均为6(小时)。';

comment on column T_TYPHOON_BEST.endsign is
'热带气旋终结标志:0表示消散, 1表示移出西太台风委员会的责任海区, 2表示合并, 3表示准静止。';

comment on column T_TYPHOON_BEST.intensitysign is
'强度标记, 以正点前2分钟至正点内的平均风速为准, 参见《热带气旋等级》国家标准（GB/T 19201-2006）：0- 弱于热带低压(TD), 或等级未知,1- 热带低压(TD, 10.8-17.1m/s),2- 热带风暴(TS,17.2-24.4 m/s),3- 强热带风暴(STS, 24.5-32.6 m/s),4- 台风(TY, 32.7-41.4 m/s),5- 强台风(STY, 41.5-50.9 m/s),6- 超强台风(SuperTY, ≥51.0 m/s),9- 变性,第一个标记表示变性完成
';

comment on column T_TYPHOON_BEST.lat is
'纬度(0.1°N)。';

comment on column T_TYPHOON_BEST.lon is
'经度(0.1°E)。';

comment on column T_TYPHOON_BEST.pressure is
'中心最低气压(hPa);  百帕。';

comment on column T_TYPHOON_BEST.wdmaxspeed is
'2分钟平均近中心最大风速(MSW, m/s)，WND=9 表示 MSW < 10m/s,WND=0 为缺测。';

comment on column T_TYPHOON_BEST.wdavespeed is
'2分钟平均风速(m/s), 有两种情况:(a) 对登陆我国的热带气旋, 表示沿海大风的风速;(b) 热带气旋位于南海时表示距中心300－500km范围的最大风速。';

comment on column T_TYPHOON_BEST.crttime is
'入库时间。';

comment on column T_TYPHOON_BEST.keyid is
'记录序号，从SEQ_TYPHOON_BEST中获取。';

alter table T_SURFDATA
   add constraint FK_SURFDA_OBTCOD foreign key (obtid)
      references T_OBTCODE (obtid);

exit;
