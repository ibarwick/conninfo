/* conninfo/conninfo--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION conninfo" to load this file.\quit


CREATE FUNCTION conninfo_parse(
    IN  conninfo TEXT,
    OUT keyword TEXT,
    OUT value TEXT
  )
  RETURNS SETOF record
  LANGUAGE C
AS 'MODULE_PATHNAME', 'conninfo_parse';


CREATE FUNCTION conninfo_validate(
    IN  conninfo TEXT
  )
  RETURNS BOOL
  LANGUAGE C
AS 'MODULE_PATHNAME', 'conninfo_validate';


CREATE FUNCTION conninfo_defaults(
    OUT keyword TEXT,
    OUT envvar TEXT,
    OUT compiled TEXT,
    OUT value TEXT,
    OUT label TEXT,
    OUT dispchar TEXT,
    OUT dispsize INT
  )
  RETURNS SETOF record
  LANGUAGE C
AS 'MODULE_PATHNAME', 'conninfo_defaults';
