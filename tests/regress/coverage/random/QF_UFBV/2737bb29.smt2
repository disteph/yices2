(set-info :source |fuzzsmt|)
(set-info :smt-lib-version 2.0)
(set-info :category "random")
(set-info :status unknown)
(set-logic QF_UFBV)
(declare-fun f0 ( (_ BitVec 4)) (_ BitVec 3))
(declare-fun p0 ( (_ BitVec 2) (_ BitVec 10) (_ BitVec 2)) Bool)
(declare-fun p1 ( (_ BitVec 8) (_ BitVec 10)) Bool)
(declare-fun v0 () (_ BitVec 12))
(declare-fun v1 () (_ BitVec 3))
(declare-fun v2 () (_ BitVec 5))
(declare-fun v3 () (_ BitVec 6))
(declare-fun v4 () (_ BitVec 10))
(assert (let ((e5(_ bv104 8)))
(let ((e6 (! (ite (p1 ((_ extract 8 1) v4) ((_ sign_extend 2) e5)) (_ bv1 1) (_ bv0 1)) :named term6)))
(let ((e7 (! (f0 ((_ extract 5 2) e5)) :named term7)))
(let ((e8 (! (ite (bvsgt v0 ((_ zero_extend 7) v2)) (_ bv1 1) (_ bv0 1)) :named term8)))
(let ((e9 (! (bvand ((_ zero_extend 11) e6) v0) :named term9)))
(let ((e10 (! (ite (p0 ((_ sign_extend 1) e6) ((_ extract 11 2) e9) ((_ extract 2 1) e7)) (_ bv1 1) (_ bv0 1)) :named term10)))
(let ((e11 (! (bvmul ((_ zero_extend 7) v1) v4) :named term11)))
(let ((e12 (! ((_ extract 5 5) v3) :named term12)))
(let ((e13 (! (p1 ((_ extract 7 0) v0) ((_ zero_extend 5) v2)) :named term13)))
(let ((e14 (! (p1 ((_ sign_extend 7) e8) ((_ sign_extend 9) e6)) :named term14)))
(let ((e15 (! (bvsge v1 v1) :named term15)))
(let ((e16 (! (p0 ((_ extract 7 6) e5) e11 ((_ sign_extend 1) e12)) :named term16)))
(let ((e17 (! (bvule v1 e7) :named term17)))
(let ((e18 (! (bvsle v0 ((_ zero_extend 6) v3)) :named term18)))
(let ((e19 (! (bvult ((_ sign_extend 3) v1) v3) :named term19)))
(let ((e20 (! (bvuge e11 v4) :named term20)))
(let ((e21 (! (bvsgt ((_ sign_extend 2) e5) e11) :named term21)))
(let ((e22 (! (bvsle v0 v0) :named term22)))
(let ((e23 (! (bvsge e9 ((_ sign_extend 6) v3)) :named term23)))
(let ((e24 (! (bvuge v3 ((_ sign_extend 1) v2)) :named term24)))
(let ((e25 (! (bvsgt e7 ((_ zero_extend 2) e6)) :named term25)))
(let ((e26 (! (distinct ((_ zero_extend 5) e8) v3) :named term26)))
(let ((e27 (! (bvult ((_ sign_extend 2) v1) v2) :named term27)))
(let ((e28 (! (bvslt ((_ zero_extend 1) v2) v3) :named term28)))
(let ((e29 (! (bvslt e9 ((_ sign_extend 11) e10)) :named term29)))
(let ((e30 (! (and e14 e20) :named term30)))
(let ((e31 (! (ite e19 e29 e21) :named term31)))
(let ((e32 (! (not e22) :named term32)))
(let ((e33 (! (or e30 e13) :named term33)))
(let ((e34 (! (ite e31 e17 e15) :named term34)))
(let ((e35 (! (ite e16 e25 e23) :named term35)))
(let ((e36 (! (not e33) :named term36)))
(let ((e37 (! (xor e24 e26) :named term37)))
(let ((e38 (! (=> e27 e37) :named term38)))
(let ((e39 (! (or e28 e34) :named term39)))
(let ((e40 (! (= e18 e32) :named term40)))
(let ((e41 (! (= e40 e36) :named term41)))
(let ((e42 (! (not e38) :named term42)))
(let ((e43 (! (not e41) :named term43)))
(let ((e44 (! (not e43) :named term44)))
(let ((e45 (! (= e44 e44) :named term45)))
(let ((e46 (! (and e35 e35) :named term46)))
(let ((e47 (! (= e39 e45) :named term47)))
(let ((e48 (! (not e46) :named term48)))
(let ((e49 (! (xor e42 e42) :named term49)))
(let ((e50 (! (or e49 e48) :named term50)))
(let ((e51 (! (or e47 e50) :named term51)))
e51
))))))))))))))))))))))))))))))))))))))))))))))))

(check-sat)
(set-option :regular-output-channel "/dev/null")
(get-model)
(get-value (term6))
(get-value (term7))
(get-value (term8))
(get-value (term9))
(get-value (term10))
(get-value (term11))
(get-value (term12))
(get-value (term13))
(get-value (term14))
(get-value (term15))
(get-value (term16))
(get-value (term17))
(get-value (term18))
(get-value (term19))
(get-value (term20))
(get-value (term21))
(get-value (term22))
(get-value (term23))
(get-value (term24))
(get-value (term25))
(get-value (term26))
(get-value (term27))
(get-value (term28))
(get-value (term29))
(get-value (term30))
(get-value (term31))
(get-value (term32))
(get-value (term33))
(get-value (term34))
(get-value (term35))
(get-value (term36))
(get-value (term37))
(get-value (term38))
(get-value (term39))
(get-value (term40))
(get-value (term41))
(get-value (term42))
(get-value (term43))
(get-value (term44))
(get-value (term45))
(get-value (term46))
(get-value (term47))
(get-value (term48))
(get-value (term49))
(get-value (term50))
(get-value (term51))
(get-info :all-statistics)
