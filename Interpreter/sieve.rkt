;; Takes two integers and returns an integer lazy list
;; containing the sequence of values first, first+1,..., last
(define seq
  (lambda (first last)
    (if (> first last)
        #f
        (cons first
            (lambda () (seq (+ first 1) last))))))

;; Takes an integer and returns an integer lazy list
;; containing the infinite sequence of values first, first+1, ...
(define inf-seq
  (lambda (first)
    (cons first
            (lambda () (inf-seq (+ first 1))))))

;; Takes a lazy list and an integer and returns an ordinary
;; Scheme list containing the first n values in the lazy list.
;; If fewer than n values in lazy-list, all values are returned.
;; If the lazy list is empty, returns an empty Scheme list.
(define first-n
  (lambda (lazy-list n)
    (cond [(null? lazy-list) (quote ())]
          [(equal? n 0) (quote ())]
          [(and (if (equal? n 0) #f #t) (equal? lazy-list #f)) (quote ())]
          [else (cons (car lazy-list) (first-n ((cdr lazy-list)) (- n 1)))])))

;; Takes a lazy list and an integer n and returns the nth value in the
;; lazy list (counting from 1). Returns #f if the lazy list contains fewer
;; than n values.
(define nth
  (lambda (lazy-list n)
    (cond [(< n 1) #f]
          [(equal? lazy-list #f) #f]
          [(equal? n 1) (car lazy-list)]
          [else (nth ((cdr lazy-list)) (- n 1))])))

;; Returns a new lazy list that has n and all integer multiples of n removed
(define filter-multiples
  (lambda (lazy-list n)
    ;(cond [(or (equal? n 0) (if (integer? n) #f #t)) #f] ;; if n is not a positive integer, return #f
    (cond [(equal? n 0) #f]
          [(equal? lazy-list #f) #f]
          [(equal? (modulo (car lazy-list) n) 0) (filter-multiples ((cdr lazy-list)) n)]
          [else (cons (car lazy-list)
                      (lambda () (filter-multiples ((cdr lazy-list)) n)))])))

;; Sieves the first item from the rest of a lazy list, returning a new lazy list containing
;; the first item but none of its multiples. Does this recursively for each item in the
;; list not yet removed
(define sieve
  (lambda (lazy-list)
    (if (equal? ((cdr lazy-list)) #f)
        (cons (car lazy-list)
              (lambda () #f))
        (cons (car lazy-list)
              (lambda () (filter-multiples (sieve ((cdr lazy-list))) (car lazy-list)))))))

;; Returns an infinite lazy list of all prime numbers
(define primes
  (lambda ()
    (sieve (inf-seq 2))))

;; Given a value val and an index n, returns #t if val is a prime that is greater
;; than the nth prime, #f if not
(define is-prime-greater-or-equal-to-nth?
  (lambda (val n)
    (cond [(<= n 0) #f] ; 0th or lower prime is nonsensical
          [(< val (nth (primes) n)) #f]
          [(equal? val (nth (primes) n)) #t]
          [else (is-prime-greater-or-equal-to-nth? val (+ n 1))])))

;; Given a value, returns #t if the value is a prime number, #f otherwises
(define is-prime?
  (lambda (val)
    (is-prime-greater-or-equal-to-nth? val 1)))

;; Takes in a single integer and returns the number of primes less than that number
(define count-smaller-primes
  (lambda (n)
    (cond [(or (< n 1) (= n 1)) 0]
          [(is-prime? (- n 1)) (+ (count-smaller-primes (- n 1)) 1)]
          [else (count-smaller-primes (- n 1))])))

;; Given an index n, returns an infinite lazy list of twin primes among primes
;; after and including the nth prime
(define twin-primes-after-nth
  (lambda (n)
    (if (equal? (nth (primes) n) (- (nth (primes) (+ n 1)) 2))
        (cons (cons (nth (primes) n)
                    (nth (primes) (+ n 1)))
              (lambda () (twin-primes-after-nth (+ n 1))))
        (twin-primes-after-nth (+ n 1)))))

;; returns an infinite lazy list of twin primes
(define twin-primes
  (lambda ()
    (twin-primes-after-nth 1)))

(first-n (primes) 5)
