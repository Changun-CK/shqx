/*==============================================================*/
/* DBMS name:      ORACLE Version 9i2                           */
/* Created on:     2020/2/16 15:41:56                           */
/*==============================================================*/


alter table T_SURFDATA
   drop constraint FK_SURFDA_OBTCODE;

alter table T_AREACODE
   drop primary key cascade;

drop table T_AREACODE cascade constraints;

alter table T_AREAOBTCODE
   drop primary key cascade;

drop table T_AREAOBTCODE cascade constraints;

drop index IDX_AREAOBTDATA_4;

drop index IDX_AREAOBTDATA_3;

drop index IDX_AREAOBTDATA_2;

drop index IDX_AREAOBTDATA_1;

alter table T_AREAOBTDATA
   drop primary key cascade;

drop table T_AREAOBTDATA cascade constraints;

drop index IDX_AREAOBTMIND_3;

drop index IDX_AREAOBTMIND_2;

drop index IDX_AREAOBTMIND_1;

alter table T_AREAOBTMIND
   drop primary key cascade;

drop table T_AREAOBTMIND cascade constraints;

drop index IDX_CLOUDFILE_2;

drop index IDX_CLOUDFILE_1;

alter table T_CLOUDFILE
   drop primary key cascade;

drop table T_CLOUDFILE cascade constraints;

drop index IDX_CPUINFO_1;

alter table T_CPUINFO
   drop primary key cascade;

drop table T_CPUINFO cascade constraints;

drop index IDX_DBSESSIONINFO_1;

alter table T_DBSESSIONINFO
   drop primary key cascade;

drop table T_DBSESSIONINFO cascade constraints;

drop index IDX_DISKINFO_1;

alter table T_DISKINFO
   drop primary key cascade;

drop table T_DISKINFO cascade constraints;

drop index IDX_AREAAWSDAYD_4;

drop index IDX_AREAAWSDAYD_3;

drop index IDX_AREAAWSDAYD_2;

drop index IDX_AREAAWSDAYD_1;

alter table T_GDAWSDAYD
   drop primary key cascade;

drop table T_GDAWSDAYD cascade constraints;

drop index IDX_AREAOBTHOURD_4;

drop index IDX_AREAOBTHOURD_3;

drop index IDX_AREAOBTHOURD_2;

drop index IDX_AREAOBTHOURD_1;

alter table T_GDOBTHOURD
   drop primary key cascade;

drop table T_GDOBTHOURD cascade constraints;

drop index IDX_AREAOBTRAIND_3;

drop index IDX_AREAOBTRAIND_2;

drop index IDX_AREAOBTRAIND_1;

alter table T_GDOBTRAIND
   drop primary key cascade;

drop table T_GDOBTRAIND cascade constraints;

alter table T_GDOBTREALD
   drop primary key cascade;

drop table T_GDOBTREALD cascade constraints;

alter table T_INDEXINFO
   drop primary key cascade;

drop table T_INDEXINFO cascade constraints;

drop index IDX_MEMINFO_1;

alter table T_MEMINFO
   drop primary key cascade;

drop table T_MEMINFO cascade constraints;

drop index IDX_NETCHECKCFG_1;

alter table T_NETCHECKCFG
   drop primary key cascade;

drop table T_NETCHECKCFG cascade constraints;

alter table T_OBTCODE
   drop primary key cascade;

drop table T_OBTCODE cascade constraints;

drop index IDX_SHRADFILE_2;

drop index IDX_SHRADFILE_1;

alter table T_SHRADFILE
   drop primary key cascade;

drop table T_SHRADFILE cascade constraints;

drop index IDX_SIGNALLOG_3;

drop index IDX_SIGNALLOG_2;

drop index IDX_SIGNALLOG_1;

alter table T_SIGNALLOG
   drop primary key cascade;

drop table T_SIGNALLOG cascade constraints;

drop index IDX_SURFDATA_3;

drop index IDX_SURFDATA_2;

drop index IDX_SURFDATA_1;

drop table T_SURFDATA cascade constraints;

alter table T_SYSARG
   drop primary key cascade;

drop table T_SYSARG cascade constraints;

drop index IDX_TBSPACEINFO_1;

alter table T_TBSPACEINFO
   drop primary key cascade;

drop table T_TBSPACEINFO cascade constraints;

drop index IDX_ZHRAIN24_2;

drop index IDX_ZHRAIN24_1;

alter table T_ZHRAIN24
   drop primary key cascade;

drop table T_ZHRAIN24 cascade constraints;

drop index IDX_ZHTEMP24_2;

drop index IDX_ZHTEMP24_1;

alter table T_ZHTEMP24
   drop primary key cascade;

drop table T_ZHTEMP24 cascade constraints;

/*==============================================================*/
/* Table: T_AREACODE                                            */
/*==============================================================*/
create table T_AREACODE  (
   obtid              char(4)                         not null,
   obtname            varchar2(30)                    not null,
   lat                number(8,2),
   lon                number(8,2),
   rsts               number(1)                      default 1 not null
      constraint CKC_RSTS_T_AREACO check (rsts in (1,2,3))
);

comment on table T_AREACODE is
'本市预警发布站点。';

comment on column T_AREACODE.obtid is
'街道代码，警分区分街道字母代码。';

comment on column T_AREACODE.obtname is
'街道名称，警分区分街道的名称。';

comment on column T_AREACODE.lat is
'纬度';

comment on column T_AREACODE.lon is
'经度';

comment on column T_AREACODE.rsts is
'状态，1-正常，2-启用。';

alter table T_AREACODE
   add constraint PK_AREACODE primary key (obtid);

/*==============================================================*/
/* Table: T_AREAOBTCODE                                         */
/*==============================================================*/
create table T_AREAOBTCODE  (
   obtid              varchar2(10)                    not null,
   obtname            varchar2(50)                    not null,
   comname            varchar2(50),
   englishname        varchar2(50),
   lon                number(8,3),
   lat                number(8,3),
   height             number(8,2),
   cushion            varchar2(20),
   ys                 varchar2(8),
   w3a                varchar2(20),
   buildtime          date,
   obtaddress         varchar2(100),
   obtsts             number(1)                      default 1 not null
      constraint CKC_OBTSTS_T_AREAOB check (obtsts in (1,2,3,4)),
   memo               varchar2(300),
   orderby            number(6)
);

comment on table T_AREAOBTCODE is
'数据中心的WEB页面维护。';

comment on column T_AREAOBTCODE.obtid is
'区站代码';

comment on column T_AREAOBTCODE.obtname is
'区站名称';

comment on column T_AREAOBTCODE.comname is
'通用名称';

comment on column T_AREAOBTCODE.englishname is
'英文名称';

comment on column T_AREAOBTCODE.lon is
'站点经度，单位：度。';

comment on column T_AREAOBTCODE.lat is
'站点纬度，单位：度。';

comment on column T_AREAOBTCODE.height is
'站点高度，指探测仪器安装的海拔高度，单位：米。';

comment on column T_AREAOBTCODE.cushion is
'站点垫面，如草面，水泥等。';

comment on column T_AREAOBTCODE.ys is
'自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
目前，共5个权位，1-降雨；2-风速；3-气温；4-湿度；5-气压；其它的权位还没有启用。
经研究，省局取回的数据中，不管是探测要素字段，还是数据质量字段，都不准确，不可用。
';

comment on column T_AREAOBTCODE.w3a is
'站点型号';

comment on column T_AREAOBTCODE.buildtime is
'建站时间';

comment on column T_AREAOBTCODE.obtaddress is
'站点地址';

comment on column T_AREAOBTCODE.obtsts is
'站点状态，default 1。1-正常；2-停机；3-故障；4-报废';

comment on column T_AREAOBTCODE.memo is
'备注说明';

comment on column T_AREAOBTCODE.orderby is
'排列顺序';

alter table T_AREAOBTCODE
   add constraint PK_AREAOBTCODE primary key (obtid);

/*==============================================================*/
/* Table: T_AREAOBTDATA                                         */
/*==============================================================*/
drop   sequence SEQ_AREAOBTDATA;
create sequence SEQ_AREAOBTDATA increment by 1 minvalue 1 nocycle;
create table T_AREAOBTDATA  (
   ddatetime          Date                            not null,
   obtid              varchar2(10)                    not null,
   station            number(2),
   baoformat          number(1),
   bps                number(6),
   ys                 varchar2(8),
   w3a                varchar2(6),
   wdidf              number(6),
   wdidd              number(6),
   wd2df              number(6),
   wd2dd              number(6),
   wd10df             number(6),
   wd10dd             number(6),
   wd3smaxdf          number(6),
   wd3smaxdd          number(6),
   wd3smaxtime        number(2),
   wd10maxdf          number(6),
   wd10maxdd          number(6),
   wd10maxtime        number(2),
   wd3daymax          number(6),
   wf3daymax          number(6),
   wd3daymaxtime      number(4),
   wd10daymax         number(6),
   wf10daymax         number(6),
   wd10daymaxtime     number(4),
   t                  number(6),
   maxt               number(6),
   maxttime           number(2),
   mint               number(6),
   minttime           number(2),
   daymaxt            number(6),
   daymaxttime        number(4),
   daymint            number(6),
   dayminttime        number(4),
   rh                 number(6),
   maxrh              number(6),
   maxrhtime          number(2),
   minrh              number(6),
   minrhtime          number(2),
   dp                 number(6),
   p                  number(6),
   maxp               number(6),
   maxptime           number(2),
   minp               number(6),
   minptime           number(2),
   p0                 number(6),
   hourrf             number(6),
   dayrf              number(6),
   rfmark1            number(6),
   minrf              varchar(50),
   wpv                number(6),
   othfields          varchar2(2000),
   rddatetime         number(2)                       not null,
   procsts            number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   keyid              number(15)                      not null
);

comment on table T_AREAOBTDATA is
'这是一个临时表，该表的数据只保留7天，7天之前的数据由数据中心的managetable程序清除，业务系统不需要访问该表，而是访问T_AREAOBTMIND。';

comment on column T_AREAOBTDATA.ddatetime is
'数据时间';

comment on column T_AREAOBTDATA.obtid is
'区站代码';

comment on column T_AREAOBTDATA.station is
'自动站编号，数值为0-99等。';

comment on column T_AREAOBTDATA.baoformat is
'资料类型，0为正点报告，1 为瞬时报告。';

comment on column T_AREAOBTDATA.bps is
'测站高度，单位：0.1米。';

comment on column T_AREAOBTDATA.ys is
'自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
目前，共5个权位，1-降雨；2-风速；3-气温；4-湿度；5-气压；其它的权位还没有启用。
经研究，省局取回的数据中，不管是探测要素字段，还是数据质量字段，都不准确，不可用。
';

comment on column T_AREAOBTDATA.w3a is
'自动站型号';

comment on column T_AREAOBTDATA.wdidf is
'瞬时风速，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wdidd is
'瞬时风向，取值0到360。';

comment on column T_AREAOBTDATA.wd2df is
'二分钟风速，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd2dd is
'二分钟风向，取值0到360。';

comment on column T_AREAOBTDATA.wd10df is
'十分钟风速，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd10dd is
'十分钟风向，取值0到360。';

comment on column T_AREAOBTDATA.wd3smaxdf is
'极大风速，本小时内出现的最大瞬时风速值，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd3smaxdd is
'极大风向，极大风速的风向，取值0到360。';

comment on column T_AREAOBTDATA.wd3smaxtime is
'极大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.wd10maxdf is
'最大风速，本小时内出现的最大10分钟平均风速值，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd10maxdd is
'最大风向，最大风速的风向，取值0到360。';

comment on column T_AREAOBTDATA.wd10maxtime is
'最大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.wd3daymax is
'日瞬时极大风速时的风向，取值0到360。';

comment on column T_AREAOBTDATA.wf3daymax is
'日瞬时极大风速，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd3daymaxtime is
'日瞬时极大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTDATA.wd10daymax is
'日十分钟最大风速时的风向。';

comment on column T_AREAOBTDATA.wf10daymax is
'日十分钟最大风速，单位：0.1m/s。';

comment on column T_AREAOBTDATA.wd10daymaxtime is
'日十分钟最大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTDATA.t is
'空气温度，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.maxt is
'最高气温，本小时内的最高气温，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.maxttime is
'最高气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.mint is
'最低气温，本小时内的最低气温，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.minttime is
'最低气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.daymaxt is
'日最高温度，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.daymaxttime is
'日最高温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTDATA.daymint is
'日最低温度，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.dayminttime is
'日最低温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTDATA.rh is
'相对湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTDATA.maxrh is
'最大湿度，本小时内的最大湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTDATA.maxrhtime is
'最大湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.minrh is
'最小湿度，本小时内的最小湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTDATA.minrhtime is
'最小湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.dp is
'露点温度，单位：0.1摄氏度。';

comment on column T_AREAOBTDATA.p is
'本站气压，单位：0.1百帕。';

comment on column T_AREAOBTDATA.maxp is
'最高气压，本小时内的最高气压，单位：0.1百帕。';

comment on column T_AREAOBTDATA.maxptime is
'最高气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.minp is
'最低气压，本小时内的最低气压，单位：0.1百帕。';

comment on column T_AREAOBTDATA.minptime is
'最低气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTDATA.p0 is
'海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数，省港澳自动站还没有海拔高度参数。';

comment on column T_AREAOBTDATA.hourrf is
'小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。';

comment on column T_AREAOBTDATA.dayrf is
'日雨量累计，日界为：08:00-08:00,即每天的08:00清零。';

comment on column T_AREAOBTDATA.rfmark1 is
'当前一分钟雨量指示,没有说明。';

comment on column T_AREAOBTDATA.minrf is
'一小时的每五分钟的雨量分布。';

comment on column T_AREAOBTDATA.wpv is
'自动站电池电压，单位：0.01伏。';

comment on column T_AREAOBTDATA.othfields is
'其它要素，采用XML表达。';

comment on column T_AREAOBTDATA.rddatetime is
'实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启用。';

comment on column T_AREAOBTDATA.procsts is
'处理状态，default 1，1-未处理，2-已处理。';

comment on column T_AREAOBTDATA.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间。';

comment on column T_AREAOBTDATA.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号。';

alter table T_AREAOBTDATA
   add constraint PK_AREAOBTDATA primary key (ddatetime, obtid);

/*==============================================================*/
/* Index: IDX_AREAOBTDATA_1                                     */
/*==============================================================*/
create unique index IDX_AREAOBTDATA_1 on T_AREAOBTDATA (
   obtid ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTDATA_2                                     */
/*==============================================================*/
create unique index IDX_AREAOBTDATA_2 on T_AREAOBTDATA (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTDATA_3                                     */
/*==============================================================*/
create index IDX_AREAOBTDATA_3 on T_AREAOBTDATA (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTDATA_4                                     */
/*==============================================================*/
create index IDX_AREAOBTDATA_4 on T_AREAOBTDATA (
   procsts ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_AREAOBTMIND                                         */
/*==============================================================*/
drop   sequence SEQ_AREAOBTMIND;
create sequence SEQ_AREAOBTMIND increment by 1 minvalue 1 nocycle;
create table T_AREAOBTMIND  (
   ddatetime          Date                            not null,
   obtid              varchar2(10)                    not null,
   station            number(2),
   baoformat          number(1),
   bps                number(6),
   ys                 varchar2(8),
   w3a                varchar2(6),
   wdidf              number(6),
   wdidd              number(6),
   wd2df              number(6),
   wd2dd              number(6),
   wd10df             number(6),
   wd10dd             number(6),
   wd3smaxdf          number(6),
   wd3smaxdd          number(6),
   wd3smaxtime        number(2),
   wd10maxdf          number(6),
   wd10maxdd          number(6),
   wd10maxtime        number(2),
   wd3daymax          number(6),
   wf3daymax          number(6),
   wd3daymaxtime      number(4),
   wd10daymax         number(6),
   wf10daymax         number(6),
   wd10daymaxtime     number(4),
   t                  number(6),
   maxt               number(6),
   maxttime           number(2),
   mint               number(6),
   minttime           number(2),
   daymaxt            number(6),
   daymaxttime        number(4),
   daymint            number(6),
   dayminttime        number(4),
   rh                 number(6),
   maxrh              number(6),
   maxrhtime          number(2),
   minrh              number(6),
   minrhtime          number(2),
   dp                 number(6),
   p                  number(6),
   maxp               number(6),
   maxptime           number(2),
   minp               number(6),
   minptime           number(2),
   p0                 number(6),
   hourrf             number(6),
   dayrf              number(6),
   rfmark1            number(6),
   minrf              varchar(50),
   wpv                number(6),
   othfields          varchar2(2000),
   rddatetime         number(2)                       not null,
   procsts            number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   keyid              number(15)                      not null
);

comment on column T_AREAOBTMIND.ddatetime is
'数据时间';

comment on column T_AREAOBTMIND.obtid is
'区站代码';

comment on column T_AREAOBTMIND.station is
'自动站编号，数值为0-99等。';

comment on column T_AREAOBTMIND.baoformat is
'资料类型，0为正点报告，1 为瞬时报告。';

comment on column T_AREAOBTMIND.bps is
'测站高度，单位：0.1米。';

comment on column T_AREAOBTMIND.ys is
'自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
目前，共5个权位，1-降雨；2-风速；3-气温；4-湿度；5-气压；其它的权位还没有启用。
经研究，省局取回的数据中，不管是探测要素字段，还是数据质量字段，都不准确，不可用。
';

comment on column T_AREAOBTMIND.w3a is
'自动站型号';

comment on column T_AREAOBTMIND.wdidf is
'瞬时风速，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wdidd is
'瞬时风向，取值0到360。';

comment on column T_AREAOBTMIND.wd2df is
'二分钟风速，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd2dd is
'二分钟风向，取值0到360。';

comment on column T_AREAOBTMIND.wd10df is
'十分钟风速，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd10dd is
'十分钟风向，取值0到360。';

comment on column T_AREAOBTMIND.wd3smaxdf is
'极大风速，本小时内出现的最大瞬时风速值，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd3smaxdd is
'极大风向，极大风速的风向，取值0到360。';

comment on column T_AREAOBTMIND.wd3smaxtime is
'极大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.wd10maxdf is
'最大风速，本小时内出现的最大10分钟平均风速值，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd10maxdd is
'最大风向，最大风速的风向，取值0到360。';

comment on column T_AREAOBTMIND.wd10maxtime is
'最大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.wd3daymax is
'日瞬时极大风速时的风向，取值0到360。';

comment on column T_AREAOBTMIND.wf3daymax is
'日瞬时极大风速，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd3daymaxtime is
'日瞬时极大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTMIND.wd10daymax is
'日十分钟最大风速时的风向。';

comment on column T_AREAOBTMIND.wf10daymax is
'日十分钟最大风速，单位：0.1m/s。';

comment on column T_AREAOBTMIND.wd10daymaxtime is
'日十分钟最大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTMIND.t is
'空气温度，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.maxt is
'最高气温，本小时内的最高气温，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.maxttime is
'最高气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.mint is
'最低气温，本小时内的最低气温，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.minttime is
'最低气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.daymaxt is
'日最高温度，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.daymaxttime is
'日最高温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTMIND.daymint is
'日最低温度，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.dayminttime is
'日最低温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_AREAOBTMIND.rh is
'相对湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTMIND.maxrh is
'最大湿度，本小时内的最大湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTMIND.maxrhtime is
'最大湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.minrh is
'最小湿度，本小时内的最小湿度，一个百分比值，取值1到100。';

comment on column T_AREAOBTMIND.minrhtime is
'最小湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.dp is
'露点温度，单位：0.1摄氏度。';

comment on column T_AREAOBTMIND.p is
'本站气压，单位：0.1百帕。';

comment on column T_AREAOBTMIND.maxp is
'最高气压，本小时内的最高气压，单位：0.1百帕。';

comment on column T_AREAOBTMIND.maxptime is
'最高气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.minp is
'最低气压，本小时内的最低气压，单位：0.1百帕。';

comment on column T_AREAOBTMIND.minptime is
'最低气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_AREAOBTMIND.p0 is
'海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数，省港澳自动站还没有海拔高度参数。';

comment on column T_AREAOBTMIND.hourrf is
'小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。';

comment on column T_AREAOBTMIND.dayrf is
'日雨量累计，日界为：08:00-08:00,即每天的08:00清零。';

comment on column T_AREAOBTMIND.rfmark1 is
'当前一分钟雨量指示,没有说明。';

comment on column T_AREAOBTMIND.minrf is
'一小时的每五分钟的雨量分布。';

comment on column T_AREAOBTMIND.wpv is
'自动站电池电压，单位：0.01伏。';

comment on column T_AREAOBTMIND.othfields is
'其它要素，采用XML表达。';

comment on column T_AREAOBTMIND.rddatetime is
'实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启用。';

comment on column T_AREAOBTMIND.procsts is
'处理状态，default 1，1-未处理，2-已处理。';

comment on column T_AREAOBTMIND.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间。';

comment on column T_AREAOBTMIND.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号。';

alter table T_AREAOBTMIND
   add constraint PK_AREAOBTMIND primary key (ddatetime, obtid);

/*==============================================================*/
/* Index: IDX_AREAOBTMIND_1                                     */
/*==============================================================*/
create unique index IDX_AREAOBTMIND_1 on T_AREAOBTMIND (
   obtid ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTMIND_2                                     */
/*==============================================================*/
create unique index IDX_AREAOBTMIND_2 on T_AREAOBTMIND (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTMIND_3                                     */
/*==============================================================*/
create index IDX_AREAOBTMIND_3 on T_AREAOBTMIND (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_CLOUDFILE                                           */
/*==============================================================*/
drop   sequence SEQ_CLOUDFILE;
create sequence SEQ_CLOUDFILE increment by 1 minvalue 1 nocycle;
create table T_CLOUDFILE  (
   filename           varchar2(200)                   not null,
   ddatetime          date                            not null,
   filesize           number(10)                      not null,
   filecontent        BLOB,
   crttime            date                           default sysdate,
   keyid              number(15)                      not null
);

comment on column T_CLOUDFILE.filename is
'文件名称';

comment on column T_CLOUDFILE.ddatetime is
'数据时间，文件中的数据所属的时间，例如雷达图文件MOC2008123010600.GIF，表示该图片是2008123010600的雷达扫描数据，与该文件的处理时间或入库时间没有关系。';

comment on column T_CLOUDFILE.filesize is
'文件大小，单位：字节。';

comment on column T_CLOUDFILE.filecontent is
'文件内容';

comment on column T_CLOUDFILE.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间，由数据中心创建该字段。';

comment on column T_CLOUDFILE.keyid is
'记录序号，从序列与本表同名的序列生成器中获取，数据中心用于同步的编号。';

alter table T_CLOUDFILE
   add constraint PK_CLOUDFILE primary key (filename);

/*==============================================================*/
/* Index: IDX_CLOUDFILE_1                                       */
/*==============================================================*/
create unique index IDX_CLOUDFILE_1 on T_CLOUDFILE (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_CLOUDFILE_2                                       */
/*==============================================================*/
create index IDX_CLOUDFILE_2 on T_CLOUDFILE (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_CPUINFO                                             */
/*==============================================================*/
drop   sequence SEQ_CPUINFO;
create sequence SEQ_CPUINFO increment by 1 minvalue 1 nocycle;
create table T_CPUINFO  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   nodip              varchar2(30)                    not null,
   users              number(6,2)                    default 0,
   sys                number(6,2)                    default 0,
   nice               number(6,2)                    default 0,
   idle               number(6,2)                    default 0,
   wait               number(6,2)                    default 0,
   irq                number(6,2)                    default 0,
   softirq            number(6,2)                    default 0,
   usep               number(6,2)                    default 0 not null,
   alarmvalue         number(6)                      default 50 not null,
   alarmsts           number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   memo               varchar2(2000),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_CPUINFO is
'本表的数据由cpuinfotodb程序插入和更新，WEB页面只维护任务名称和告警阀值字段。';

comment on column T_CPUINFO.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_CPUINFO.taskname is
'任务名称，后台程序用节点地址填充本字段，无需界面维护。';

comment on column T_CPUINFO.nodip is
'IP地址';

comment on column T_CPUINFO.users is
'用户空间';

comment on column T_CPUINFO.sys is
'内核空间';

comment on column T_CPUINFO.nice is
'用户进程';

comment on column T_CPUINFO.idle is
'空闲CPU';

comment on column T_CPUINFO.wait is
'I/O等待';

comment on column T_CPUINFO.irq is
'硬件中断';

comment on column T_CPUINFO.softirq is
'软件中断';

comment on column T_CPUINFO.usep is
'已用比例';

comment on column T_CPUINFO.alarmvalue is
'告警阀值';

comment on column T_CPUINFO.alarmsts is
'告警状态，1-正常；2-告警中；3-已过期。';

comment on column T_CPUINFO.crttime is
'更新时间';

comment on column T_CPUINFO.memo is
'备注说明';

comment on column T_CPUINFO.rsts is
'记录状态，default 1。1-启用；2-禁用，该字段暂时不启用。';

alter table T_CPUINFO
   add constraint PK_CPUINFO primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_CPUINFO_1                                         */
/*==============================================================*/
create unique index IDX_CPUINFO_1 on T_CPUINFO (
   nodip ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_DBSESSIONINFO                                       */
/*==============================================================*/
drop   sequence SEQ_DBSESSIONINFO;
create sequence SEQ_DBSESSIONINFO increment by 1 minvalue 1 nocycle;
create table T_DBSESSIONINFO  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   nodip              varchar2(30)                    not null,
   total              number(6)                      default 0 not null,
   alarmvalue         number(6)                      default 200 not null,
   alarmsts           number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   memo               varchar2(2000),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_DBSESSIONINFO is
'本表的数据由dbsessioninfotodb程序插入和更新，WEB页面只维护任务名称和告警阀值字段。';

comment on column T_DBSESSIONINFO.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_DBSESSIONINFO.taskname is
'任务名称，后台程序用节点地址填充本字段，无需界面维护。';

comment on column T_DBSESSIONINFO.nodip is
'IP地址';

comment on column T_DBSESSIONINFO.total is
'会话总数';

comment on column T_DBSESSIONINFO.alarmvalue is
'告警阀值';

comment on column T_DBSESSIONINFO.alarmsts is
'告警状态，1-正常；2-告警中；3-已过期。';

comment on column T_DBSESSIONINFO.crttime is
'更新时间';

comment on column T_DBSESSIONINFO.memo is
'备注说明';

comment on column T_DBSESSIONINFO.rsts is
'记录状态，default 1。1-启用；2-禁用，该字段暂时不启用。';

alter table T_DBSESSIONINFO
   add constraint PK_DBSESSIONINFO primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_DBSESSIONINFO_1                                   */
/*==============================================================*/
create unique index IDX_DBSESSIONINFO_1 on T_DBSESSIONINFO (
   nodip ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_DISKINFO                                            */
/*==============================================================*/
drop   sequence SEQ_DISKINFO;
create sequence SEQ_DISKINFO increment by 1 minvalue 1 nocycle;
create table T_DISKINFO  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   nodip              varchar2(30)                    not null,
   filesystem         varchar2(100)                   not null,
   mount              varchar2(100)                   not null,
   total              number(10,2)                    not null,
   used               number(10,2)                    not null,
   available          number(10,2)                    not null,
   usep               number(6,2)                    default 0 not null,
   alarmvalue         number(6)                      default 85 not null,
   alarmsts           number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   memo               varchar2(2000),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_DISKINFO is
'本表的数据由diskinfotodb程序插入和更新，WEB页面只维护任务名称和告警阀值字段。';

comment on column T_DISKINFO.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_DISKINFO.taskname is
'任务名称，后台程序用节点地址加文件系统填充本字段，无需界面维护。';

comment on column T_DISKINFO.nodip is
'IP地址';

comment on column T_DISKINFO.filesystem is
'文件系统，WINDOWS系统填分区符号，如C:、D:等，UNIX填设备号，如：/dev/sda、/dev/sdb等。';

comment on column T_DISKINFO.mount is
'挂载节点，WINDOWS系统填分区符号，如C:、D:等，UNIX填设备号，如：/root、/home等。';

comment on column T_DISKINFO.total is
'全部空间，单位G。';

comment on column T_DISKINFO.used is
'已用空间，单位G。';

comment on column T_DISKINFO.available is
'可用空间，单位G。';

comment on column T_DISKINFO.usep is
'已用比例';

comment on column T_DISKINFO.alarmvalue is
'告警阀值';

comment on column T_DISKINFO.alarmsts is
'告警状态，1-正常；2-告警中；3-已过期。';

comment on column T_DISKINFO.crttime is
'更新时间';

comment on column T_DISKINFO.memo is
'备注说明';

comment on column T_DISKINFO.rsts is
'记录状态，default 1。1-启用；2-禁用，该字段暂时不启用。';

alter table T_DISKINFO
   add constraint PK_DISKINFO primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_DISKINFO_1                                        */
/*==============================================================*/
create unique index IDX_DISKINFO_1 on T_DISKINFO (
   nodip ASC,
   filesystem ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_GDAWSDAYD                                           */
/*==============================================================*/
drop   sequence SEQ_AREAAWSDAYD;
create sequence SEQ_AREAAWSDAYD increment by 1 minvalue 1 nocycle;
create table T_GDAWSDAYD  (
   ddatetime          DATE                            not null,
   obtid              varchar2(10)                    not null,
   r0808              number(6),
   r2008              number(6),
   r0820              number(6),
   r24h               number(6),
   maxr10m            number(6),
   maxr10mtime        DATE,
   maxr01h            number(6),
   maxr01htime        DATE,
   maxt               number(6),
   maxttime           DATE,
   mint               number(6),
   minttime           DATE,
   avgt               number(6),
   maxrh              number(6),
   maxrhtime          date,
   minrh              number(6),
   minrhtime          DATE,
   avgrh              number(6),
   maxp0              number(6),
   maxp0time          DATE,
   minp0              number(6),
   minp0time          DATE,
   avgp0              number(6),
   maxp               number(6),
   maxptime           DATE,
   minp               number(6),
   minptime           DATE,
   avgp               number(6),
   wd2maxdf           number(6),
   wd2maxdd           number(6),
   wd2maxtime         DATE,
   avgwd2df           number(6),
   wd10maxdf          number(6),
   wd10maxdd          number(6),
   wd10maxtime        DATE,
   avgwd10df          number(6),
   mdcount            number(8),
   hdcount            number(8),
   keyid              number(15)                      not null,
   crttime            date                           default sysdate
);

comment on table T_GDAWSDAYD is
'该数据集是由pobtdayd程序从自动站分钟数据表（T_GDOBTMIND）、自动站整点数据表（T_GDOBTHOURD）和雨量数据表（T_GDOBTRAIND）中实时统计出来的。
注意：
（1）由于自动站原始数据不规范，错误比比皆是，该数据集在某些要素上的统计方法有争议，统计结果有分歧，不具备权威性，在使用的时候需慎重。
（2）在自动站分钟数据和整点数据表中，部分要素有日统计字段，但是，经验证，自动站分钟数据和整点数据表的日统计字段也不可靠，此表的数据仅参考。
三水不存在';

comment on column T_GDAWSDAYD.ddatetime is
'数据时间，这是一个时间型字段，只包括了日期，查询的时候，可以用to_date(ddatetime,''yyyymmdd'')方法把它转换为字符串。';

comment on column T_GDAWSDAYD.obtid is
'区站代码';

comment on column T_GDAWSDAYD.r0808 is
'昨08-今08时累计雨量，单位：0.1mm，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.r2008 is
'昨20-今08时累计雨量，单位：0.1mm，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.r0820 is
'今08-今20时累计雨量，单位：0.1mm，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.r24h is
'日雨量，昨20时至今20时的累计雨量，单位：0.1mm。
如果每日缺测的时次超过6次，按缺测处理。
注意，在2007年之前的自动站数据中，部分站点在无降水的时候把hourr字段置为空，没有和缺测情况区分开，目前在统计的时候把hourr字段为空的都视为缺测。
按王博的要求，统计缺测次数的时候，不包括字段hourr为空的记录。2010-08-30';

comment on column T_GDAWSDAYD.maxr10m is
'最大十分钟滑动雨量，单位：0.1mm。
从分钟数据表中挑出日最大的十分钟雨量并且雨量标志为准确的值。';

comment on column T_GDAWSDAYD.maxr10mtime is
'最大十分钟滑动雨量出现时间';

comment on column T_GDAWSDAYD.maxr01h is
'最大小时滑动雨量，单位：0.1mm。
从分钟数据表中挑出日最大的小时雨量并且雨量标志为准确的值。';

comment on column T_GDAWSDAYD.maxr01htime is
'最大小时滑动雨量出现时间';

comment on column T_GDAWSDAYD.maxt is
'最高气温，单位：0.1摄氏度。取一天中某自动站全部分钟记录的空气温度字段（t)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.maxttime is
'最高气温出现时间';

comment on column T_GDAWSDAYD.mint is
'最低气温，单位：0.1摄氏度。取一天中某自动站全部分钟记录的空气温度字段（t)的最小值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.minttime is
'最低气温出现时间';

comment on column T_GDAWSDAYD.avgt is
'平均气温，单位：0.1摄氏度。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.maxrh is
'最大湿度，一个百分比值，取值1到100。取一天中某自动站全部分钟记录的相对湿度字段（rh)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.maxrhtime is
'最大湿度出现时间';

comment on column T_GDAWSDAYD.minrh is
'最小湿度，一个百分比值，取值1到100。取一天中某自动站全部分钟记录的相对湿度字段（rh)的最小值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.minrhtime is
'最小湿度出现时间';

comment on column T_GDAWSDAYD.avgrh is
'平均湿度，一个百分比值。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.maxp0 is
'最高海面气压，单位：0.1百帕。取一天中某自动站全部分钟记录的海平面气压字段（p0)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.maxp0time is
'最高海面气压出现时间';

comment on column T_GDAWSDAYD.minp0 is
'最低海平面气压，单位：0.1百帕。取一天中某自动站全部记录的海平面气压字段（p0)的最小值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.minp0time is
'最低海平面气压出现时间';

comment on column T_GDAWSDAYD.avgp0 is
'平均海面气压，单位：0.1百帕。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.maxp is
'最高气压，单位：0.1百帕。取一天中某自动站全部分钟记录的本站气压字段（p)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.maxptime is
'最高气压出现时间';

comment on column T_GDAWSDAYD.minp is
'最低气压，单位：0.1百帕。取一天中某自动站全部分钟记录的本站气压字段（p)的最小值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.minptime is
'最低气压出现时间';

comment on column T_GDAWSDAYD.avgp is
'平均气压，单位：0.1百帕。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.wd2maxdf is
'最大两分钟风速，单位：0.1m/s。 取一天中某自动站全部分钟记录的两分钟风速字段（wd2df)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.wd2maxdd is
'最大两分钟风速风向';

comment on column T_GDAWSDAYD.wd2maxtime is
'最大两分钟风速出现时间';

comment on column T_GDAWSDAYD.avgwd2df is
'平均两分钟风速，单位：0.1m/s。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.wd10maxdf is
'最大十分钟风速，单位：0.1m/s。 取一天中某自动站全部分钟记录的十分钟风速字段（wd10df)的最大值，不考虑缺测的情况。';

comment on column T_GDAWSDAYD.wd10maxdd is
'最大十分钟风速风向';

comment on column T_GDAWSDAYD.wd10maxtime is
'最大十分钟风速出现时间';

comment on column T_GDAWSDAYD.avgwd10df is
'平均十分钟风速，单位：0.1m/s。
如果一日内没有缺测，则日平均为24次的平均，如果有缺测但02、08、14、20次没有缺测，则日平均按四次观测的统计；如果以上两个条件都不满足，当缺测次数小于等于6次时按实有次数统计，大于6次时按缺测处理。';

comment on column T_GDAWSDAYD.mdcount is
'分钟数据记录总数';

comment on column T_GDAWSDAYD.hdcount is
'整点数据记录总数';

comment on column T_GDAWSDAYD.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号';

comment on column T_GDAWSDAYD.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间，由数据中心创建该字段。';

alter table T_GDAWSDAYD
   add constraint PK_AREAAWSDAYD primary key (ddatetime, obtid);

/*==============================================================*/
/* Index: IDX_AREAAWSDAYD_1                                     */
/*==============================================================*/
create unique index IDX_AREAAWSDAYD_1 on T_GDAWSDAYD (
   obtid ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAAWSDAYD_2                                     */
/*==============================================================*/
create unique index IDX_AREAAWSDAYD_2 on T_GDAWSDAYD (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_AREAAWSDAYD_3                                     */
/*==============================================================*/
create index IDX_AREAAWSDAYD_3 on T_GDAWSDAYD (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAAWSDAYD_4                                     */
/*==============================================================*/
create index IDX_AREAAWSDAYD_4 on T_GDAWSDAYD (
   crttime ASC
);

/*==============================================================*/
/* Table: T_GDOBTHOURD                                          */
/*==============================================================*/
drop   sequence SEQ_AREAOBTHOURD;
create sequence SEQ_AREAOBTHOURD increment by 1 minvalue 1 nocycle;
create table T_GDOBTHOURD  (
   ddatetime          Date                            not null,
   obtid              varchar2(10)                    not null,
   station            number(2),
   baoformat          number(1),
   bps                number(6),
   ys                 varchar2(8),
   w3a                varchar2(6),
   wdidf              number(6),
   wdidd              number(6),
   wd2df              number(6),
   wd2dd              number(6),
   wd10df             number(6),
   wd10dd             number(6),
   wd3smaxdf          number(6),
   wd3smaxdd          number(6),
   wd3smaxtime        number(2),
   wd10maxdf          number(6),
   wd10maxdd          number(6),
   wd10maxtime        number(2),
   wd3daymax          number(6),
   wf3daymax          number(6),
   wd3daymaxtime      number(4),
   wd10daymax         number(6),
   wf10daymax         number(6),
   wd10daymaxtime     number(4),
   t                  number(6),
   maxt               number(6),
   maxttime           number(2),
   mint               number(6),
   minttime           number(2),
   daymaxt            number(6),
   daymaxttime        number(4),
   daymint            number(6),
   dayminttime        number(4),
   rh                 number(6),
   maxrh              number(6),
   maxrhtime          number(2),
   minrh              number(6),
   minrhtime          number(2),
   dp                 number(6),
   p                  number(6),
   maxp               number(6),
   maxptime           number(2),
   minp               number(6),
   minptime           number(2),
   p0                 number(6),
   hourrf             number(6),
   dayrf              number(6),
   rfmark1            number(6),
   minrf              varchar(50),
   wpv                number(6),
   othfields          varchar2(2000),
   rddatetime         number(2)                       not null,
   procsts            number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   keyid              number(15)                      not null
);

comment on table T_GDOBTHOURD is
'pobtdata程序读取T_GDOBTDATA表中procsts=1的记录，二次加工后写入本表。
本表存放的是某站点在同一整点时次中，最后一条到报的分钟数据，所以不一定是00分的数据。
rddatetime字段为当前记录采用的本小时内分钟数据的实际时间，整数，从1-59到00，表示本小时的分钟部分加1和下一小时的00分钟。';

comment on column T_GDOBTHOURD.ddatetime is
'数据时间';

comment on column T_GDOBTHOURD.obtid is
'区站代码';

comment on column T_GDOBTHOURD.station is
'自动站编号，数值为0-99等。';

comment on column T_GDOBTHOURD.baoformat is
'资料类型，0为正点报告，1 为瞬时报告。';

comment on column T_GDOBTHOURD.bps is
'测站高度，单位：0.1米。';

comment on column T_GDOBTHOURD.ys is
'自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
目前，共5个权位，1-降雨；2-风速；3-气温；4-湿度；5-气压；其它的权位还没有启用。
经研究，省局取回的数据中，不管是探测要素字段，还是数据质量字段，都不准确，不可用。
';

comment on column T_GDOBTHOURD.w3a is
'自动站型号';

comment on column T_GDOBTHOURD.wdidf is
'瞬时风速，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wdidd is
'瞬时风向，取值0到360。';

comment on column T_GDOBTHOURD.wd2df is
'二分钟风速，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd2dd is
'二分钟风向，取值0到360。';

comment on column T_GDOBTHOURD.wd10df is
'十分钟风速，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd10dd is
'十分钟风向，取值0到360。';

comment on column T_GDOBTHOURD.wd3smaxdf is
'极大风速，本小时内出现的最大瞬时风速值，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd3smaxdd is
'极大风向，极大风速的风向，取值0到360。';

comment on column T_GDOBTHOURD.wd3smaxtime is
'极大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.wd10maxdf is
'最大风速，本小时内出现的最大10分钟平均风速值，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd10maxdd is
'最大风向，最大风速的风向，取值0到360。';

comment on column T_GDOBTHOURD.wd10maxtime is
'最大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.wd3daymax is
'日瞬时极大风速时的风向，取值0到360。';

comment on column T_GDOBTHOURD.wf3daymax is
'日瞬时极大风速，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd3daymaxtime is
'日瞬时极大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTHOURD.wd10daymax is
'日十分钟最大风速时的风向。';

comment on column T_GDOBTHOURD.wf10daymax is
'日十分钟最大风速，单位：0.1m/s。';

comment on column T_GDOBTHOURD.wd10daymaxtime is
'日十分钟最大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTHOURD.t is
'空气温度，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.maxt is
'最高气温，本小时内的最高气温，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.maxttime is
'最高气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.mint is
'最低气温，本小时内的最低气温，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.minttime is
'最低气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.daymaxt is
'日最高温度，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.daymaxttime is
'日最高温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTHOURD.daymint is
'日最低温度，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.dayminttime is
'日最低温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTHOURD.rh is
'相对湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTHOURD.maxrh is
'最大湿度，本小时内的最大湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTHOURD.maxrhtime is
'最大湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.minrh is
'最小湿度，本小时内的最小湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTHOURD.minrhtime is
'最小湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.dp is
'露点温度，单位：0.1摄氏度。';

comment on column T_GDOBTHOURD.p is
'本站气压，单位：0.1百帕。';

comment on column T_GDOBTHOURD.maxp is
'最高气压，本小时内的最高气压，单位：0.1百帕。';

comment on column T_GDOBTHOURD.maxptime is
'最高气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.minp is
'最低气压，本小时内的最低气压，单位：0.1百帕。';

comment on column T_GDOBTHOURD.minptime is
'最低气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTHOURD.p0 is
'海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数，省港澳自动站还没有海拔高度参数。';

comment on column T_GDOBTHOURD.hourrf is
'小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。';

comment on column T_GDOBTHOURD.dayrf is
'日雨量累计，日界为：08:00-08:00,即每天的08:00清零。';

comment on column T_GDOBTHOURD.rfmark1 is
'当前一分钟雨量指示,没有说明。';

comment on column T_GDOBTHOURD.minrf is
'一小时的每五分钟的雨量分布。';

comment on column T_GDOBTHOURD.wpv is
'自动站电池电压，单位：0.01伏。';

comment on column T_GDOBTHOURD.othfields is
'其它要素，采用XML表达。';

comment on column T_GDOBTHOURD.rddatetime is
'实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启用。';

comment on column T_GDOBTHOURD.procsts is
'处理状态，default 1，1-未处理，2-已处理。';

comment on column T_GDOBTHOURD.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间。';

comment on column T_GDOBTHOURD.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号。';

alter table T_GDOBTHOURD
   add constraint PK_AREAOBTHOURD primary key (ddatetime, obtid);

/*==============================================================*/
/* Index: IDX_AREAOBTHOURD_1                                    */
/*==============================================================*/
create unique index IDX_AREAOBTHOURD_1 on T_GDOBTHOURD (
   obtid ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTHOURD_2                                    */
/*==============================================================*/
create unique index IDX_AREAOBTHOURD_2 on T_GDOBTHOURD (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTHOURD_3                                    */
/*==============================================================*/
create index IDX_AREAOBTHOURD_3 on T_GDOBTHOURD (
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTHOURD_4                                    */
/*==============================================================*/
create index IDX_AREAOBTHOURD_4 on T_GDOBTHOURD (
   crttime ASC
);

/*==============================================================*/
/* Table: T_GDOBTRAIND                                          */
/*==============================================================*/
drop   sequence SEQ_AREAOBTRAIND;
create sequence SEQ_AREAOBTRAIND increment by 1 minvalue 1 nocycle;
create table T_GDOBTRAIND  (
   ddatetime          Date                            not null,
   obtid              varchar2(10)                    not null,
   hourrf             number(6),
   minr               number(6),
   r05m               number(6),
   r10m               number(6),
   r30m               number(6),
   r01h               number(6),
   r02h               number(6),
   r03h               number(6),
   r06h               number(6),
   r12h               number(6),
   r24h               number(6),
   r48h               number(6),
   r72h               number(6),
   hdrbz              char(20),
   crttime            date                           default sysdate not null,
   keyid              number(15)                      not null
);

comment on table T_GDOBTRAIND is
'本表存放了自动站的滑动雨量数据
pobtdata程序读取T_GDOBTDATA表中procsts=1的记录，二次加工后写入本表。
注意，pobtdata程序只记录T_OBTCODE表中datacycle不为空，calhdr=1的站点。';

comment on column T_GDOBTRAIND.ddatetime is
'数据时间';

comment on column T_GDOBTRAIND.obtid is
'区站代码';

comment on column T_GDOBTRAIND.hourrf is
'小时雨量，单位：0.1mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。采用真实值';

comment on column T_GDOBTRAIND.minr is
'分钟雨量，单位：0.1mm，由本小时累计雨量计算出来的，用当前分钟的小时雨量减本小时上一分钟的小时雨量，如果本小时上一分钟的数据缺失，就用本小时上上一分钟的数据，如果本小时上上一分钟的数据缺失，就用本小时上上上一分钟的数据，采用真实值';

comment on column T_GDOBTRAIND.r05m is
'05分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近05分钟的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r10m is
'10分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近10分钟的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r30m is
'30分钟滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近30分钟的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r01h is
'01小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近01小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r02h is
'02小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近02小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r03h is
'03小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近03小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r06h is
'06小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近06小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r12h is
'12小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近12小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r24h is
'24小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近24小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r48h is
'48小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近48小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.r72h is
'72小时滑动雨量，单位：0.1mm，由本小时累计雨量计算出来的，表示该站点最近72小时的累计雨量。采用真实值';

comment on column T_GDOBTRAIND.hdrbz is
'滑动雨量标志，当数据缺失的时候，当前分钟的雨量和各时段滑动雨量的计算结果可能不准确，该字用于标记雨量计算结果是否准确，采用权位表达，一个权位表示一种雨量，1-准确，0-不准确，权位1-当前分钟雨量，权位2-06分钟滑动雨量，...权位12-72小时滑动雨量。';

comment on column T_GDOBTRAIND.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间。';

comment on column T_GDOBTRAIND.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号';

alter table T_GDOBTRAIND
   add constraint PK_AREAOBTRAIND primary key (ddatetime, obtid);

/*==============================================================*/
/* Index: IDX_AREAOBTRAIND_1                                    */
/*==============================================================*/
create unique index IDX_AREAOBTRAIND_1 on T_GDOBTRAIND (
   obtid ASC,
   ddatetime ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTRAIND_2                                    */
/*==============================================================*/
create unique index IDX_AREAOBTRAIND_2 on T_GDOBTRAIND (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_AREAOBTRAIND_3                                    */
/*==============================================================*/
create index IDX_AREAOBTRAIND_3 on T_GDOBTRAIND (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_GDOBTREALD                                          */
/*==============================================================*/
drop   sequence SEQ_AREAOBTREALD;
create sequence SEQ_AREAOBTREALD increment by 1 minvalue 1 nocycle;
create table T_GDOBTREALD  (
   obtid              varchar2(10)                    not null,
   ddatetime          Date                            not null,
   station            number(2),
   baoformat          number(1),
   bps                number(6),
   ys                 varchar2(8),
   w3a                varchar2(6),
   wdidf              number(6),
   wdidd              number(6),
   wd2df              number(6),
   wd2dd              number(6),
   wd10df             number(6),
   wd10dd             number(6),
   wd3smaxdf          number(6),
   wd3smaxdd          number(6),
   wd3smaxtime        number(2),
   wd10maxdf          number(6),
   wd10maxdd          number(6),
   wd10maxtime        number(2),
   wd3daymax          number(6),
   wf3daymax          number(6),
   wd3daymaxtime      number(4),
   wd10daymax         number(6),
   wf10daymax         number(6),
   wd10daymaxtime     number(4),
   t                  number(6),
   maxt               number(6),
   maxttime           number(2),
   mint               number(6),
   minttime           number(2),
   daymaxt            number(6),
   daymaxttime        number(4),
   daymint            number(6),
   dayminttime        number(4),
   rh                 number(6),
   maxrh              number(6),
   maxrhtime          number(2),
   minrh              number(6),
   minrhtime          number(2),
   dp                 number(6),
   p                  number(6),
   maxp               number(6),
   maxptime           number(2),
   minp               number(6),
   minptime           number(2),
   p0                 number(6),
   hourrf             number(6),
   dayrf              number(6),
   rfmark1            number(6),
   minrf              varchar(50),
   wpv                number(6),
   othfields          varchar2(2000),
   rddatetime         number(2)                       not null,
   procsts            number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   keyid              number(15)                      not null
);

comment on table T_GDOBTREALD is
'本表存放了自动站的最近的数据，每个自动站在本表中只有一条记录。
pobtdata程序读取T_GDOBTDATA表中procsts=1的记录，二次加工后写入本表。';

comment on column T_GDOBTREALD.obtid is
'区站代码';

comment on column T_GDOBTREALD.ddatetime is
'数据时间';

comment on column T_GDOBTREALD.station is
'自动站编号，数值为0-99等。';

comment on column T_GDOBTREALD.baoformat is
'资料类型，0为正点报告，1 为瞬时报告。';

comment on column T_GDOBTREALD.bps is
'测站高度，单位：0.1米。';

comment on column T_GDOBTREALD.ys is
'自动站探测项目,标志自动站是否有某气象要素的探测，采用权位表示：0-没有，1-有，2-故障。
目前，共5个权位，1-降雨；2-风速；3-气温；4-湿度；5-气压；其它的权位还没有启用。
经研究，省局取回的数据中，不管是探测要素字段，还是数据质量字段，都不准确，不可用。
';

comment on column T_GDOBTREALD.w3a is
'自动站型号';

comment on column T_GDOBTREALD.wdidf is
'瞬时风速，单位：0.1m/s。';

comment on column T_GDOBTREALD.wdidd is
'瞬时风向，取值0到360。';

comment on column T_GDOBTREALD.wd2df is
'二分钟风速，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd2dd is
'二分钟风向，取值0到360。';

comment on column T_GDOBTREALD.wd10df is
'十分钟风速，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd10dd is
'十分钟风向，取值0到360。';

comment on column T_GDOBTREALD.wd3smaxdf is
'极大风速，本小时内出现的最大瞬时风速值，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd3smaxdd is
'极大风向，极大风速的风向，取值0到360。';

comment on column T_GDOBTREALD.wd3smaxtime is
'极大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.wd10maxdf is
'最大风速，本小时内出现的最大10分钟平均风速值，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd10maxdd is
'最大风向，最大风速的风向，取值0到360。';

comment on column T_GDOBTREALD.wd10maxtime is
'最大风速出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.wd3daymax is
'日瞬时极大风速时的风向，取值0到360。';

comment on column T_GDOBTREALD.wf3daymax is
'日瞬时极大风速，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd3daymaxtime is
'日瞬时极大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTREALD.wd10daymax is
'日十分钟最大风速时的风向。';

comment on column T_GDOBTREALD.wf10daymax is
'日十分钟最大风速，单位：0.1m/s。';

comment on column T_GDOBTREALD.wd10daymaxtime is
'日十分钟最大风速出现时间，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTREALD.t is
'空气温度，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.maxt is
'最高气温，本小时内的最高气温，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.maxttime is
'最高气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.mint is
'最低气温，本小时内的最低气温，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.minttime is
'最低气温出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.daymaxt is
'日最高温度，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.daymaxttime is
'日最高温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTREALD.daymint is
'日最低温度，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.dayminttime is
'日最低温度出现时间,用时分表示，用时分表示，如：2230为22时30分。';

comment on column T_GDOBTREALD.rh is
'相对湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTREALD.maxrh is
'最大湿度，本小时内的最大湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTREALD.maxrhtime is
'最大湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.minrh is
'最小湿度，本小时内的最小湿度，一个百分比值，取值1到100。';

comment on column T_GDOBTREALD.minrhtime is
'最小湿度出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.dp is
'露点温度，单位：0.1摄氏度。';

comment on column T_GDOBTREALD.p is
'本站气压，单位：0.1百帕。';

comment on column T_GDOBTREALD.maxp is
'最高气压，本小时内的最高气压，单位：0.1百帕。';

comment on column T_GDOBTREALD.maxptime is
'最高气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.minp is
'最低气压，本小时内的最低气压，单位：0.1百帕。';

comment on column T_GDOBTREALD.minptime is
'最低气压出现时间，整数，从0-59，只保留了时间中的分钟部分。';

comment on column T_GDOBTREALD.p0 is
'海平面气压，单位：百帕，海平台气压需要根据自动站安装的海拔高度参数计算，目前，深圳市自动站只有一部分站点有海拔高度参数，省港澳自动站还没有海拔高度参数。';

comment on column T_GDOBTREALD.hourrf is
'小时雨量，单位：mm，本小时的累计雨量，从该小时的01分开始累积到下一小时的00分，下一小时00分后的时候清零。';

comment on column T_GDOBTREALD.dayrf is
'日雨量累计，日界为：08:00-08:00,即每天的08:00清零。';

comment on column T_GDOBTREALD.rfmark1 is
'当前一分钟雨量指示,没有说明。';

comment on column T_GDOBTREALD.minrf is
'一小时的每五分钟的雨量分布。';

comment on column T_GDOBTREALD.wpv is
'自动站电池电压，单位：0.01伏。';

comment on column T_GDOBTREALD.othfields is
'其它要素，采用XML表达。';

comment on column T_GDOBTREALD.rddatetime is
'实际时间，当前记录采用的本小时内数据的实际时间，整数，从1-59、00，表示本小时的分钟部分加1和下一小时的00分钟，该字段不启用。';

comment on column T_GDOBTREALD.procsts is
'处理状态，default 1，1-未处理，2-已处理。';

comment on column T_GDOBTREALD.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间。';

comment on column T_GDOBTREALD.keyid is
'记录序号，从序与本表同名的序列中取值，数据中心用于同步的编号。';

alter table T_GDOBTREALD
   add constraint PK_AREAOBTREALD primary key (obtid);

/*==============================================================*/
/* Table: T_INDEXINFO                                           */
/*==============================================================*/
create table T_INDEXINFO  (
   obtid              char(5)                         not null,
   obtname            varchar2(30)                    not null,
   jrrc               varchar2(5),
   jrrl               varchar2(5),
   mrrc               varchar2(5),
   mrrl               varchar2(5),
   jryc               varchar2(6),
   jryl               varchar2(6),
   mryc               varchar2(6),
   mryl               varchar2(6),
   pubtime            DATE
);

comment on table T_INDEXINFO is
'目前，本表中只有日出日落数据，其它生活指数字段都没有值。';

comment on column T_INDEXINFO.obtid is
'站点代码';

comment on column T_INDEXINFO.obtname is
'站点名称，一般用城市的名称。';

comment on column T_INDEXINFO.jrrc is
'今日日出';

comment on column T_INDEXINFO.jrrl is
'今日日落';

comment on column T_INDEXINFO.mrrc is
'明日日出';

comment on column T_INDEXINFO.mrrl is
'明日日落';

comment on column T_INDEXINFO.jryc is
'今日月出';

comment on column T_INDEXINFO.jryl is
'今日月落';

comment on column T_INDEXINFO.mryc is
'明日月出';

comment on column T_INDEXINFO.mryl is
'明日月落';

comment on column T_INDEXINFO.pubtime is
'计算时间';

alter table T_INDEXINFO
   add constraint PK_INDEXINFO primary key (obtid);

/*==============================================================*/
/* Table: T_MEMINFO                                             */
/*==============================================================*/
drop   sequence SEQ_MEMINFO;
create sequence SEQ_MEMINFO increment by 1 minvalue 1 nocycle;
create table T_MEMINFO  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   nodip              varchar2(30)                    not null,
   memtotal           number(10)                     default 0,
   memused            number(10)                     default 0,
   memfree            number(10)                     default 0,
   memusep            number(6,2)                    default 0 not null,
   alarmvalue         number(6)                      default 100 not null,
   alarmsts           number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   memo               varchar2(2000),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_MEMINFO is
'本表的数据由meminfotodb程序插入和更新，WEB页面只维护任务名称和告警阀值字段。';

comment on column T_MEMINFO.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_MEMINFO.taskname is
'任务名称，后台程序用节点地址填充本字段，无需界面维护。';

comment on column T_MEMINFO.nodip is
'IP地址';

comment on column T_MEMINFO.memtotal is
'总内存，单位：M。';

comment on column T_MEMINFO.memused is
'已使用内存，单位：M。';

comment on column T_MEMINFO.memfree is
'未使用内存，单位：M。';

comment on column T_MEMINFO.memusep is
'已用比例';

comment on column T_MEMINFO.alarmvalue is
'告警阀值';

comment on column T_MEMINFO.alarmsts is
'告警状态，1-正常；2-告警中；3-已过期。';

comment on column T_MEMINFO.crttime is
'更新时间';

comment on column T_MEMINFO.memo is
'备注说明';

comment on column T_MEMINFO.rsts is
'记录状态，default 1。1-启用；2-禁用，该字段暂时不启用。';

alter table T_MEMINFO
   add constraint PK_MEMINFO primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_MEMINFO_1                                         */
/*==============================================================*/
create unique index IDX_MEMINFO_1 on T_MEMINFO (
   nodip ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_NETCHECKCFG                                         */
/*==============================================================*/
drop   sequence SEQ_NETCHECKCFG;
create sequence SEQ_NETCHECKCFG increment by 1 minvalue 1 nocycle;
create table T_NETCHECKCFG  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   serverip           varchar2(30)                    not null,
   pcount             number(6)                       not null,
   loss               number(6,2),
   dtime              number(6),
   exectvl            number(10)                     default 5 not null,
   exectime           date,
   checkingbz         number(1)                      default 1 not null,
   alarmsts           number(1)                      default 1 not null,
   memo               varchar2(2000),
   orderby            number(6),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_NETCHECKCFG is
'本表配置参数，后台的netcheckserver程序读取本表的参数，向对方ping若干包，如果不能或有丢包，就发出告警。';

comment on column T_NETCHECKCFG.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_NETCHECKCFG.taskname is
'任务名称';

comment on column T_NETCHECKCFG.serverip is
'IP地址';

comment on column T_NETCHECKCFG.pcount is
'发包数';

comment on column T_NETCHECKCFG.loss is
'丢包率';

comment on column T_NETCHECKCFG.dtime is
'延时';

comment on column T_NETCHECKCFG.exectvl is
'检查周期，单位：分钟。';

comment on column T_NETCHECKCFG.exectime is
'最近检查时间';

comment on column T_NETCHECKCFG.checkingbz is
'检查标志，1-未检查；2-检查中。';

comment on column T_NETCHECKCFG.alarmsts is
'告警状态，1-正常；2-告警中。';

comment on column T_NETCHECKCFG.memo is
'备注说明';

comment on column T_NETCHECKCFG.orderby is
'显示顺序，暂时不启用。';

comment on column T_NETCHECKCFG.rsts is
'记录状态，default 1。1-启用；2-禁用';

alter table T_NETCHECKCFG
   add constraint PK_NETCHECKCFG primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_NETCHECKCFG_1                                     */
/*==============================================================*/
create unique index IDX_NETCHECKCFG_1 on T_NETCHECKCFG (
   taskname ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_OBTCODE                                             */
/*==============================================================*/
create table T_OBTCODE  (
   obtid              char(5)                         not null,
   obtname            varchar2(30)                    not null,
   provname           varchar2(30)                    not null,
   lat                number(8,2),
   lon                number(8,2),
   height             number(8,2),
   rsts               number(1)                      default 1 not null
      constraint CKC_RSTS_T_OBTCOD check (rsts in (1,2,3))
);

comment on table T_OBTCODE is
'该表存放了全国气象站点参数，2019年9月有839个站点。';

comment on column T_OBTCODE.obtid is
'站点代码';

comment on column T_OBTCODE.obtname is
'站点名称，一般用城市的名称。';

comment on column T_OBTCODE.provname is
'所属省，采用省的简称。';

comment on column T_OBTCODE.lat is
'纬度';

comment on column T_OBTCODE.lon is
'经度';

comment on column T_OBTCODE.height is
'高度，指气压传感器安装的海拔高度。';

comment on column T_OBTCODE.rsts is
'状态，1-正常，2-禁用，3-故障。';

alter table T_OBTCODE
   add constraint PK_OBTCODE primary key (obtid);

/*==============================================================*/
/* Table: T_SHRADFILE                                           */
/*==============================================================*/
drop   sequence SEQ_SHRADFILE;
create sequence SEQ_SHRADFILE increment by 1 minvalue 1 nocycle;
create table T_SHRADFILE  (
   filename           varchar2(200)                   not null,
   ddatetime          date                            not null,
   filesize           number(10)                      not null,
   filecontent        BLOB,
   crttime            date                           default sysdate,
   keyid              number(15)                      not null
);

comment on column T_SHRADFILE.filename is
'文件名称';

comment on column T_SHRADFILE.ddatetime is
'数据时间，文件中的数据所属的时间，例如雷达图文件MOC2008123010600.GIF，表示该图片是2008123010600的雷达扫描数据，与该文件的处理时间或入库时间没有关系。';

comment on column T_SHRADFILE.filesize is
'文件大小，单位：字节。';

comment on column T_SHRADFILE.filecontent is
'文件内容';

comment on column T_SHRADFILE.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间，由数据中心创建该字段。';

comment on column T_SHRADFILE.keyid is
'记录序号，从序列与本表同名的序列生成器中获取，数据中心用于同步的编号。';

alter table T_SHRADFILE
   add constraint PK_SHRADFILE primary key (filename);

/*==============================================================*/
/* Index: IDX_SHRADFILE_1                                       */
/*==============================================================*/
create unique index IDX_SHRADFILE_1 on T_SHRADFILE (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_SHRADFILE_2                                       */
/*==============================================================*/
create index IDX_SHRADFILE_2 on T_SHRADFILE (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_SIGNALLOG                                           */
/*==============================================================*/
drop sequence SEQ_SIGNALLOG;
create sequence SEQ_SIGNALLOG minvalue 1;
create table T_SIGNALLOG  (
   obtid              char(4)                         not null,
   ddatetime          date                            not null,
   signalname         char(1)                         not null,
   signalcolor        char(1),
   crttime            date                           default sysdate not null,
   keyid              number(6)                       not null
);

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

alter table T_SIGNALLOG
   add constraint PK_SIGNALLOG primary key (obtid, ddatetime, signalname);

/*==============================================================*/
/* Index: IDX_SIGNALLOG_1                                       */
/*==============================================================*/
create unique index IDX_SIGNALLOG_1 on T_SIGNALLOG (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_SIGNALLOG_2                                       */
/*==============================================================*/
create index IDX_SIGNALLOG_2 on T_SIGNALLOG (
   obtid ASC
);

/*==============================================================*/
/* Index: IDX_SIGNALLOG_3                                       */
/*==============================================================*/
create index IDX_SIGNALLOG_3 on T_SIGNALLOG (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_SURFDATA                                            */
/*==============================================================*/
drop sequence SEQ_SURFDATA;
create sequence SEQ_SURFDATA minvalue 1;
create table T_SURFDATA  (
   obtid              char(5),
   ddatetime          date                            not null,
   t                  number(6),
   p                  number(6),
   u                  number(6),
   wd                 number(6),
   wf                 number(6),
   r                  number(6),
   vis                number(6),
   crttime            date                           default sysdate not null,
   keyid              number(6)                       not null
);

comment on table T_SURFDATA is
'该表存放了全国气象站点分钟数据，数据从2017年1月1日开始，约839个站点。';

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
'入库时间，default sysdate。';

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
/* Table: T_SYSARG                                              */
/*==============================================================*/
create table T_SYSARG  (
   argcode            varchar2(50)                    not null,
   argname            varchar2(50)                    not null,
   argvalue           varchar2(300)                   not null
);

comment on table T_SYSARG is
'存放系统运行的相关参数。';

comment on column T_SYSARG.argcode is
'参数代码';

comment on column T_SYSARG.argname is
'参数名称';

comment on column T_SYSARG.argvalue is
'参数值';

alter table T_SYSARG
   add constraint PK_SYSARG primary key (argcode);

/*==============================================================*/
/* Table: T_TBSPACEINFO                                         */
/*==============================================================*/
drop   sequence SEQ_TBSPACEINFO;
create sequence SEQ_TBSPACEINFO increment by 1 minvalue 1 nocycle;
create table T_TBSPACEINFO  (
   taskid             number(10)                      not null,
   taskname           varchar2(300)                   not null,
   nodip              varchar2(30)                    not null,
   tablespace         varchar2(100)                   not null,
   total              number(10,2)                    not null,
   used               number(10,2)                    not null,
   available          number(10,2)                    not null,
   usep               number(6,2)                     not null,
   alarmvalue         number(6)                      default 85 not null,
   alarmsts           number(1)                      default 1 not null,
   crttime            date                           default sysdate not null,
   memo               varchar2(2000),
   rsts               number(1)                      default 1 not null
)
tablespace users;

comment on table T_TBSPACEINFO is
'本表的数据由tbspaceinfotodb程序插入和更新，WEB页面只维护任务名称和告警阀值字段。';

comment on column T_TBSPACEINFO.taskid is
'任务代码，从与本表同名的序列生成器中获取。';

comment on column T_TBSPACEINFO.taskname is
'任务名称，后台程序用节点地址加表空间名填充本字段，无需界面维护。';

comment on column T_TBSPACEINFO.nodip is
'IP地址';

comment on column T_TBSPACEINFO.tablespace is
'表空间';

comment on column T_TBSPACEINFO.total is
'总空间，单位G。';

comment on column T_TBSPACEINFO.used is
'已用空间，单位G。';

comment on column T_TBSPACEINFO.available is
'可用空间，单位G。';

comment on column T_TBSPACEINFO.usep is
'已用比例';

comment on column T_TBSPACEINFO.alarmvalue is
'告警阀值';

comment on column T_TBSPACEINFO.alarmsts is
'告警状态，1-正常；2-告警中；3-已过期。';

comment on column T_TBSPACEINFO.crttime is
'更新时间';

comment on column T_TBSPACEINFO.memo is
'备注说明';

comment on column T_TBSPACEINFO.rsts is
'记录状态，default 1。1-启用；2-禁用，该字段暂时不启用。';

alter table T_TBSPACEINFO
   add constraint PK_TBSPACEINFO primary key (taskid)
      using index tablespace users;

/*==============================================================*/
/* Index: IDX_TBSPACEINFO_1                                     */
/*==============================================================*/
create unique index IDX_TBSPACEINFO_1 on T_TBSPACEINFO (
   nodip ASC,
   tablespace ASC
)
tablespace users;

/*==============================================================*/
/* Table: T_ZHRAIN24                                            */
/*==============================================================*/
drop   sequence SEQ_ZHRAIN24;
create sequence SEQ_ZHRAIN24 increment by 1 minvalue 1 nocycle;
create table T_ZHRAIN24  (
   filename           varchar2(200)                   not null,
   ddatetime          date                            not null,
   filesize           number(10)                      not null,
   filecontent        BLOB,
   crttime            date                           default sysdate,
   keyid              number(15)                      not null
);

comment on column T_ZHRAIN24.filename is
'文件名称，用全路径。';

comment on column T_ZHRAIN24.ddatetime is
'数据时间，文件中的数据所属的时间，例如雷达图文件MOC2008123010600.GIF，表示该图片是2008123010600的雷达扫描数据，与该文件的处理时间或入库时间没有关系。';

comment on column T_ZHRAIN24.filesize is
'文件大小，单位：字节。';

comment on column T_ZHRAIN24.filecontent is
'文件内容';

comment on column T_ZHRAIN24.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间，由数据中心创建该字段。';

comment on column T_ZHRAIN24.keyid is
'记录序号，从序列与本表同名的序列生成器中获取，数据中心用于同步的编号。';

alter table T_ZHRAIN24
   add constraint PK_ZHRAIN24 primary key (filename);

/*==============================================================*/
/* Index: IDX_ZHRAIN24_1                                        */
/*==============================================================*/
create unique index IDX_ZHRAIN24_1 on T_ZHRAIN24 (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_ZHRAIN24_2                                        */
/*==============================================================*/
create index IDX_ZHRAIN24_2 on T_ZHRAIN24 (
   ddatetime ASC
);

/*==============================================================*/
/* Table: T_ZHTEMP24                                            */
/*==============================================================*/
drop   sequence SEQ_ZHTEMP24;
create sequence SEQ_ZHTEMP24 increment by 1 minvalue 1 nocycle;
create table T_ZHTEMP24  (
   filename           varchar2(200)                   not null,
   ddatetime          date                            not null,
   filesize           number(10)                      not null,
   filecontent        BLOB,
   crttime            date                           default sysdate,
   keyid              number(15)                      not null
);

comment on column T_ZHTEMP24.filename is
'文件名称';

comment on column T_ZHTEMP24.ddatetime is
'数据时间，文件中的数据所属的时间，例如雷达图文件MOC2008123010600.GIF，表示该图片是2008123010600的雷达扫描数据，与该文件的处理时间或入库时间没有关系。';

comment on column T_ZHTEMP24.filesize is
'文件大小，单位：字节。';

comment on column T_ZHTEMP24.filecontent is
'文件内容';

comment on column T_ZHTEMP24.crttime is
'入库时间，default sysdate，数据被插入或更新本数据库表的时间，由数据中心创建该字段。';

comment on column T_ZHTEMP24.keyid is
'记录序号，从序列与本表同名的序列生成器中获取，数据中心用于同步的编号。';

alter table T_ZHTEMP24
   add constraint PK_ZHTEMP24 primary key (filename);

/*==============================================================*/
/* Index: IDX_ZHTEMP24_1                                        */
/*==============================================================*/
create unique index IDX_ZHTEMP24_1 on T_ZHTEMP24 (
   keyid ASC
);

/*==============================================================*/
/* Index: IDX_ZHTEMP24_2                                        */
/*==============================================================*/
create index IDX_ZHTEMP24_2 on T_ZHTEMP24 (
   ddatetime ASC
);

alter table T_SURFDATA
   add constraint FK_SURFDA_OBTCODE foreign key (obtid)
      references T_OBTCODE (obtid)
      on delete set null;

exit;
