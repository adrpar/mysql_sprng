CREATE FUNCTION sprng_make_seed RETURNS INTEGER SONAME 'mysql_sprng_UDF.so';
CREATE FUNCTION sprng_dbl RETURNS REAL SONAME 'mysql_sprng_UDF.so';
CREATE FUNCTION sprng_int RETURNS INTEGER SONAME 'mysql_sprng_UDF.so';

INSTALL PLUGIN sprng SONAME 'mysql_sprng_UDF.so';

