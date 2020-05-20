
(define A
  (lambda (k x1 x2 x3 x4 x5)
    (letrec ((B
              (lambda ()
                (begin
                  (set! k (- k 1))
                  (set! z (+ z 1))
                  (A k B x1 x2 x3 x4)))))
      (if (<= k 0)
          (begin
            (set! y (+ y 1))
            (+ (x4) (x5)))
          (B)))))

(A 10 (lambda () 1) (lambda () -1) (lambda () -1) (lambda () 1) (lambda () 0))
