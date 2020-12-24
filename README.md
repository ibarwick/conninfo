conninfo
========

**conninfo** is a PostgreSQL extension providing some utility functions for
parsing `conninfo` strings.

This extension is made available under the same license as PostgreSQL.

Functions
---------

**conninfo** provides the following functions:

- **conninfo_parse()**

  Parses the provided conninfo string into a set of `keyword`/`value` records, e.g.:

      postgres=# SELECT * FROM conninfo_parse('host=node1 user=foo');
       keyword | value
      ---------+-------
       user    | foo
       host    | node1
      (2 rows)

  or:

      postgres=# SELECT * FROM conninfo_parse('postgresql://localhost:5433');
       keyword |   value
      ---------+-----------
       host    | localhost
       port    | 5433
      (2 rows)


- **conninfo_validate()**

  Validates the keywords in the provided `conninfo` string. Note this does not
  sanity-check the provided values, merely verifies that the string can be parsed.

      postgres=# SELECT conninfo_validate('foo=bar');
      WARNING:  unable to parse provided conninfo string "foo=bar"
      DETAIL:  invalid connection option "foo"

       conninfo_validate
      -------------------
       f
      (1 row)

      postgres=# SELECT conninfo_validate('host=foo user=bar');
       conninfo_validate
      -------------------
       t
      (1 row)


- **conninfo_defaults()**

  Returns the default values, as reported by `libpq` function `PQconninfodefaults()`,
  e.g.:

      postgres=# SELECT * FROM conninfo_defaults() LIMIT 5;
           keyword     |      envvar       | compiled | value |       label       | dispchar | dispsize
      -----------------+-------------------+----------+-------+-------------------+----------+----------
       authtype        | PGAUTHTYPE        |          |       | Database-Authtype | D        |       20
       service         | PGSERVICE         |          |       | Database-Service  |          |       20
       user            | PGUSER            |          | ian   | Database-User     |          |       20
       password        | PGPASSWORD        |          |       | Database-Password | *        |       20
       connect_timeout | PGCONNECT_TIMEOUT |          |       | Connect-timeout   |          |       10
      (5 rows)


Useful links
------------

- libpq documentation: https://www.postgresql.org/docs/current/libpq-connect.html
