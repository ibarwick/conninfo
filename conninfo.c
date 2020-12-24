/*
 * conninfo.c
 *
 */

#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "utils/builtins.h"


#include "libpq-fe.h"
PG_MODULE_MAGIC;

void _PG_init(void);

Datum conninfo_parse(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(conninfo_parse);

Datum conninfo_defaults(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(conninfo_defaults);

Datum conninfo_validate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(conninfo_validate);


/*
 * conninfo_parse()
 *
 * Basically a wrapper for PQconninfoParse()
 */
Datum conninfo_parse(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;

	const char *conninfo = NULL;
	PQconninfoOption *conninfo_options = NULL;
	PQconninfoOption *conninfo_option = NULL;
	char       *err = NULL;

	Datum		values[2];
	bool		nulls[2];

	/* check to see if caller supports this function returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));


	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	conninfo = text_to_cstring(PG_GETARG_TEXT_PP(0));

	conninfo_options = PQconninfoParse(conninfo, &err);

	if (conninfo_options == NULL)
	{
		ereport(ERROR,
				(errmsg("unable to parse provided conninfo string \"%s\"",
					   conninfo),
				 errdetail("%s", err)));

		PG_RETURN_NULL();
	}

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	/* Build a tuple descriptor for function's result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	/* We don't currently return any NULL values */
	memset(nulls, 0, sizeof(nulls));

	for (conninfo_option = conninfo_options; conninfo_option->keyword != NULL; conninfo_option++)
	{
		/* This probably will never happen, but just in case */
		if (conninfo_option->val == NULL)
			continue;

		memset(values, 0, sizeof(values));

		values[0] = CStringGetTextDatum(conninfo_option->keyword);
		values[1] = CStringGetTextDatum(conninfo_option->val);

		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	}

	PQconninfoFree(conninfo_options);

	/* no more rows */
	tuplestore_donestoring(tupstore);

	return (Datum) 0;
}


/*
 * conninfo_defaults()
 *
 * Basically a wrapper for PQconninfodefaults()
 */
Datum conninfo_defaults(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;

	PQconninfoOption *defs = NULL;
	PQconninfoOption *def = NULL;

	Datum		values[7];
	bool		nulls[7];

	/* check to see if caller supports this function returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	/* Build a tuple descriptor for function's result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);


	defs = PQconndefaults();

	for (def = defs; def->keyword; def++)
	{
		memset(values, 0, sizeof(values));
		memset(nulls, 0, sizeof(nulls));

		values[0] = CStringGetTextDatum(def->keyword);

		if (def->envvar == NULL)
			nulls[1] = true;
		else
			values[1] = CStringGetTextDatum(def->envvar);

		if (def->compiled == NULL)
			nulls[2] = true;
		else
			values[2] = CStringGetTextDatum(def->compiled);


		if (def->val == NULL)
			nulls[3] = true;
		else
			values[3] = CStringGetTextDatum(def->val);

		if (def->label == NULL)
			nulls[4] = true;
		else
			values[4] = CStringGetTextDatum(def->label);

		if (def->dispchar == NULL)
			nulls[5] = true;
		else
			values[5] = CStringGetTextDatum(def->dispchar);

		values[6] = Int32GetDatum(def->dispsize);
		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	}

	PQconninfoFree(defs);

	/* no more rows */
	tuplestore_donestoring(tupstore);

	return (Datum) 0;
}


Datum
conninfo_validate(PG_FUNCTION_ARGS)
{
	const char *conninfo = NULL;
	PQconninfoOption *conninfo_options = NULL;
	char       *err = NULL;

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	conninfo = text_to_cstring(PG_GETARG_TEXT_PP(0));

	conninfo_options = PQconninfoParse(conninfo, &err);

	if (conninfo_options == NULL)
	{
		ereport(WARNING,
				(errmsg("unable to parse provided conninfo string \"%s\"",
					   conninfo),
				 errdetail("%s", err)));

		PG_RETURN_BOOL(false);
	}

	PG_RETURN_BOOL(true);
}
