(set-info :source |fuzzsmt|)
(set-info :smt-lib-version 2.0)
(set-info :category "random")
(set-info :status unknown)
(set-logic QF_IDL)
(declare-fun v0 () Int)
(declare-fun v1 () Int)
(declare-fun v2 () Int)
(assert (let ((e3 4))
(let ((e4 1))
(let ((e5 (> (- v0 v2) (- e3))))
(let ((e6 (distinct v0 v0)))
(let ((e7 (< v1 v2)))
(let ((e8 (>= (- v1 v1) e3)))
(let ((e9 (<= (- v2 v2) e4)))
(let ((e10 (< (- v2 v0) (- e4))))
(let ((e11 (>= v0 v0)))
(let ((e12 (< (- v0 v2) (- e4))))
(let ((e13 (> v0 v2)))
(let ((e14 (= (- v0 v2) e4)))
(let ((e15 (<= v0 v1)))
(let ((e16 (> v0 v2)))
(let ((e17 (distinct v0 v1)))
(let ((e18 (or e17 e12)))
(let ((e19 (not e18)))
(let ((e20 (xor e13 e5)))
(let ((e21 (xor e10 e8)))
(let ((e22 (or e9 e7)))
(let ((e23 (= e6 e14)))
(let ((e24 (= e16 e20)))
(let ((e25 (= e19 e11)))
(let ((e26 (xor e15 e25)))
(let ((e27 (or e23 e23)))
(let ((e28 (or e26 e21)))
(let ((e29 (= e22 e28)))
(let ((e30 (or e24 e29)))
(let ((e31 (and e27 e30)))
e31
))))))))))))))))))))))))))))))

(check-sat)