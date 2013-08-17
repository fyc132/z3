/*++
Copyright (c) 2011 Microsoft Corporation

Module Name:

    iz3mgr.cpp

Abstract:

   A wrapper around an ast manager, providing convenience methods.

Author:

    Ken McMillan (kenmcmil)

Revision History:

--*/


#include "iz3mgr.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ostream>

#include "expr_abstract.h"
#include "params.h"


#ifndef WIN32
using namespace stl_ext;
#endif


std::ostream &operator <<(std::ostream &s, const iz3mgr::ast &a){
  return s;
}


iz3mgr::ast iz3mgr::make_var(const std::string &name, type ty){ 
  symbol s = symbol(name.c_str());
  return cook(m().mk_const(m().mk_const_decl(s, ty)));
}

iz3mgr::ast iz3mgr::make(opr op, int n, raw_ast **args){
  switch(op) {
  case True:     return mki(m_basic_fid,OP_TRUE,n,args);
  case False:    return mki(m_basic_fid,OP_FALSE,n,args);
  case Equal:    return mki(m_basic_fid,OP_EQ,n,args);
  case Distinct: return mki(m_basic_fid,OP_DISTINCT,n,args);
  case Ite:      return mki(m_basic_fid,OP_ITE,n,args);
  case And:      return mki(m_basic_fid,OP_AND,n,args);
  case Or:       return mki(m_basic_fid,OP_OR,n,args);
  case Iff:      return mki(m_basic_fid,OP_IFF,n,args);
  case Xor:      return mki(m_basic_fid,OP_XOR,n,args);
  case Not:      return mki(m_basic_fid,OP_NOT,n,args);
  case Implies:  return mki(m_basic_fid,OP_IMPLIES,n,args);
  case Oeq:      return mki(m_basic_fid,OP_OEQ,n,args);
  case Interp:   return mki(m_basic_fid,OP_INTERP,n,args);
  case Leq:      return mki(m_arith_fid,OP_LE,n,args);
  case Geq:      return mki(m_arith_fid,OP_GE,n,args);
  case Lt:       return mki(m_arith_fid,OP_LT,n,args);
  case Gt:       return mki(m_arith_fid,OP_GT,n,args);
  case Plus:     return mki(m_arith_fid,OP_ADD,n,args);
  case Sub:      return mki(m_arith_fid,OP_SUB,n,args);
  case Uminus:   return mki(m_arith_fid,OP_UMINUS,n,args);
  case Times:    return mki(m_arith_fid,OP_MUL,n,args);
  case Div:      return mki(m_arith_fid,OP_DIV,n,args);
  case Idiv:     return mki(m_arith_fid,OP_IDIV,n,args);
  case Rem:      return mki(m_arith_fid,OP_REM,n,args);
  case Mod:      return mki(m_arith_fid,OP_MOD,n,args);
  case Power:    return mki(m_arith_fid,OP_POWER,n,args);
  case ToReal:   return mki(m_arith_fid,OP_TO_REAL,n,args);
  case ToInt:    return mki(m_arith_fid,OP_TO_INT,n,args);
  case IsInt:    return mki(m_arith_fid,OP_IS_INT,n,args);
  case Store:    return mki(m_array_fid,OP_STORE,n,args);
  case Select:   return mki(m_array_fid,OP_SELECT,n,args);
  case ConstArray: return mki(m_array_fid,OP_CONST_ARRAY,n,args);
  case ArrayDefault: return mki(m_array_fid,OP_ARRAY_DEFAULT,n,args);
  case ArrayMap: return mki(m_array_fid,OP_ARRAY_MAP,n,args);
  case SetUnion: return mki(m_array_fid,OP_SET_UNION,n,args);
  case SetIntersect: return mki(m_array_fid,OP_SET_INTERSECT,n,args);
  case SetDifference: return mki(m_array_fid,OP_SET_DIFFERENCE,n,args);
  case SetComplement: return mki(m_array_fid,OP_SET_COMPLEMENT,n,args);
  case SetSubSet: return mki(m_array_fid,OP_SET_SUBSET,n,args);
  case AsArray:   return mki(m_array_fid,OP_AS_ARRAY,n,args);
  default:
    assert(0);
    return ast();
  }
}

iz3mgr::ast iz3mgr::mki(family_id fid, decl_kind dk, int n, raw_ast **args){
  return cook(m().mk_app(fid, dk, 0, 0, n, (expr **)args));        
}

iz3mgr::ast iz3mgr::make(opr op, const std::vector<ast> &args){
  static std::vector<raw_ast*> a(10);
  if(a.size() < args.size())
    a.resize(args.size());
  for(unsigned i = 0; i < args.size(); i++)
    a[i] = args[i].raw();
  return make(op,args.size(), args.size() ? &a[0] : 0);
}

iz3mgr::ast iz3mgr::make(opr op){
  return make(op,0,0);
}

iz3mgr::ast iz3mgr::make(opr op, const ast &arg0){
  raw_ast *a = arg0.raw();
  return make(op,1,&a);
}

iz3mgr::ast iz3mgr::make(opr op, const ast &arg0, const ast &arg1){
  raw_ast *args[2];
  args[0] = arg0.raw();
  args[1] = arg1.raw();
  return make(op,2,args);
}

iz3mgr::ast iz3mgr::make(opr op, const ast &arg0, const ast &arg1, const ast &arg2){
  raw_ast *args[3];
  args[0] = arg0.raw();
  args[1] = arg1.raw();
  args[2] = arg2.raw();
  return make(op,3,args);
}

iz3mgr::ast iz3mgr::make(symb sym, int n, raw_ast **args){
  return cook(m().mk_app(sym, n, (expr **) args));   
}

iz3mgr::ast iz3mgr::make(symb sym, const std::vector<ast> &args){
  static std::vector<raw_ast*> a(10);
  if(a.size() < args.size())
    a.resize(args.size());
  for(unsigned i = 0; i < args.size(); i++)
    a[i] = args[i].raw();
  return make(sym,args.size(), args.size() ? &a[0] : 0);
}

iz3mgr::ast iz3mgr::make(symb sym){
  return make(sym,0,0);
}

iz3mgr::ast iz3mgr::make(symb sym, const ast &arg0){
  raw_ast *a = arg0.raw();
  return make(sym,1,&a);
}

iz3mgr::ast iz3mgr::make(symb sym, const ast &arg0, const ast &arg1){
  raw_ast *args[2];
  args[0] = arg0.raw();
  args[1] = arg1.raw();
  return make(sym,2,args);
}

iz3mgr::ast iz3mgr::make(symb sym, const ast &arg0, const ast &arg1, const ast &arg2){
  raw_ast *args[3];
  args[0] = arg0.raw();
  args[1] = arg1.raw();
  args[2] = arg2.raw();
  return make(sym,3,args);
}

iz3mgr::ast iz3mgr::make_quant(opr op, const std::vector<ast> &bvs, ast &body){
  if(bvs.size() == 0) return body;
  std::vector<raw_ast *> foo(bvs.size());


  std::vector<symbol> names;
  std::vector<sort *> types;
  std::vector<expr *>  bound_asts;
  unsigned num_bound = bvs.size();

  for (unsigned i = 0; i < num_bound; ++i) {
    app* a = to_app(bvs[i].raw());
    symbol s(to_app(a)->get_decl()->get_name());
    names.push_back(s);
    types.push_back(m().get_sort(a));
    bound_asts.push_back(a);
  }
  expr_ref abs_body(m());
  expr_abstract(m(), 0, num_bound, &bound_asts[0], to_expr(body.raw()), abs_body);
  expr_ref result(m());
  result = m().mk_quantifier(
			     op == Forall, 
			     names.size(), &types[0], &names[0], abs_body.get(),            
			     0, 
			     symbol(),
			     symbol(),
			     0, 0,
			     0, 0
			     );
  return cook(result.get());
}

// FIXME replace this with existing Z3 functionality

iz3mgr::ast iz3mgr::clone(const ast &t, const std::vector<ast> &_args){
  if(_args.size() == 0)
    return t;

  ast_manager& m = m_manager;
  expr* a = to_expr(t.raw());
  static std::vector<raw_ast*> rargs(10);
  if(rargs.size() < _args.size())
    rargs.resize(_args.size());
  for(unsigned i = 0; i < _args.size(); i++)
    rargs[i] = _args[i].raw();
  expr* const* args =  (expr **)&rargs[0];
  switch(a->get_kind()) {
  case AST_APP: {
    app* e = to_app(a);
    if (e->get_num_args() != _args.size()) {
      assert(0);
    }
    else {
      a = m.mk_app(e->get_decl(), _args.size(), args);
    }
    break;
  }
  case AST_QUANTIFIER: {
    if (_args.size() != 1) {
      assert(0);
    }
    else {
      a = m.update_quantifier(to_quantifier(a), args[0]);
    }
    break;
  }
  default:
    break;
  }            
  return cook(a);
}


void iz3mgr::show(ast t){
  std::cout  << mk_pp(t.raw(), m()) << std::endl;
}

void iz3mgr::show_symb(symb s){
  std::cout  << mk_pp(s, m()) << std::endl;
}

void iz3mgr::print_expr(std::ostream &s, const ast &e){
  s << mk_pp(e.raw(), m());
}


void iz3mgr::print_clause(std::ostream &s, std::vector<ast> &cls){
  s << "(";
  for(unsigned i = 0; i < cls.size(); i++){
    if(i > 0) s << ",";
    print_expr(s,cls[i]);
  }
  s << ")";
}

void iz3mgr::show_clause(std::vector<ast> &cls){
  print_clause(std::cout,cls);
  std::cout << std::endl;
}

void iz3mgr::print_lit(ast lit){
  ast abslit = is_not(lit) ? arg(lit,0) : lit;
  int f = op(abslit);
  if(f == And || f == Or || f == Iff){
    if(is_not(lit)) std::cout << "~";
    std::cout << "[" << abslit << "]";
  }
  else
    std::cout << lit;
}  


static int pretty_cols = 79;
static int pretty_indent_chars = 2;

static int pretty_find_delim(const std::string &s, int pos){
  int level = 0;
  int end = s.size();
  for(; pos < end; pos++){
    int ch = s[pos];
    if(ch == '(')level++;
    if(ch == ')')level--;
    if(level < 0 || (level == 0 && ch == ','))break;
  }
  return pos;
}

static void pretty_newline(std::ostream &f, int indent){
  f << std::endl;
  for(int i = 0; i < indent; i++)
    f << " ";
}

void iz3mgr::pretty_print(std::ostream &f, const std::string &s){
  int cur_indent = 0;
  int indent = 0;
  int col = 0;
  int pos = 0;
  while(pos < (int)s.size()){
    int delim = pretty_find_delim(s,pos);
    if(s[pos] != ')' && s[pos] != ',' && cur_indent > indent){
      pretty_newline(f,indent);
      cur_indent = indent;
      col = indent;
      continue;
    }
    if (col + delim - pos > pretty_cols) {
      if (col > indent) {
	pretty_newline(f,indent);
	cur_indent = indent;
	col = indent;
	continue;
      }
      int paren = s.find('(',pos);
      if(paren != (int)std::string::npos){
	int chars = paren - pos + 1;
	f << s.substr(pos,chars);
	indent += pretty_indent_chars;
	if(col) pretty_newline(f,indent);
	cur_indent = indent;
	pos += chars;
	col = indent;
	continue;
      }
    }
    int chars = delim - pos + 1;
    f << s.substr(pos,chars);
    pos += chars;
    col += chars;
    if(s[delim] == ')')
      indent -= pretty_indent_chars;
  }
}


iz3mgr::opr iz3mgr::op(const ast &t){
    ast_kind dk = t.raw()->get_kind();
    switch(dk){
    case AST_APP: {
      expr * e = to_expr(t.raw());
      func_decl *d = to_app(t.raw())->get_decl();
      if (null_family_id == d->get_family_id())
	return Uninterpreted;
      // return (opr)d->get_decl_kind();
      if (m_basic_fid == d->get_family_id()) {
	switch(d->get_decl_kind()) {
	case OP_TRUE:     return True;
	case OP_FALSE:    return False;
	case OP_EQ:       return Equal;
	case OP_DISTINCT: return Distinct;
	case OP_ITE:      return Ite;
	case OP_AND:      return And;
	case OP_OR:       return Or;
	case OP_IFF:      return Iff;
	case OP_XOR:      return Xor;
	case OP_NOT:      return Not;
	case OP_IMPLIES:  return Implies;
	case OP_OEQ:      return Oeq;
	case OP_INTERP:   return Interp;
	default:
	  return Other;
        }
      }
      if (m_arith_fid == d->get_family_id()) {
	switch(d->get_decl_kind()) {
	case OP_LE: return Leq;
	case OP_GE: return Geq;
	case OP_LT: return Lt;
	case OP_GT: return Gt;
	case OP_ADD: return Plus;
	case OP_SUB: return Sub;
	case OP_UMINUS: return Uminus;
	case OP_MUL: return Times;
	case OP_DIV: return Div;
	case OP_IDIV: return Idiv;
	case OP_REM: return Rem;
	case OP_MOD: return Mod;
	case OP_POWER: return Power;
	case OP_TO_REAL: return ToReal;
	case OP_TO_INT: return ToInt;
	case OP_IS_INT: return IsInt;
	default:
	  if (m().is_unique_value(e)) 
	    return Numeral;
	  return Other;
	  }
      }
      if (m_array_fid == d->get_family_id()) {
	switch(d->get_decl_kind()) {
	case OP_STORE: return Store;
	case OP_SELECT: return Select;
	case OP_CONST_ARRAY: return ConstArray;
	case OP_ARRAY_DEFAULT: return ArrayDefault;
	case OP_ARRAY_MAP: return ArrayMap;
	case OP_SET_UNION: return SetUnion;
	case OP_SET_INTERSECT: return SetIntersect;
	case OP_SET_DIFFERENCE: return SetDifference;
	case OP_SET_COMPLEMENT: return SetComplement;
	case OP_SET_SUBSET: return SetSubSet;
	case OP_AS_ARRAY: return AsArray;
	default:
	  return Other;
	}
      }
      
      return Other;
    }


    case AST_QUANTIFIER:
      return to_quantifier(t.raw())->is_forall() ? Forall : Exists;
    case AST_VAR:
      return Variable;
    default:;
    }
    return Other;
}


iz3mgr::pfrule iz3mgr::pr(const ast &t){
  func_decl *d = to_app(t.raw())->get_decl();
  assert(m_basic_fid == d->get_family_id());
  return d->get_decl_kind();
}

void iz3mgr::print_sat_problem(std::ostream &out, const ast &t){
  ast_smt_pp pp(m());
  pp.set_simplify_implies(false);
  pp.display_smt2(out, to_expr(t.raw()));
}

iz3mgr::ast iz3mgr::z3_simplify(const ast &e){
    ::expr * a = to_expr(e.raw());
    params_ref p; 
    th_rewriter m_rw(m(), p);
    expr_ref    result(m());
    m_rw(a, result);
    return cook(result);
}


#if 0
static rational lcm(const rational &x, const rational &y){
  int a = x.numerator();
  int b = y.numerator();
  return rational(a * b / gcd(a, b));
}
#endif

static rational extract_lcd(std::vector<rational> &res){
  if(res.size() == 0) return rational(1); // shouldn't happen
  rational lcd = denominator(res[0]);
  for(unsigned i = 1; i < res.size(); i++)
    lcd = lcm(lcd,denominator(res[i]));
  for(unsigned i = 0; i < res.size(); i++)
    res[i] *= lcd;
  return lcd;
}

void iz3mgr::get_farkas_coeffs(const ast &proof, std::vector<ast>& coeffs){
  std::vector<rational> rats;
  get_farkas_coeffs(proof,rats);
  coeffs.resize(rats.size());
  for(unsigned i = 0; i < rats.size(); i++){
    sort *is = m().mk_sort(m_arith_fid, INT_SORT);
    ast coeff = cook(m_arith_util.mk_numeral(rats[i],is));
    coeffs[i] = coeff;
  }
}

void iz3mgr::get_farkas_coeffs(const ast &proof, std::vector<rational>& rats){
  symb s = sym(proof);
  int numps = s->get_num_parameters();
  rats.resize(numps-2);
  for(int i = 2; i < numps; i++){
    rational r;
    bool ok = s->get_parameter(i).is_rational(r);
    if(!ok)
      throw "Bad Farkas coefficient";
    {
      ast con = conc(prem(proof,i-2));
      ast temp = make_real(r); // for debugging
      opr o = is_not(con) ? op(arg(con,0)) : op(con);
      if(is_not(con) ? (o == Leq || o == Lt) : (o == Geq || o == Gt))
	r = -r;
    }
    rats[i-2] = r;
  }
  extract_lcd(rats);
}

void iz3mgr::get_assign_bounds_coeffs(const ast &proof, std::vector<ast>& coeffs){
  std::vector<rational> rats;
  get_assign_bounds_coeffs(proof,rats);
  coeffs.resize(rats.size());
  for(unsigned i = 0; i < rats.size(); i++){
    coeffs[i] = make_int(rats[i]);
  }
}

void iz3mgr::get_assign_bounds_coeffs(const ast &proof, std::vector<rational>& rats){
  symb s = sym(proof);
  int numps = s->get_num_parameters();
  rats.resize(numps-1);
  rats[0] = rational(1);
  for(int i = 2; i < numps; i++){
    rational r;
    bool ok = s->get_parameter(i).is_rational(r);
    if(!ok)
      throw "Bad Farkas coefficient";
    {
      ast con = arg(conc(proof),i-1);
      ast temp = make_real(r); // for debugging
#if 0
      opr o = is_not(con) ? op(arg(con,0)) : op(con);
      if(is_not(con) ? (o == Leq || o == Lt) : (o == Geq || o == Gt))
#endif
	r = -r;
    }
    rats[i-1] = r;
  }
  extract_lcd(rats);
}

  /** Set P to P + cQ, where P and Q are linear inequalities. Assumes P is 0 <= y or 0 < y. */

void iz3mgr::linear_comb(ast &P, const ast &c, const ast &Q){
  ast Qrhs;
  bool strict = op(P) == Lt;
  if(is_not(Q)){
    ast nQ = arg(Q,0);
    switch(op(nQ)){
    case Gt:
      Qrhs = make(Sub,arg(nQ,1),arg(nQ,0));
      break;
    case Lt: 
      Qrhs = make(Sub,arg(nQ,0),arg(nQ,1));
      break;
    case Geq:
      Qrhs = make(Sub,arg(nQ,1),arg(nQ,0));
      strict = true;
      break;
    case Leq: 
      Qrhs = make(Sub,arg(nQ,0),arg(nQ,1));
      strict = true;
      break;
    default:
      throw "not an inequality";
    }
  }
  else {
    switch(op(Q)){
    case Leq:
      Qrhs = make(Sub,arg(Q,1),arg(Q,0));
      break;
    case Geq: 
      Qrhs = make(Sub,arg(Q,0),arg(Q,1));
      break;
    case Lt:
      Qrhs = make(Sub,arg(Q,1),arg(Q,0));
      strict = true;
      break;
    case Gt: 
      Qrhs = make(Sub,arg(Q,0),arg(Q,1));
      strict = true;
      break;
    default:
      throw "not an inequality";
    }
  }
  Qrhs = make(Times,c,Qrhs);
  if(strict)
    P = make(Lt,arg(P,0),make(Plus,arg(P,1),Qrhs));
  else
    P = make(Leq,arg(P,0),make(Plus,arg(P,1),Qrhs));
}

iz3mgr::ast iz3mgr::sum_inequalities(const std::vector<ast> &coeffs, const std::vector<ast> &ineqs){
  ast zero = make_int("0");
  ast thing = make(Leq,zero,zero);
  for(unsigned i = 0; i < ineqs.size(); i++){
    linear_comb(thing,coeffs[i],ineqs[i]);
  }
  thing = simplify_ineq(thing);
  return thing;
}

void iz3mgr::mk_idiv(const ast& t, const rational &d, ast &whole, ast &frac){
  opr o = op(t);
  if(o == Plus){
    int nargs = num_args(t);
    for(int i = 0; i < nargs; i++)
      mk_idiv(arg(t,i),d,whole,frac);
    return;
  }
  else if(o == Times){
    rational coeff;
    if(is_numeral(arg(t,0),coeff)){
      if(gcd(coeff,d) == d){
	whole = make(Plus,whole,make(Times,make_int(coeff/d),arg(t,1)));
	return;
      }
    }
  }
  frac = make(Plus,frac,t);
}

iz3mgr::ast iz3mgr::mk_idiv(const ast& q, const rational &d){
  ast t = z3_simplify(q);
  if(d == rational(1))
    return t;
  else {
    ast whole = make_int("0");
    ast frac = whole;
    mk_idiv(t,d,whole,frac);
    return z3_simplify(make(Plus,whole,make(Idiv,z3_simplify(frac),make_int(d))));
  }
}

iz3mgr::ast iz3mgr::mk_idiv(const ast& t, const ast &d){
  rational r;
  if(is_numeral(d,r))
    return mk_idiv(t,r);
  return make(Idiv,t,d);
}
