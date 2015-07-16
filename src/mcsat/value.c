/*
 * value.c
 *
 *  Created on: May 5, 2014
 *      Author: dejan
 */

#include "mcsat/value.h"

#include <assert.h>

#include "utils/memalloc.h"

const mcsat_value_t mcsat_value_none = { VALUE_NONE, { true } };
const mcsat_value_t mcsat_value_true = { VALUE_BOOLEAN, { true } };
const mcsat_value_t mcsat_value_false = { VALUE_BOOLEAN, { false } };

void mcsat_value_construct_default(mcsat_value_t* value) {
  value->type = VALUE_NONE;
}

void mcsat_value_construct_bool(mcsat_value_t* value, bool b) {
  value->type = VALUE_BOOLEAN;
  value->b = b;
}

void mcsat_value_construct_rational(mcsat_value_t* value, const rational_t* q) {
  value->type = VALUE_RATIONAL;
  q_init(&value->q);
  q_set(&value->q, q);
}

void mcsat_value_construct_lp_value(mcsat_value_t* value, const lp_value_t* lp_value) {
  value->type = VALUE_LIBPOLY;
  lp_value_construct_copy(&value->lp_value, lp_value);
}

void mcsat_value_construct_copy(mcsat_value_t* value, const mcsat_value_t* from) {
  value->type = from->type;
  switch (value->type) {
  case VALUE_NONE:
    break;
  case VALUE_BOOLEAN:
    value->b = from->b;
    break;
  case VALUE_RATIONAL:
    q_init(&value->q);
    q_set(&value->q, &from->q);
    break;
  case VALUE_LIBPOLY:
    lp_value_construct_copy(&value->lp_value, &from->lp_value);
    break;
  default:
    assert(false);
  }
}

void mcsat_value_destruct(mcsat_value_t* value) {
  switch (value->type) {
  case VALUE_NONE:
    break;
  case VALUE_BOOLEAN:
    break;
  case VALUE_RATIONAL:
    q_clear(&value->q);
    break;
  case VALUE_LIBPOLY:
    lp_value_destruct(&value->lp_value);
    break;
  default:
    assert(false);
  }
}

void mcsat_value_assign(mcsat_value_t* value, const mcsat_value_t* from) {
  if (value != from) {
    mcsat_value_destruct(value);
    mcsat_value_construct_copy(value, from);
  }
}

void mcsat_value_print(const mcsat_value_t* value, FILE* out) {
  switch (value->type) {
  case VALUE_NONE:
    fprintf(out, "<NONE>");
    break;
  case VALUE_BOOLEAN:
    if (value->b) {
      fprintf(out, "true");
    } else {
      fprintf(out, "false");
    }
    break;
  case VALUE_RATIONAL:
    q_print(out, (rational_t*) &value->q);
    break;
  case VALUE_LIBPOLY:
    lp_value_print(&value->lp_value, out);
    break;
  default:
    assert(false);
  }
}