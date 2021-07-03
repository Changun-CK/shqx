--把数据库用户允许错误重试的次数改为不限制
alter profile DEFAULT limit FAILED_LOGIN_ATTEMPTS UNLIMITED;
alter profile DEFAULT limit PASSWORD_LIFE_TIME  UNLIMITED;

-- 上海气象数据中心的主用户
-- drop user shqx cascade;
create user shqx profile default identified by pwdidc default tablespace users account unlock;
grant connect to shqx;
grant dba to shqx;

exit;
