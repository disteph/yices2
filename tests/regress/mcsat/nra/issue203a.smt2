(set-logic QF_NRA)
(declare-fun x1 () Real)
(declare-fun x2 () Real)
(declare-fun x3 () Real)
(declare-fun x4 () Real)
(declare-fun x5 () Real)
(declare-const r1 Real)
(declare-const r2 Real)
(declare-const r7 Real)
(assert (<= 1 (* r2 1 (/ (* 3000 r1 (+ 1500 r1)) r2)) 1 (- r7) x3))
(assert (or (< r1 r2 x4 0) (< r1 r2 (/ (+ 1500 r1) (* r2 (/ (* 3000 r1 (+ 1500 r1)) r2))) 0)))
(assert (<= x1 1 (- r7) x5))
(check-sat)
