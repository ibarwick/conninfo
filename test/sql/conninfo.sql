/*
 * Regression test for conninfo
 */

-- conninfo_parse()

SELECT * FROM conninfo_parse('host=foo user=bar');
SELECT * FROM conninfo_parse('foo=bar');
SELECT * FROM conninfo_parse('postgresql://localhost:5433');
SELECT * FROM conninfo_parse('mysql://localhost:5433');


-- conninfo_validate()

SELECT conninfo_validate('host=foo user=bar');
SELECT conninfo_validate('foo=bar');
SELECT conninfo_validate('postgresql://localhost:5433');
SELECT conninfo_validate('mysql://localhost:5433');

-- conninfo_defaults()
--
-- Note: the output will vary between PostgreSQL versions, so we select
-- some rows which should be the same in all versions.

SELECT * FROM conninfo_defaults() WHERE keyword IN ('application_name','dbname','host','hostaddr','password','replication') ORDER BY keyword;

