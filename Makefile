# See:
#   https://www.postgresql.org/docs/current/extend-pgxs.html
# for all variables

EXTENSION   = conninfo
EXTVERSION 	= 1.0

MODULE_big = $(EXTENSION)

PG_CPPFLAGS = -std=gnu99 -I$(includedir_internal) -I$(libpq_srcdir) -Wall -Wmissing-prototypes -Wmissing-declarations $(EXTRA_CFLAGS)
SHLIB_LINK += $(libpq)

OBJS = conninfo.o

DATA = conninfo--1.0.sql

TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --outputdir=test \
	--load-extension=$(EXTENSION)

PG_CONFIG = pg_config

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
