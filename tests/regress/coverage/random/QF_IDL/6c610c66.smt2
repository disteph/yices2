(set-info :source |fuzzsmt|)
(set-info :smt-lib-version 2.0)
(set-info :category "random")
(set-info :status unknown)
(set-logic QF_IDL)
(declare-fun v0 () Int)
(declare-fun v1 () Int)
(declare-fun v2 () Int)
(assert (let ((e3 4))
(let ((e4 (! 0 :named term4)))
(let ((e5 (! (<= v0 v1) :named term5)))
(let ((e6 (! (<= v1 v0) :named term6)))
(let ((e7 (! (distinct (- v0 v0) (- e3)) :named term7)))
(let ((e8 (! (= v0 v1) :named term8)))
(let ((e9 (! (>= v1 v1) :named term9)))
(let ((e10 (! (> (- v2 v1) e3) :named term10)))
(let ((e11 (! (>= v2 v0) :named term11)))
(let ((e12 (! (>= (- v0 v0) e4) :named term12)))
(let ((e13 (! (>= (- v1 v2) e3) :named term13)))
(let ((e14 (! (distinct v2 v2) :named term14)))
(let ((e15 (! (<= (- v2 v0) e4) :named term15)))
(let ((e16 (! (<= v2 v2) :named term16)))
(let ((e17 (! (< v2 v2) :named term17)))
(let ((e18 (! (= (- v1 v0) (- e3)) :named term18)))
(let ((e19 (! (= v1 v0) :named term19)))
(let ((e20 (! (xor e11 e9) :named term20)))
(let ((e21 (! (=> e7 e5) :named term21)))
(let ((e22 (! (ite e12 e17 e6) :named term22)))
(let ((e23 (! (ite e13 e14 e16) :named term23)))
(let ((e24 (! (xor e23 e23) :named term24)))
(let ((e25 (! (= e20 e10) :named term25)))
(let ((e26 (! (not e25) :named term26)))
(let ((e27 (! (xor e19 e21) :named term27)))
(let ((e28 (! (and e27 e27) :named term28)))
(let ((e29 (! (or e26 e15) :named term29)))
(let ((e30 (! (ite e29 e28 e28) :named term30)))
(let ((e31 (! (= e22 e8) :named term31)))
(let ((e32 (! (= e24 e30) :named term32)))
(let ((e33 (! (and e18 e31) :named term33)))
(let ((e34 (! (and e32 e32) :named term34)))
(let ((e35 (! (and e34 e33) :named term35)))
e35
))))))))))))))))))))))))))))))))))

(check-sat)
(set-option :regular-output-channel "/dev/null")
(get-model)
(get-value (term4))
(get-value (term5))
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
(get-info :all-statistics)
