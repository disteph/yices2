/*
 * This file is part of the Yices SMT Solver.
 * Copyright (C) 2018 SRI International.
 *
 * Yices is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Yices is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Yices.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * EXPERIMENTAL: SUPPORT TO DETECT EQUIVALENCE BETWEEN GATES
 */

#ifndef __GATES_EQUIV_H
#define __GATES_EQUIV_H

#include <stdint.h>

#include "solvers/cdcl/gates_hash_table.h"
#include "utils/int_hash_map.h"
#include "utils/int_vectors.h"

/*
 * Truth tables for Boolean gates of arity <= 3
 *
 * - for functions of arity 3,
 *     var[0], var[1], var[2] are the indices of three Boolean variables
 *     in increasing order
 * - for functions of arity 2, var[2] is not used (it's null_bvar = -1)
 *     var[0] and var[1] are two Boolean variables
 *
 * - the truth table is an array of 8bits b7 ... b0
 * - the encoding is as follows
 *
 *    var[0] var[1] var[2]  f
 *       0      0      0    b0
 *       0      0      1    b1
 *       0      1      0    b2
 *       0      1      1    b3
 *       1      0      0    b4
 *       1      0      1    b5
 *       1      1      0    b6
 *       1      1      1    b7
 *
 * For functions of arity 2, this looks like:
 *
 *    var[0] var[1] var[2]  f
 *       0      0      0    b0
 *       0      0      1    b0
 *       0      1      0    b2
 *       0      1      1    b2
 *       1      0      0    b4
 *       1      0      1    b4
 *       1      1      0    b6
 *       1      1      1    b6
 *
 * and var[2] is set to -1.
 */
typedef struct bgate_s {
  uint8_t ttbl; // truth table
  bvar_t  var[3]; // variables in increasing order
} bgate_t;


/*
 * Intermediate structure to store a truth table:
 * - this is used to simplify and normalize truth tables
 * - a table consists of nvars columns where nvars is between 0 and 3
 * - each column is labeled by a signed integer, which can be either a literal
 *   or a Boolean variable, or -1
 * - the truth values are stored in a bit mask (8 bit, unsigned word).
 *   all 8bits are used even if the table has fewer than 3 columms.
 */
typedef struct ttbl_s {
  uint32_t nvars;     // number of columns (between 0 and 3)
  int32_t  label[3] ; // column labels
  uint8_t  mask;      // 8-bit truth table
} ttbl_t;


/*
 * Resizable array of bgate_t descriptors
 * - size = size of array data
 * - ngates = total number of gates in use
 */
typedef struct bgate_array_s {
  bgate_t *data;
  uint32_t ngates;
  uint32_t size;
} bgate_array_t;

#define DEF_BGATE_ARRAY_SIZE 1024
#define MAX_BGATE_ARRAY_SIZE (UINT32_MAX/sizeof(bgate_t))


/*
 * Full table:
 * - gates: gate table
 * - defs: map from variable to an index in the gate table
 * - vars: array of all variables that have a definition in defs
 * - interface to access a sat solver
 */
typedef struct bdef_table_s {
  bgate_array_t gates;
  int_hmap_t defs;
  ivector_t vars;
} bdef_table_t;


/*
 * Initialization:
 */
extern void init_bdef_table(bdef_table_t *table);

/*
 * Deletion
 */
extern void delete_bdef_table(bdef_table_t *table);

/*
 * Process a gate descriptor d
 * - if d's arity is <= 3, this adds entry in the table for every
 *   output variable of d.
 */
extern void bdef_table_process_gate(bdef_table_t *table, const boolgate_t *d);

/*
 * Process all gates in gate_table
 */
extern void bdef_table_process_all_gates(bdef_table_t *table, const gate_table_t *gate_table);

#endif /* __GATES_EQUIV_H */
