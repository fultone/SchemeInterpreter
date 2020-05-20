;; Given a value val and an index n, returns #t if val is a prime that is greater
;; than the nth prime, #f if not
(define is-prime-greater-or-equal-to-nth?
  (lambda (val n)
    (cond [(<= n 0) #f] ; 0th or lower prime is nonsensical
          [(< val (nth (primes) n)) #f]
          [(equal? val (nth (primes) n)) #t]
          [else (is-prime-greater-or-equal-to-nth? val (+ n 1))])))