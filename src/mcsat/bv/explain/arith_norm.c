/*
 * The Yices SMT Solver. Copyright 2015 SRI International.
 *
 * This program may only be used subject to the noncommercial end user
 * license agreement which is downloadable along with this program.
 */

#include "mcsat/tracing.h"
#include "terms/bvarith_buffer_terms.h"
#include "terms/bvarith64_buffer_terms.h"
#include "terms/bv_constants.h"
#include "terms/bv64_constants.h"

#include "mcsat/bv/bv_utils.h"
#include "arith_norm.h"


// var_cache hash map has dynamically allocated values
// So before resetting or deleting it, one must free the memory of the stored values
// which the following function does

void arith_norm_freeval(arith_norm_t* norm) {
  for (pmap_rec_t* current = pmap_first_record(&norm->var_cache);
       current != NULL;
       current = pmap_next_record(&norm->var_cache, current)) {
    safe_free((arith_analyse_t*) current->val);
  }
}

void print_analyse(plugin_context_t* ctx, arith_analyse_t*  analysis){
  FILE* out = ctx_trace_out(ctx);
  fprintf(out, "analyse produces suffix = %d, length = %d, base = ", analysis->suffix, analysis->length);
  if (analysis->base != NULL_TERM){
    ctx_trace_term(ctx, analysis->base);
    fprintf(out, "starting at start = %d,", analysis->start);
  }
  else 
    fprintf(out, "NO_BASE,");
  fprintf(out, " with evaluable = ");    
  ctx_trace_term(ctx, analysis->eval);
  fprintf(out, "and var = ");    
  ctx_trace_term(ctx, analysis->var);
}

// The following two functions are mutually recursive

arith_analyse_t* arith_analyse(arith_norm_t* norm, term_t t, uint32_t w){

  plugin_context_t* ctx = norm->csttrail.ctx;
  term_manager_t* tm    = ctx->tm;
  term_table_t* terms   = ctx->terms;

  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
    FILE* out = ctx_trace_out(ctx);
    fprintf(out, "Starting analyse on the lower %d bits of ",w);
    ctx_trace_term(ctx, t);
  }

  assert(is_bitvector_term(terms, t));
  assert(w <= term_bitsize(terms, t));

  pmap_rec_t* entry = pmap_get(&norm->var_cache, t, w);
  assert(entry != NULL);
  arith_analyse_t* result = (arith_analyse_t*) entry->val;
  if (result != DEFAULT_PTR) {
    assert (result != NULL);
    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Found it in memoisation table!\n");
      print_analyse(ctx, result);
    }
    return result;
  }

  result = safe_malloc(sizeof(arith_analyse_t));
  entry->val = (void*) result;
  result->suffix  = 0;
  result->length  = 0;
  result->start   = 0;
  result->base    = NULL_TERM;
  result->eval    = NULL_TERM;
  result->var     = NULL_TERM;
  result->norm    = NULL_TERM;
  result->intros  = false;
  result->nobueno = false;

  switch (term_kind(terms, t)) {
  case BV_POLY:
  case BV64_POLY:
    assert(false);
  case BV_ARRAY: {  // Concatenated boolean terms
    composite_term_t* concat_desc = bvarray_term_desc(terms, t);
    term_t ebits[w]; // Where we build result->eval

    // First, we eliminate BIT_TERM-over-BV_ARRAYs:
    for (uint32_t i = 0; i < w; i++)
      ebits[i] = bv_bitterm(terms, concat_desc->arg[i]);
    
    // Hand-made hash map (we want it light, non-redimensionable,
    // and we can do so because we know the max size is w).
    // In each cell i, there are 4 integers:
    // preproc[0][i] is the key, which is a term_t, let's call it k
    // preproc[1][i] is the maximum i such that k[i] is a bit of this bv_array, let's call it top; then:
    // preproc[1][i] is the term_t arith_normalise_upto(k,top+1) (normalised version of k over the lowest top+1 bits), let's call it norm
    // preproc[2][i] is the value returned by bv_evaluator_not_free_up_to(norm), let's call it maxeval
    // preproc[3][i] is the term_t arith_normalise_upto(norm,maxeval), if maxeval is not 0
    term_t preproc[4][w];
    // We initialise the hashmap
    fix_htbl_init(preproc[0], w);
      
    // Now we range over the bits of the t bv_array of the form base[index], and we fill-in preproc[0][*] and preproc[1][*]
    for (uint32_t i = 0; i < w; i++) {
      term_t t_i = ebits[i];        // The Boolean term that constitutes bit i of t
      if (term_kind(terms, t_i) == BIT_TERM) { // Is it of the form base[index] ?
        uint32_t index = bit_term_index(terms, t_i);  // Get selected bit
        term_t base    = bit_term_arg(terms, t_i);    // Get the base
        assert(term_kind(terms, base) != BV_ARRAY);
        uint32_t key_index = fix_htbl_index(preproc[0],w,base);
        if (preproc[0][key_index] == NULL_TERM
            || preproc[1][key_index] < index)
          preproc[1][key_index] = index;
        if (preproc[0][key_index] == NULL_TERM)
          preproc[0][key_index] = base;
      }
    }

    // Now we fill-in preproc[1][*], preproc[2][*], preproc[3][*]
    for (uint32_t i = 0; i < w; i++) {
      if (preproc[0][i] != NULL_TERM) {
        uint32_t size = preproc[1][i] + 1;
        preproc[1][i] = arith_normalise_upto(norm, preproc[0][i], size);
        preproc[2][i] = bv_evaluator_not_free_up_to(&norm->csttrail, preproc[1][i]);
        if (preproc[2][i] > size)
          preproc[2][i] = size;
        if (preproc[2][i] > 0) {
          preproc[3][i] = (preproc[2][i] == size) ?
            preproc[1][i] :
            arith_normalise_upto(norm, preproc[1][i], preproc[2][i]);
        }
      }
    }

    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Back to analyse function:\n");
    }

    term_t cbits[w]; // Where we build the central section, if the term is bad
    uint32_t shortlength  = 0;         // Number of bits of the central section of t, excluding sign-extension bits
    term_t signbit        = NULL_TERM; // The sign bit of the central section of t
    term_t lastbase       = NULL_TERM; // The base from the previous cell
    bool is_negated       = false; // whether the first bit of the central section is negated
    
    // We inspect each bit of the bv_array
    for (uint32_t i = 0; i < w; i++) {

      term_t t_i = ebits[i]; // The Boolean term that constitutes that bit

      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "bit %d is ",i);
        term_print_to_file(out, terms, t_i);
        fprintf(out, " with suffix = %d, length = %d, shortlength = %d\n",result->suffix,result->length, shortlength);
      }

      // Then we normalise the bit t_i
      if (term_kind(terms, t_i) == BIT_TERM) {
        uint32_t index = bit_term_index(terms, t_i); // Get selected bit
        term_t base    = bit_term_arg(terms, t_i);   // Get the base
        assert(term_kind(terms, base) != BV_ARRAY);
        bool isneg     = is_neg_term(t_i);
        uint32_t key_index = fix_htbl_index(preproc[0],w,base);
        base = (index < preproc[2][key_index]) ?
          preproc[3][key_index] :
          preproc[1][key_index] ;
        t_i = bv_bitterm(terms, mk_bitextract(tm, base, index));
        if (isneg) t_i = not_term(terms, t_i); // Same polarity as the original t_i
      }

      ebits[i] = t_i; // and we record it in ebits
      bool evaluable = bv_evaluator_is_evaluable(&norm->csttrail, t_i); // Whether this bit can be evaluated

      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "bit %d is simplified (and is %s) to ", i, evaluable ? "evaluable" : "non-evaluable");
        ctx_trace_term(ctx, t_i);
      }

      // Now we handle the fields suffix, length, base, start, etc of result
      if (evaluable) { // So we look at whether that bit can be evaluated
        lastbase = NULL_TERM;     // No base here
        if (result->length == 0)  // If still in suffix
          result->suffix ++;      // We increase suffix
      } else {

        bool isneg = is_neg_term(t_i); // Whether the boolean term is negated
        if (result->length == 0) // This is the first unevaluable bit
          is_negated = isneg;    // It defines the polarity of the central section

        if (term_kind(terms, t_i) != BIT_TERM // not a bit-select
            || is_negated != isneg ) {        // or not the right polarity
          result->base = NULL_TERM; // Makes the whole term BAD (if not evaluable)
        } else {
          uint32_t index = bit_term_index(terms, t_i); // Get selected bit
          term_t base    = bit_term_arg(terms, t_i);   // Get the base
          assert(term_kind(terms, base) != BV_ARRAY);
          if (result->length == 0) { // This is the first unevaluable bit
            lastbase  = base;
            result->base  = base;
            result->start = index;
            if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
              FILE* out = ctx_trace_out(ctx);
              fprintf(out, "result->start set to %d with t_i\n", result->start);
              ctx_trace_term(ctx, t_i);
            }
          }
          // Now we look at whether this bit, necessarily from the central section, is good
          if (base != lastbase                                   // Not the right base
              || (index - result->start) != (i - result->suffix) ) { // Not the right index
            result->base = NULL_TERM;    // in both cases, the term is BAD
          }
        }
        result->length = i - result->suffix +1;
        if (signbit != t_i) {
          shortlength = result->length;
          signbit = t_i;
        }
        assert(result->length > 0);
      }
      
      if (result->length != 0) // The central bits are recorded, flipping polarity so that the first bit in there is positive
        cbits[i] = is_negated ? not_term(terms, t_i) : t_i;
    }

    // Now we look at the central section between suffix and suffix+shortlength,
    // and try to build result->base and result->start

    if (shortlength > 0) { // if the central section exists
      if (result->base == NULL_TERM) { // ...but the term is bad
        result->base = mk_bvarray(tm, shortlength, &cbits[result->suffix]); // build the central section anyways
        result->start = 0;
        result->nobueno = true;
        if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
          FILE* out = ctx_trace_out(ctx);
          fprintf(out, "Not a good term, creating base\n");
          ctx_trace_term(ctx, result->base);
        }
      }
    }

    if (is_negated) { // The central cbits have flipped polarity
      assert(shortlength > 0);
      assert(result->base != NULL_TERM);
      result->base   = arith_negate(tm, arith_add_one(tm, result->base));
      result->intros = true;
      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "First bit negated, changing base to\n");
        ctx_trace_term(ctx, result->base);
      }
    }
    
    if (shortlength != result->length) {
      // This is a sign-extension,
      // base to return is (0extend(base+half(shortlength)) - 0extend(half(shortlength)))
      term_t tmp1 = arith_upextension(tm,
                                         arith_add_half(tm, result->base),
                                         false_term,
                                         result->length);
      term_t tmp2 = arith_upextension(tm,
                                         arith_add_half(tm, arith_zero(tm, shortlength)),
                                         false_term,
                                         result->length);
      result->base   = arith_sub(tm, tmp1, tmp2);
      result->intros = true;
      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "Sign extension, changing base to\n");
        ctx_trace_term(ctx, result->base);
      }
    }

    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Final result: suffix = %d, length = %d, shortlength = %d\n",result->suffix,result->length, shortlength);
      if (result->base != NULL_TERM)
        ctx_trace_term(ctx, result->base);
      else
        fprintf(out, "No base\n");
    }

    // Now we can construct result->norm, result->eval and result->var
    // We first finish the normalisation of the central bits:
    for (uint32_t i = 0; i < result->length; i++)
      ebits[i + result->suffix] = mk_bitextract(tm, result->base, i + result->start);
    result->norm = mk_bvarray(tm, w, ebits);
    
    // We distribute each bit recorded in ebits to either ebits (for result->eval) or cbits (for result->var)
    for (uint32_t i = 0; i < w; i++)
      if (i < result->suffix || i >= result->suffix + result->length)
        cbits[i] = false_term; // prefix and suffix are 0...0 for result->var
      else { // central section
        cbits[i] = ebits[i];
        ebits[i] = false_term;
      }
    result->eval = mk_bvarray(tm, w, ebits);
    result->var  = mk_bvarray(tm, w, cbits);
    break;
  }

  default: { // Term t is now not a bv_array, nor a bv_poly nor a bv64_poly
    // There is no recursive normalization to perform
    term_t tmp  = term_extract(tm, t, 0, w);
    term_t zero = arith_zero(tm, w);
    if (bv_evaluator_is_evaluable(&norm->csttrail, t)) {
      result->eval = tmp;
      result->var  = zero;
      result->norm   = tmp;
      result->suffix = w;
      result->length = 0;
      result->start  = 0;
      result->base = NULL_TERM;
      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "Term is not a BV_POLY/BV64_POLY/BV_ARRAY, we just get evaluable ");
        ctx_trace_term(ctx, tmp);
      }
    } else {
      result->eval   = zero;
      result->var    = tmp;
      result->norm   = tmp;
      result->suffix = 0;
      result->length = w;
      result->start  = 0;
      result->base   = tmp;
      if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
        FILE* out = ctx_trace_out(ctx);
        fprintf(out, "Term is not a BV_POLY/BV64_POLY/BV_ARRAY, we just get non-evaluable ");
        ctx_trace_term(ctx, tmp);
      }
    }
  }
  }

  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan"))
    print_analyse(ctx, result);
  return result;  // Note that the result is automatically memoised
  
}

// Extracting the w lowest bits of t, normalising on the way
term_t arith_normalise_upto(arith_norm_t* norm, term_t t, uint32_t w){

  // standard abbreviations
  plugin_context_t* ctx = norm->csttrail.ctx;
  term_t conflict_var   = norm->csttrail.conflict_var_term;
  term_manager_t* tm    = ctx->tm;
  term_table_t* terms   = ctx->terms;

  assert(is_bitvector_term(terms,t));
  uint32_t original_bitsize = term_bitsize(terms, t);
  assert(w <= original_bitsize);
  
  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
    FILE* out = ctx_trace_out(ctx);
    fprintf(out, "Normalising %d lowest bits of ",w);
    term_print_to_file(out, terms, t);
    fprintf(out, " (bitsize is %d))\n",original_bitsize);
  }

  if (t == conflict_var) {
    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Conflict variable, so it's already normalised\n");
    }
    return term_extract(tm, t, 0, w);
  }

  switch (term_kind(terms, t)) { // Simple check for constants
  case CONSTANT_TERM:
  case BV_CONSTANT:
  case BV64_CONSTANT: {
    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Constant, so it's already normalised\n");
    }
    return term_extract(tm, t, 0, w);
  }
  default: {
  }
  }

  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
    FILE* out = ctx_trace_out(ctx);
    fprintf(out, "Not conflict var nor a constant. Now looking at memoisation table.\n");
  }

  // We first look at whether the value is cached
  int_hmap2_rec_t* t_norm = int_hmap2_find(&norm->norm_cache, t, w);
  if (t_norm != NULL) {
    if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
      FILE* out = ctx_trace_out(ctx);
      fprintf(out, "Found in the memoisation table! It's ");
      term_print_to_file(out, terms, t_norm->val);
    }
    return t_norm->val;
  }

  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
    FILE* out = ctx_trace_out(ctx);
    fprintf(out, "Not memoised. We now look into the term.\n");
  }

  // Now the result will be a sum; first first compute the number of summands
  uint32_t n_monom;
  bvpoly_t* t_poly = NULL;
  bvpoly64_t* t_poly64 = NULL;
  
  switch (term_kind(terms, t)) {
  case BV_POLY: {
    t_poly = bvpoly_term_desc(ctx->terms, t);
    n_monom = t_poly->nterms;
    break;
  }
  case BV64_POLY: {
    t_poly64 = bvpoly64_term_desc(ctx->terms, t);
    n_monom = t_poly64->nterms;
    break;
  }
  default: {
    n_monom = 1;
  }
  }

  term_t monom[n_monom]; // where we place the monomials
  bvconstant_t coeff[n_monom]; // where we place the monomials' coefficients
  uint64_t coeff64[n_monom];   // where we place the monomials' coefficients

  switch (term_kind(terms, t)) {
  case BV_POLY: {
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (w<65) {
        // If we extract fewer than 65 bits, we use uint64_t coefficients for the bv_poly to produce
        coeff64[i] = // projecting the monomial coefficient onto w bits
          (original_bitsize < 33) ? // requires an annoying case analysis, for some reason
          ( (uint64_t) bvconst_get32(t_poly->mono[i].coeff)) :
          bvconst_get64(t_poly->mono[i].coeff) ;
      } else {
        init_bvconstant(&coeff[i]);
        bvconstant_extract(&coeff[i], t_poly->mono[i].coeff, 0, w); // projecting the monomial coefficient onto w bits
        bvconstant_normalize(&coeff[i]);
      }
      monom[i] = (t_poly->mono[i].var != const_idx) ? t_poly->mono[i].var : NULL_TERM;
    }
    break;
  }
  case BV64_POLY: { // Same game, but now t is a bv64_poly, so w <= 64
    for (uint32_t i = 0; i < n_monom; ++ i) {
      coeff64[i] = t_poly64->mono[i].coeff;
      monom[i] = (t_poly64->mono[i].var != const_idx) ? t_poly64->mono[i].var : NULL_TERM;
    }
    break;
  }
  default: {
    if (w<65) {
      coeff64[0] = 1;
    } else {
      init_bvconstant(coeff);
      bvconstant_set_bitsize(coeff, w);
      bvconstant_set_one(coeff);
    }
    monom[0] = t;
  }
  }

  term_t evaluables[n_monom]; // where we place the monomials
  term_t garbage[n_monom]; // where we place the monomials
  term_t zero = arith_zero(tm, w);
  // Now we proceed to recursively extract the monomials
  for (uint32_t i = 0; i < n_monom; ++ i) {
    if (monom[i] != NULL_TERM) {
      assert(monom[i] <= t);
      arith_analyse_t* s = arith_analyse(norm, monom[i], w);
      evaluables[i] = s->eval;
      if (s->var == NULL_TERM) {
        monom[i]   = zero;
        garbage[i] = zero;
      } else {
        if (s->nobueno) {
          monom[i]   = zero;
          garbage[i] = s->var;
        } else {
          monom[i]   = s->var;
          garbage[i] = zero;
        }
      }
      assert(term_bitsize(terms,monom[i]) == w);
    }
  }

  term_t eval_term;
  term_t garbage_term;
  term_t var_term;
  
  if (w<65) {
    // If we extract fewer than 65 bits, we use uint64_t coefficients for the bv_poly to produce
    // we construct that bv_poly from a bvarith64_buffer_t called buffer:
    bvarith64_buffer_t* buffer = term_manager_get_bvarith64_buffer(tm);
    bvarith64_buffer_prepare(buffer, w); // Setting the desired width
    // Now going into each monomial
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] != NULL_TERM)
        bvarith64_buffer_add_const_times_term(buffer, terms, coeff64[i], monom[i]);
    }
    var_term = mk_bvarith64_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
    bvarith64_buffer_prepare(buffer, w); // Setting the desired width
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] != NULL_TERM)
        bvarith64_buffer_add_const_times_term(buffer, terms, coeff64[i], garbage[i]);
    }
    garbage_term = mk_bvarith64_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
    bvarith64_buffer_prepare(buffer, w); // Setting the desired width
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] == NULL_TERM)
        bvarith64_buffer_add_const(buffer, coeff64[i]); // constant coefficient gets added to the buffer bv_poly
      else
        bvarith64_buffer_add_const_times_term(buffer, terms, coeff64[i], evaluables[i]);
    }
    eval_term = mk_bvarith64_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
  } else {
    // If we extract more than 64 bits, we use regular coefficients for the bv_poly to produce
    // we construct that bv_poly from a bvarith_buffer_t called buffer:
    bvarith_buffer_t* buffer = term_manager_get_bvarith_buffer(tm);
    bvarith_buffer_prepare(buffer, w); // Setting the desired width
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] != NULL_TERM)
        bvarith_buffer_add_const_times_term(buffer, terms, coeff[i].data, monom[i]); // Otherwise we add the w-bit monomial to the bv_poly
    }
    var_term = mk_bvarith_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
    bvarith_buffer_prepare(buffer, w); // Setting the desired width
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] != NULL_TERM)
        bvarith_buffer_add_const_times_term(buffer, terms, coeff[i].data, garbage[i]); // Otherwise we add the w-bit monomial to the bv_poly
    }
    garbage_term = mk_bvarith_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
    bvarith_buffer_prepare(buffer, w); // Setting the desired width
    for (uint32_t i = 0; i < n_monom; ++ i) {
      if (monom[i] == NULL_TERM)
        bvarith_buffer_add_const(buffer, coeff[i].data);// constant coefficient gets aded to the buffer bv_poly
      else
        bvarith_buffer_add_const_times_term(buffer, terms, coeff[i].data, evaluables[i]); // Otherwise we add the w-bit monomial to the bv_poly
      delete_bvconstant(&coeff[i]); //cleaning up
    }
    eval_term = mk_bvarith_term(tm, buffer); // We turn the bv_poly into an actual term, and return it
  }

  term_t result = arith_add(tm, var_term, arith_add(tm, garbage_term, eval_term));
  if (ctx_trace_enabled(ctx, "mcsat::bv::arith::scan")) {
    FILE* out = ctx_trace_out(ctx);
    fprintf(out, "Normalising %d lowest bits of ",w);
    term_print_to_file(out, terms, t);
    fprintf(out, " successfully gave var_term = ");
    term_print_to_file(out, terms, var_term);
    fprintf(out, ", garbage_term = ");
    term_print_to_file(out, terms, garbage_term);
    fprintf(out, ", eval_term = ");
    term_print_to_file(out, terms, eval_term);
    fprintf(out, ", adding up to ");
    term_print_to_file(out, terms, result);
    fprintf(out, "\n");
  }
  
  // We know what we are returning, now we just cache it for later
  int_hmap2_add(&norm->norm_cache, t, w, result);

  return result; 

}