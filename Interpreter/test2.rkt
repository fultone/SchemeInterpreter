(define tears
    (lambda (n L)
        (cond ((null? L) ())
            ((or (< n 0) (= n 0)) ())
            (else (cons (car L) (tears (- n 1) (cdr L)))))))

(tears 9 (quote (1 2 3 4 5 6 7)))

(letrec ((sum (lambda (x)
    (if (= x 0)
        0
        (+ x (sum (- x 1)))))))
    (sum 3))

;(letrec ((countdown (lambda (i)
;                        (if (= i 0) (quote liftoff)
;                            (begin
;                                (display i)
;                                (countdown (- i 1)))))))
;    (countdown 10))
