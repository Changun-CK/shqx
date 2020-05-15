drop table t_surfdata;
drop table t_obtcode;

create table t_surfdata
(
    obtid     char(5)    not null,
    ddatetime date       not null,
    t         number(4)      null,
    p         number(5)      null,
    u         number(3)      null,
    wd        number(3)      null,
    wf        number(4)      null,
    r         number(4)      null,
    vis       number(6)      null,
	keyid     number(15) not null,
	crttime   date       not null,
    primary   key(obtid, ddatetime)
);

create table t_obtcode
(
	obtid        char(5),
	obtname      varchar2(30),
	provname     varchar2(30),
	lat          number(5, 2),
	lon          number(5, 2),
	height       number(8, 2),
	rsts         number(1),     -- 1-启用, 2-禁用, 3-故障
	primary key(obtid)
);

drop sequence seq_surfdata;
create sequence seq_surfdata increment by 1 start with 1;

create index idx_tsurfdata_1 on t_surfdata(ddatetime);
create index idx_tsurfdata_2 on t_surfdata(obtid);
create unique index idx_tsurfdata_3 on t_surfdata(keyid);

exit();
