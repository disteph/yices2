(set-info :source |fuzzsmt|)
(set-info :smt-lib-version 2.0)
(set-info :category "random")
(set-info :status unknown)
(set-logic QF_IDL)
(declare-fun v0 () Int)
(declare-fun v1 () Int)
(assert (let ((e2 999764216024949875))
(let ((e3 3578410580))
(let ((e4 (> (- v1 v1) (- e2))))
(let ((e5 (distinct (- v1 v0) (- e2))))
(let ((e6 (>= (- v0 v1) (- e3))))
(let ((e7 (> (- v1 v0) e2)))
(let ((e8 (= v0 v1)))
(let ((e9 (= v0 v0)))
(let ((e10 (= (- v1 v1) e2)))
(let ((e11 (<= (- v1 v0) e2)))
(let ((e12 (> v0 v1)))
(let ((e13 (> v1 v1)))
(let ((e14 (<= (- v0 v0) e2)))
(let ((e15 (< (- v1 v1) (- e2))))
(let ((e16 (<= (- v1 v0) e2)))
(let ((e17 (= (- v1 v1) (- e2))))
(let ((e18 (>= v1 v0)))
(let ((e19 (> (- v0 v0) (- e2))))
(let ((e20 (= (- v0 v1) (- e2))))
(let ((e21 (> (- v1 v0) e3)))
(let ((e22 (= v0 v1)))
(let ((e23 (< v0 v0)))
(let ((e24 (>= v1 v1)))
(let ((e25 (> v1 v0)))
(let ((e26 (<= (- v1 v1) (- e3))))
(let ((e27 (or e13 e23)))
(let ((e28 (and e10 e26)))
(let ((e29 (not e6)))
(let ((e30 (=> e9 e15)))
(let ((e31 (xor e14 e19)))
(let ((e32 (=> e17 e11)))
(let ((e33 (and e16 e30)))
(let ((e34 (or e8 e7)))
(let ((e35 (=> e4 e34)))
(let ((e36 (=> e31 e35)))
(let ((e37 (and e12 e12)))
(let ((e38 (and e32 e21)))
(let ((e39 (not e24)))
(let ((e40 (not e20)))
(let ((e41 (or e5 e5)))
(let ((e42 (and e28 e29)))
(let ((e43 (or e25 e18)))
(let ((e44 (=> e39 e27)))
(let ((e45 (not e36)))
(let ((e46 (=> e43 e38)))
(let ((e47 (xor e33 e33)))
(let ((e48 (or e40 e42)))
(let ((e49 (= e45 e48)))
(let ((e50 (= e44 e46)))
(let ((e51 (ite e50 e37 e49)))
(let ((e52 (and e47 e41)))
(let ((e53 (or e51 e51)))
(let ((e54 (=> e53 e52)))
(let ((e55 (and e22 e54)))
e55
)))))))))))))))))))))))))))))))))))))))))))))))))))))))

(check-sat)